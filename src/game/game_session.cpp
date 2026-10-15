#include "game/game_session.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <boost/asio/co_spawn.hpp>
#include <exception>
#include <memory>
#include <stdexcept>
#include <utility>

#include "features/authentication/password_credentials.h"
#include "protocol/character_messages.h"
#include "protocol/inventory_messages.h"
#include "protocol/lobby_messages.h"
#include "protocol/login_messages.h"
#include "protocol/main_menu_messages.h"
#include "protocol/shop_messages.h"
#include "protocol/user_stat_messages.h"

namespace bubblefighter::game {
void GameSession::RefreshCharacter(std::uint32_t character_id) {
  if (!stopped_ && initial_info_received_ && character_ &&
      character_->id == character_id) {
    QueuePacket({825, {}});
  }
}

void GameSession::SendUserStats(const characters::CharacterStats& stats) {
  character_->level = stats.progress.level;
  if (const auto room =
          rooms_.UpdateLevel(character_->id, stats.progress.level);
      room && room->setting.state == 2) {
    for (std::size_t slot = 0; slot < room->members.size(); ++slot) {
      const auto& member = room->members[slot];
      if (member.profile.character &&
          member.profile.character->id == character_->id) {
        BroadcastRoom(*room,
                      protocol::CreateRoomSlotReply(
                          static_cast<std::uint32_t>(slot), member.profile));
      }
    }
  }
  if (const auto plaza =
          plazas_.UpdateLevel(character_->id, stats.progress.level)) {
    BroadcastPlaza(plaza->plaza,
                   protocol::CreateRoomSlotReply(
                       plaza->slot, plaza->plaza.members[plaza->slot].profile));
  }
  connection_.Send(protocol::CreateUserStatReply(*character_, stats));
}

namespace {
class InventoryActivity {
 public:
  InventoryActivity(RoomDirectory& rooms, std::uint64_t session_id,
                    std::uint32_t character_id)
      : rooms_(rooms),
        session_id_(session_id),
        character_id_(character_id),
        accepted_(rooms.SetInventoryBusy(session_id, true, character_id)) {}

  ~InventoryActivity() {
    if (accepted_) {
      rooms_.SetInventoryBusy(session_id_, false, character_id_);
    }
  }

  bool IsAccepted() const { return accepted_; }

