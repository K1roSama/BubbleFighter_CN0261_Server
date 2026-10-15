#include <spdlog/spdlog.h>

#include <stdexcept>

#include "game/game_session.h"
#include "protocol/lobby_messages.h"
#include "protocol/main_menu_messages.h"

namespace bubblefighter::game {
boost::asio::awaitable<void> GameSession::EnterLobby() {
  if (plazas_.Find(session_id_)) {
    // An already-present client does not create a second slot or rebuild
    // everybody's scene.
    co_return;
  }
  auto items = co_await ReadInventory(true);
  if (stopped_) {
    co_return;
  }
  if (rooms_.ContainsCharacter(character_->id, session_id_) ||
      plazas_.ContainsCharacter(character_->id, session_id_)) {
    throw std::invalid_argument(
        "Character is already present on another game connection");
  }
  LeaveRoom();
  if (channel_type_ == 0) {
    channel_type_ = 60;
    channel_index_ = 0;
    connection_.Send(
        protocol::CreateMoveIntoReply(true, channel_type_, channel_index_));
  }
  RegisterChat();
  const auto admission =
      plazas_.Enter(session_id_, {character_, std::move(items), 1},
                    channel_type_, channel_index_);
  in_lobby_ = true;
  std::array<protocol::SessionMember, kPlazaCapacity> members;
  for (std::size_t slot = 0; slot < members.size(); ++slot) {
    members[slot] = admission.plaza.members[slot].profile;
  }
  const auto setting = admission.plaza.GetSetting();
  connection_.Send(protocol::CreateRoomEntryReply(
      setting, admission.plaza.master_slot, admission.slot, members));
  BroadcastPlaza(
      admission.plaza,
      protocol::CreateRoomSlotReply(admission.slot, members[admission.slot]),
      session_id_);
  BroadcastPlaza(admission.plaza, protocol::CreateRoomSettingReply(setting),
                 session_id_);
  spdlog::info("[Plaza] session={} instance={} channel={} slot={} entered",
               session_id_, admission.plaza.instance_id, channel_index_,
               admission.slot);
}

boost::asio::awaitable<void> GameSession::EnterMainMenu() {
  const auto items = co_await ReadInventory(true);
  if (stopped_ || lobby_entry_pending_) {
    co_return;
  }
  LeaveRoom();
  LeavePlaza();
  in_lobby_ = false;
  connection_.Send(protocol::CreateMainMenuReply(*character_, items));
}

void GameSession::BroadcastPlaza(const PlazaSnapshot& plaza,
                                 const protocol::LambdaPacket& packet,
                                 std::uint64_t excluded_session,
                                 bool initialized_only) {
  for (const auto& member : plaza.members) {
    if (member.session_id != 0 && member.session_id != excluded_session &&
        (!initialized_only || member.initialized)) {
      notify_(member.session_id, packet);
    }
  }
}

void GameSession::LeavePlaza() {
  const auto departure = plazas_.Leave(session_id_);
  if (!departure) {
    return;
  }
  BroadcastPlaza(departure->plaza,
                 protocol::CreateRoomDepartureReply(departure->slot));
  if (departure->changed) {
    BroadcastPlaza(departure->plaza, protocol::CreateRoomMasterReply(
                                         departure->plaza.master_slot));
  }
  BroadcastPlaza(departure->plaza, protocol::CreateRoomSettingReply(
                                       departure->plaza.GetSetting()));
  spdlog::info("[Plaza] session={} instance={} slot={} left", session_id_,
               departure->plaza.instance_id, departure->slot);
}

void GameSession::HandlePlazaRequest(const protocol::LambdaPacket& packet) {
  if (packet.message_id == 511) {
    const auto position = protocol::ReadPlazaPosition(packet);
    const auto movement = plazas_.Move(session_id_, position);
    if (movement && movement->plaza.members[movement->slot].initialized) {
      BroadcastPlaza(movement->plaza,
                     protocol::CreatePlazaPositionReply(account_->id, position),
                     session_id_, true);
    }
    return;
  }
  protocol::ValidateSessionInitialization(packet);
  const auto initialization = plazas_.Initialize(session_id_);
  if (!initialization || !initialization->changed) {
    return;
  }
  const auto& plaza = initialization->plaza;
  for (const auto& member : plaza.members) {
    if (member.session_id != session_id_ && member.initialized &&
        member.position) {
      connection_.Send(protocol::CreatePlazaPositionReply(
          member.profile.character->account_id, *member.position));
    }
  }
  if (const auto& position = plaza.members[initialization->slot].position) {
    BroadcastPlaza(plaza,
                   protocol::CreatePlazaPositionReply(account_->id, *position),
                   session_id_, true);
  }
  BroadcastPlaza(plaza,
                 protocol::CreateSessionInitializationReply(account_->id),
                 session_id_, true);
}

}  // namespace bubblefighter::game
