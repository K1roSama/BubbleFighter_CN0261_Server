#ifndef BUBBLEFIGHTER_SERVER_GAME_GAME_SESSION_H_
#define BUBBLEFIGHTER_SERVER_GAME_GAME_SESSION_H_

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <deque>
#include <memory>

#include "features/authentication/account_repository.h"
#include "features/characters/character_repository.h"
#include "features/inventory/inventory_repository.h"
#include "features/shop/shop_repository.h"
#include "game/channel_directory.h"
#include "game/chat_directory.h"
#include "game/client_connection.h"
#include "game/game_round_coordinator.h"
#include "game/plaza_directory.h"
#include "game/room_directory.h"

namespace bubblefighter::game {
class GameSession : public std::enable_shared_from_this<GameSession> {
 public:
  GameSession(
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
      std::function<void(std::uint64_t)> return_to_lobby);
  void Start();
  void Receive(std::span<const std::uint8_t> bytes);
  void Stop();
  bool FinishTransfer();
  void SendNotification(const protocol::LambdaPacket& packet);
  void RefreshCharacter(std::uint32_t character_id);
  void ReturnToLobby();

 private:
  void QueuePacket(const protocol::LambdaPacket& packet);
  void StartProcessing();
  boost::asio::awaitable<void> ProcessPackets();
  boost::asio::awaitable<void> HandlePacket(protocol::LambdaPacket packet);
  boost::asio::awaitable<void> HandleCreation(protocol::LambdaPacket packet);
  boost::asio::awaitable<std::vector<inventory::InventoryItem>> ReadInventory(
      bool synchronize_changes = false);
  void ScheduleInventoryCheck(
      std::optional<inventory::InventoryExpiry::Clock::time_point> deadline);
  boost::asio::awaitable<void> RefreshExpiredInventory();
  void SendLoginReply();
  void SendUserStats(const characters::CharacterStats& stats);
  boost::asio::awaitable<void> HandleMoveInto(
      const protocol::LambdaPacket& packet);
  void HandleChannelRequest(const protocol::LambdaPacket& packet);
  boost::asio::awaitable<void> HandleRoomRequest(protocol::LambdaPacket packet);
  void SendRoomList(const protocol::LambdaPacket& packet);
  void HandleRoomGameSetting(const protocol::LambdaPacket& packet);
  void HandleRoomKick(const protocol::LambdaPacket& packet);
  void HandleSessionPing(const protocol::LambdaPacket& packet);
  void LeaveRoom();
  boost::asio::awaitable<void> EnterLobby();
  boost::asio::awaitable<void> EnterMainMenu();
  void LeavePlaza();
  void HandlePlazaRequest(const protocol::LambdaPacket& packet);
  void RegisterChat();
  bool HandleChatCommand(std::u16string_view text);
  void HandleSessionChat(const protocol::LambdaPacket& packet);
  void BroadcastPlaza(const PlazaSnapshot& plaza,
                      const protocol::LambdaPacket& packet,
                      std::uint64_t excluded_session = 0,
                      bool initialized_only = false);
  void RefreshRoomInventory(const std::vector<inventory::InventoryItem>& items,
                            bool notify_owner = false);
  void BroadcastRoom(const RoomSnapshot& room,
                     const protocol::LambdaPacket& packet,
                     std::uint64_t excluded_session = 0);

  boost::asio::io_context& context_;
  authentication::AccountRepository& accounts_;
  characters::CharacterRepository& characters_;
  inventory::InventoryRepository& inventory_;
  shop::ShopRepository& shop_;
  ChannelDirectory& channels_;
  ChatDirectory& chats_;
  ChatDirectory::Clock::time_point next_chat_notice_{};
  RoomDirectory& rooms_;
  GameRoundCoordinator& rounds_;
  PlazaDirectory& plazas_;
  std::uint64_t session_id_;
  std::string peer_address_;
  boost::asio::ip::tcp::endpoint server_endpoint_;
  std::function<void()> disconnect_;
  std::function<void()> finish_;
  std::function<bool(std::uint64_t)> complete_transfer_;
  std::function<void(std::uint64_t, const protocol::LambdaPacket&)> notify_;
  std::function<void(std::uint64_t)> return_to_lobby_;
  ClientConnection connection_;
  boost::asio::steady_timer inventory_timer_;
  inventory::InventoryExpiry inventory_expiry_;
  std::uint64_t inventory_timer_generation_ = 0;
  bool inventory_refresh_pending_ = false;
  std::deque<protocol::LambdaPacket> packets_;
  std::size_t queued_bytes_ = 0;
  bool processing_ = false;
  bool stopped_ = false;
  bool initial_info_received_ = false;
  bool login_reply_sent_ = false;
  bool in_lobby_ = false;
  bool lobby_entry_pending_ = false;
  std::uint32_t channel_type_ = 0;
  std::uint32_t channel_index_ = 0;
  std::optional<authentication::AccountIdentity> account_;
  std::optional<characters::CharacterProfile> character_;
  std::uint32_t first_challenge_ = 0;
  std::uint32_t second_challenge_ = 0;
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_GAME_SESSION_H_