 private:
  RoomDirectory& rooms_;
  std::uint64_t session_id_;
  std::uint32_t character_id_;
  bool accepted_;
};
}  // namespace

GameSession::GameSession(
    boost::asio::io_context& context,
    authentication::AccountRepository& accounts,
    characters::CharacterRepository& characters,
    inventory::InventoryRepository& inventory, shop::ShopRepository& shop,
    ChannelDirectory& channels, ChatDirectory& chats, RoomDirectory& rooms,
    GameRoundCoordinator& rounds, PlazaDirectory& plazas,
    std::uint64_t session_id, std::string peer_address,
    boost::asio::ip::tcp::endpoint server_endpoint,
    ClientConnection::SendHandler send, std::function<void()> disconnect,
    std::function<void()> finish,
    std::function<bool(std::uint64_t)> complete_transfer,
    std::function<void(std::uint64_t, const protocol::LambdaPacket&)> notify,
    std::function<void(std::uint64_t)> return_to_lobby)
    : context_(context),
      accounts_(accounts),
      characters_(characters),
      inventory_(inventory),
      shop_(shop),
      channels_(channels),
      chats_(chats),
      rooms_(rooms),
      rounds_(rounds),
      plazas_(plazas),
      session_id_(session_id),
      peer_address_(std::move(peer_address)),
      server_endpoint_(server_endpoint),
      disconnect_(std::move(disconnect)),
      finish_(std::move(finish)),
      complete_transfer_(std::move(complete_transfer)),
      notify_(std::move(notify)),
      return_to_lobby_(std::move(return_to_lobby)),
      connection_(std::move(send),
                  [this](const protocol::LambdaPacket& packet) {
                    QueuePacket(packet);
                  }),
      inventory_timer_(context) {}

void GameSession::Start() { connection_.Start(); }

void GameSession::Receive(std::span<const std::uint8_t> bytes) {
  if (stopped_) {
    return;
  }
  const auto initialized = connection_.HasCompletedHandshake();
  connection_.Receive(bytes);
  if (!initialized && connection_.HasCompletedHandshake()) {
    spdlog::info("[Protocol] session={} initial protocol request accepted",
                 session_id_);
  }
}

void GameSession::Stop() {
  if (stopped_) {
    return;
  }
  stopped_ = true;
  ++inventory_timer_generation_;
  inventory_timer_.cancel();
  inventory_refresh_pending_ = false;
  channels_.Cancel(session_id_);
  chats_.Leave(session_id_);
  LeaveRoom();
  LeavePlaza();
  packets_.clear();
  queued_bytes_ = 0;
}

bool GameSession::FinishTransfer() {
  if (stopped_) {
    return false;
  }

  // The original S10 handler promotes the second connection before closing this
  // socket.
  connection_.Send({10, {}});
  Stop();
  finish_();
  return true;
}

void GameSession::SendNotification(const protocol::LambdaPacket& packet) {
  if (!stopped_) {
    connection_.Send(packet);
  }
}

void GameSession::QueuePacket(const protocol::LambdaPacket& packet) {
  if (packets_.size() >= 128 ||
      packet.payload.size() > 1024 * 1024 - queued_bytes_) {
    throw std::runtime_error("Client packet queue limit exceeded");
  }
  packets_.push_back(packet);
  queued_bytes_ += packet.payload.size();
  StartProcessing();
}

void GameSession::ReturnToLobby() {
  if (!stopped_ && initial_info_received_ && character_) {
    // Internal state transition, ordered after any outstanding database
    // operation and before queued requests.
    lobby_entry_pending_ = true;
    StartProcessing();
  }
}

void GameSession::StartProcessing() {
  if (processing_) {
    return;
  }
  processing_ = true;
  // Keep the session alive until outstanding database operations finish, even
  // after socket closure.
  boost::asio::co_spawn(
      context_, ProcessPackets(),
      [self = shared_from_this()](std::exception_ptr failure) {
        if (failure && !self->stopped_) {
          spdlog::warn("[Game] session={} request failed; closing connection",
                       self->session_id_);
          self->Stop();
          self->disconnect_();
        }
      });
}

boost::asio::awaitable<void> GameSession::ProcessPackets() {
  while (!stopped_ && (lobby_entry_pending_ || inventory_refresh_pending_ ||
                       !packets_.empty())) {
    if (lobby_entry_pending_) {
      lobby_entry_pending_ = false;
      co_await EnterLobby();
      continue;
    }
    if (inventory_refresh_pending_) {
      inventory_refresh_pending_ = false;
      co_await RefreshExpiredInventory();
      continue;
    }

    auto packet = std::move(packets_.front());
    packets_.pop_front();
    queued_bytes_ -= packet.payload.size();
    const auto message_id = packet.message_id;
    const auto payload_bytes = packet.payload.size();
    try {
      co_await HandlePacket(std::move(packet));
    } catch (const std::invalid_argument& error) {
      spdlog::warn(
          "[Protocol] session={} messageId={} payloadBytes={} rejected: {}",
          session_id_, message_id, payload_bytes, error.what());
      throw;
    } catch (...) {
      spdlog::warn(
          "[Game] session={} messageId={} payloadBytes={} processing failed",
          session_id_, message_id, payload_bytes);
      throw;
    }
  }
  processing_ = false;
}

boost::asio::awaitable<std::vector<inventory::InventoryItem>>
GameSession::ReadInventory(bool synchronize_changes) {
  const auto query_started = inventory::InventoryExpiry::Clock::now();
  auto items = co_await inventory_.FindByCharacter(character_->id);
  const auto presets = co_await inventory_.ReadWeaponPresets(character_->id);
  if (!stopped_) {
    const auto changed = inventory_expiry_.Observe(items, query_started);
    inventory_refresh_pending_ = false;
    ScheduleInventoryCheck(inventory_expiry_.NextCheck());
    if (!synchronize_changes || (changed && initial_info_received_)) {
      connection_.Send(
          protocol::CreateWeaponPresetsReply(account_->id, presets));
    }
    if (synchronize_changes && changed && initial_info_received_) {
      connection_.Send(protocol::CreateInventoryReply(items));
      connection_.Send(protocol::CreateEquipmentReply(
          account_->id, character_->character_type, items));
      RefreshRoomInventory(items, true);
      spdlog::info(
          "[Inventory] session={} characterId={} online inventory refreshed items={}",
          session_id_, character_->id, items.size());
    }
  }
  co_return items;
}

void GameSession::ScheduleInventoryCheck(
    std::optional<inventory::InventoryExpiry::Clock::time_point> deadline) {
  const auto generation = ++inventory_timer_generation_;
  inventory_timer_.cancel();
  if (stopped_ || !deadline) {
    return;
  }

  inventory_timer_.expires_at(std::max(
      *deadline,
      inventory::InventoryExpiry::Clock::now() + std::chrono::seconds(1)));
  inventory_timer_.async_wait([weak = weak_from_this(), generation](
                                  const boost::system::error_code& error) {
    const auto self = weak.lock();
    if (error || !self || self->stopped_ ||
        generation != self->inventory_timer_generation_) {
      return;
    }
    self->inventory_refresh_pending_ = true;
    self->StartProcessing();
  });
}

boost::asio::awaitable<void> GameSession::RefreshExpiredInventory() {
  if (!initial_info_received_ || !character_) {
    co_return;
  }
  const InventoryActivity activity(rooms_, session_id_, character_->id);
  if (channels_.HasPending(session_id_) || !activity.IsAccepted()) {
    // A round keeps its frozen equipment through results. Do no database work
    // until the guard is available.
    ScheduleInventoryCheck(inventory::InventoryExpiry::Clock::now() +
                           std::chrono::seconds(1));
    co_return;
  }
  co_await ReadInventory(true);
}

boost::asio::awaitable<void> GameSession::HandlePacket(
    protocol::LambdaPacket packet) {
  if (!account_) {
    if (packet.message_id == 14) {
      co_await HandleMoveInto(packet);
      co_return;
    }
    auto token = protocol::ReadLoginTicket(packet);
    const auto identity =
        co_await accounts_.ConsumeLoginTicket(std::move(token));
    if (stopped_) {
      co_return;
    }
    if (!identity) {
      connection_.Send(protocol::CreateLoginReply(1, 0, u"", 0, 0));
      spdlog::warn("[Authentication] session={} login ticket rejected",
                   session_id_);
      co_return;
    }
    account_ = identity;
    character_ = co_await characters_.FindByAccount(account_->id);
    if (stopped_) {
      co_return;
    }
    if (character_) {
      SendLoginReply();
    } else {
      connection_.Send({12, {}});
    }
    spdlog::info(
        "[Authentication] session={} accountId={} login ticket accepted",
        session_id_, account_->id);
    co_return;
  }
  if (!login_reply_sent_) {
    co_await HandleCreation(std::move(packet));
    co_return;
  }
  if (!initial_info_received_) {
    protocol::ValidateInitialInfo(packet, first_challenge_, second_challenge_);
    const auto items = co_await ReadInventory();
    const auto stats = co_await characters_.FindStats(character_->id);
    if (stopped_) {
      co_return;
    }
    SendUserStats(stats);
    connection_.Send(protocol::CreateInventoryReply(items));
    connection_.Send(protocol::CreateEquipmentReply(
        account_->id, character_->character_type, items));
    connection_.Send(protocol::CreateSoloPartyReply(*character_, items));
    connection_.Send(protocol::CreateMainMenuReply(*character_, items));
    initial_info_received_ = true;
    RegisterChat();
    spdlog::info(
        "[Game] session={} accountId={} InitialInfo accepted; inventoryItems={} main menu sent",
        session_id_, account_->id, items.size());
    co_return;
  }
  if (packet.message_id == 3 || packet.message_id == 14 ||
      packet.message_id == 102 || packet.message_id == 15 ||
      packet.message_id == 16 || packet.message_id == 17) {
    throw std::invalid_argument(
        "Login initialization cannot be repeated on an authenticated connection");
  }
  if (channels_.HasPending(session_id_)) {
    // Freeze business work while identity is in transit; no old-connection
    // mutation may overlap adoption.
    co_return;
  }
  if (packet.message_id == 12 || packet.message_id == 13) {
    HandleChannelRequest(packet);
    co_return;
  }
  if (packet.message_id == 105 || packet.message_id == 106) {
    if (!packet.payload.empty()) {
      throw std::invalid_argument(
          "Scene entry request must have an empty payload");
    }
    if (packet.message_id == 105) {
      co_await EnterMainMenu();
    } else {
      co_await EnterLobby();
    }
    co_return;
  }
  if (packet.message_id == 511 || packet.message_id == 521) {
    HandlePlazaRequest(packet);
    co_return;
  }
  if (packet.message_id == 501 || packet.message_id == 502 ||
      packet.message_id == 304) {
    HandleSessionChat(packet);
    co_return;
  }
  if (packet.message_id == 401) {
    SendRoomList(packet);
    co_return;
  }
  if (packet.message_id == 503) {
    const auto request = protocol::ReadGameStartRequest(packet);
    const auto room = rooms_.FindBySession(session_id_);
    if (room && room->setting.state == 2 &&
        room->members[room->master_slot].session_id == session_id_) {
      // Freeze inventory operations while rereading expiration and appearance
      // before a new round.
      std::vector<std::unique_ptr<InventoryActivity>> activities;
      for (const auto& member : room->members) {
        if (member.session_id == 0) {
          continue;
        }
        auto activity = std::make_unique<InventoryActivity>(
            rooms_, member.session_id, member.profile.character->id);
        if (!activity->IsAccepted()) {
          connection_.Send(protocol::CreateGameEntryReply(
              protocol::GameStartResult::kNotReady, u"", 2));
          co_return;
        }
        activities.push_back(std::move(activity));
      }
      for (std::size_t slot = 0; slot < room->members.size(); ++slot) {
        const auto& member = room->members[slot];
        if (member.session_id == 0) {
          continue;
        }
        const auto items =
            co_await inventory_.FindByCharacter(member.profile.character->id);
        const auto current = rooms_.FindBySession(session_id_);
        if (stopped_ || !current ||
            current->setting.room_id != room->setting.room_id ||
            current->members[slot].session_id != member.session_id ||
            current->setting.state != 2) {
          co_return;
        }
        if (!std::equal(items.begin(), items.end(),
                        member.profile.items.begin(),
                        member.profile.items.end(),
                        [](const auto& left, const auto& right) {
                          return left.item_id == right.item_id &&
                                 left.quantity == right.quantity &&
                                 left.equipped_slot == right.equipped_slot;
                        })) {
          const auto updated = rooms_.UpdateInventory(
              member.session_id, items, member.profile.character->id);
          if (updated) {
            notify_(member.session_id, protocol::CreateInventoryReply(items));
            notify_(member.session_id,
                    protocol::CreateEquipmentReply(
                        member.profile.character->account_id,
                        member.profile.character->character_type, items));
            BroadcastRoom(*updated, protocol::CreateRoomSlotReply(
                                        static_cast<std::uint32_t>(slot),
                                        updated->members[slot].profile));
          }
        }
      }
    }
    spdlog::debug("[Game] session={} start request bytes={} legacyValue156={}",
                  session_id_, packet.payload.size(), request.value156);
    rounds_.Begin(session_id_, request);
    co_return;
  }
  if (packet.message_id == 601 || packet.message_id == 607 ||
      packet.message_id == 701 || packet.message_id == 2015 ||
      packet.message_id == 2017 || packet.message_id == 713 ||
      packet.message_id == 704) {
    rounds_.HandleReport(session_id_, packet);
    co_return;
  }
  if (packet.message_id == 517 || packet.message_id == 518) {
    HandleSessionPing(packet);
    co_return;
  }
  if (packet.message_id == 513) {
    HandleRoomGameSetting(packet);
    co_return;
  }
  if (packet.message_id == 510) {
    HandleRoomKick(packet);
    co_return;
  }
  if (packet.message_id == 402 || packet.message_id == 403 ||
      packet.message_id == 404 || packet.message_id == 505 ||
      packet.message_id == 515) {
    co_await HandleRoomRequest(std::move(packet));
    co_return;
  }
  if (packet.message_id == 801) {
    const auto category = protocol::ReadShopEntry(packet);
    const auto catalog = co_await shop_.ReadCatalog();
    if (!stopped_) {
      connection_.Send(protocol::CreateShopReply(catalog, category));
    }
    co_return;
  }
  if (packet.message_id == 808) {
    const auto request = protocol::ReadShopPurchase(packet);
    const InventoryActivity activity(rooms_, session_id_, character_->id);
    const auto result =
        request.alternate_purchase || !activity.IsAccepted()
            ? shop::PurchaseResult::kUnavailable
            : co_await shop_.Purchase(character_->id, request.stock_id);
    if (result == shop::PurchaseResult::kSuccess) {
      const auto items = co_await ReadInventory();
      const auto stats = co_await characters_.FindStats(character_->id);
      if (!stopped_) {
        SendUserStats(stats);
        connection_.Send(protocol::CreateInventoryReply(items));
        connection_.Send(protocol::CreateEquipmentReply(
            account_->id, character_->character_type, items));
        RefreshRoomInventory(items);
      }
    }
    if (!stopped_) {
      connection_.Send(
          protocol::CreateShopPurchaseReply(request.stock_id, result));
    }
    spdlog::info("[Shop] session={} characterId={} stockId={} result={}",
                 session_id_, character_->id, request.stock_id,
                 static_cast<std::uint32_t>(result));
    co_return;
  }
  if (packet.message_id == 825) {
    if (!packet.payload.empty()) {
      throw std::invalid_argument("ReqCheckCash must have an empty payload");
    }
    const auto stats = co_await characters_.FindStats(character_->id);
    if (!stopped_) {
      SendUserStats(stats);
    }
    co_return;
  }
  if (packet.message_id == 830) {
    const auto index = protocol::ReadWeaponPresetRequest(packet);
    const InventoryActivity activity(rooms_, session_id_, character_->id);
    const bool success =
        activity.IsAccepted() &&
        co_await inventory_.SelectWeaponPreset(character_->id, index);
    if (!stopped_ && success) {
      const auto items = co_await ReadInventory();
      if (!stopped_) {
        connection_.Send(protocol::CreateInventoryReply(items));
        connection_.Send(protocol::CreateEquipmentReply(
            account_->id, character_->character_type, items));
        RefreshRoomInventory(items);
      }
    }
    if (!stopped_) {
      connection_.Send(protocol::CreateWeaponPresetResult(index, success));
    }
    spdlog::info(
        "[Inventory] session={} characterId={} weaponPreset={} success={}",
        session_id_, character_->id, index, success);
    co_return;
  }
  if (packet.message_id == 802 || packet.message_id == 803 ||
      packet.message_id == 804) {
    const InventoryActivity activity(rooms_, session_id_, character_->id);
    if (packet.message_id == 802 && !packet.payload.empty()) {
      throw std::invalid_argument("EnterInventory must have an empty payload");
    }
    if (packet.message_id == 802 && !activity.IsAccepted()) {
      // Do not normalize or replace equipment while this character belongs to a
      // frozen round.
      co_return;
    }
    if (packet.message_id != 802) {
      const auto item_id = protocol::ReadEquipmentRequest(packet);
      const auto equipped = packet.message_id == 803;
      if (!activity.IsAccepted()) {
        connection_.Send(protocol::CreateEquipmentResult(equipped, false));
        co_return;
      }
      const auto success =
          co_await inventory_.SetEquipped(character_->id, item_id, equipped);
      if (stopped_) {
        co_return;
      }
      if (!success) {
        connection_.Send(protocol::CreateEquipmentResult(equipped, false));
        co_return;
      }
    }

    const auto items = co_await ReadInventory();
    if (stopped_) {
      co_return;
    }
    connection_.Send(protocol::CreateInventoryReply(items));
    connection_.Send(protocol::CreateEquipmentReply(
        account_->id, character_->character_type, items));
    RefreshRoomInventory(items);
    if (packet.message_id != 802) {
      // The original result handler can initiate tutorial actions; update
      // appearance before acknowledging.
      connection_.Send(
          protocol::CreateEquipmentResult(packet.message_id == 803, true));
    }
    co_return;
  }
  spdlog::info(
      "[Protocol] session={} messageId={} payloadBytes={} awaiting business handler",
      session_id_, packet.message_id, packet.payload.size());
}

boost::asio::awaitable<void> GameSession::HandleCreation(
    protocol::LambdaPacket packet) {
  if (packet.message_id == 15 && !character_) {
    const auto nickname = protocol::ReadNicknameRequest(packet);
    const auto result = co_await characters_.CheckName(nickname);
    if (!stopped_) {
      connection_.Send(protocol::CreateNicknameReply(result, nickname));
    }
    co_return;
  }
  if (packet.message_id == 16 && !character_) {
    const auto request = protocol::ReadCreateCharacterRequest(packet);
    const auto result = co_await characters_.Create(
        account_->id, request.nickname, request.character_type,
        request.starter_selection);
    if (result == characters::CreationResult::kSuccess) {
      character_ = co_await characters_.FindByAccount(account_->id);
      if (!character_) {
        throw std::runtime_error("Created character could not be loaded");
      }
    }
    if (!stopped_) {
      connection_.Send(protocol::CreateCharacterReply(result));
      spdlog::info("[Character] session={} accountId={} creation result={}",
                   session_id_, account_->id,
                   static_cast<std::uint32_t>(result));
    }
    co_return;
  }
  if (packet.message_id == 17 && packet.payload.empty() && character_) {
    SendLoginReply();
    co_return;
  }
  throw std::invalid_argument("Unexpected packet during character creation");
}

void GameSession::SendLoginReply() {
  const auto challenge = authentication::CreateLoginToken();
  first_challenge_ = static_cast<std::uint32_t>(
      std::stoul(challenge.substr(0, 8), nullptr, 16));
  second_challenge_ = static_cast<std::uint32_t>(
      std::stoul(challenge.substr(8, 8), nullptr, 16));
  const std::u16string account_name(account_->account_name.begin(),
                                    account_->account_name.end());
  connection_.Send(protocol::CreateLoginReply(
      0, account_->id, account_name, first_challenge_, second_challenge_,
      character_->nickname));
  login_reply_sent_ = true;
}

void GameSession::HandleChannelRequest(const protocol::LambdaPacket& packet) {
  const auto request = protocol::ReadChannelRequest(packet);
  if (rooms_.FindBySession(session_id_) ||
      !server_endpoint_.address().is_v4() ||
      !ChannelDirectory::Contains(request.channel_type,
                                  request.channel_index)) {
    connection_.Send(protocol::CreateChannelReply(1, {}, 0));
    return;
  }

  ChannelTransfer transfer{*account_,
                           *character_,
                           session_id_,
                           request.channel_type,
                           request.channel_index,
                           inventory_expiry_};
  const auto prepared = channels_.Prepare(std::move(transfer), peer_address_);
  connection_.Send(protocol::CreateChannelReply(
      prepared ? 0 : 1, server_endpoint_.address().to_v4().to_bytes(),
      server_endpoint_.port()));
  spdlog::info("[Channel] session={} transfer prepared={} type={} index={}",
               session_id_, prepared, request.channel_type,
               request.channel_index);
}

boost::asio::awaitable<void> GameSession::HandleMoveInto(
    const protocol::LambdaPacket& packet) {
  const auto request = protocol::ReadMoveIntoRequest(packet);
  auto transfer =
      channels_.Consume(request.account_id, peer_address_, session_id_);
  if (!transfer || !complete_transfer_(transfer->source_session_id)) {
    connection_.Send(protocol::CreateMoveIntoReply(false, 0, 0));
    Stop();
    finish_();
    co_return;
  }

  account_ = std::move(transfer->account);
  character_ = std::move(transfer->character);
  channel_type_ = transfer->channel_type;
  channel_index_ = transfer->channel_index;
  inventory_expiry_ = std::move(transfer->inventory_expiry);
  ScheduleInventoryCheck(inventory_expiry_.NextCheck());
  login_reply_sent_ = true;
  initial_info_received_ = true;
  RegisterChat();
  connection_.Send(
      protocol::CreateMoveIntoReply(true, channel_type_, channel_index_));
  if (request.enter_lobby) {
    co_await EnterLobby();
  }
  spdlog::info(
      "[Channel] session={} adopted accountId={} type={} index={} lobby={}",
      session_id_, account_->id, channel_type_, channel_index_, in_lobby_);
}
}  // namespace bubblefighter::game
