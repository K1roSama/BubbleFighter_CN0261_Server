#ifndef BUBBLEFIGHTER_SERVER_APPLICATION_SERVER_APPLICATION_H_
#define BUBBLEFIGHTER_SERVER_APPLICATION_SERVER_APPLICATION_H_

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <string>
#include <unordered_map>

#include "configuration/server_config.h"
#include "features/authentication/authentication_listener.h"
#include "game/game_session.h"
#include "network/tcp_listener.h"
#include "persistence/my_sql_database.h"

namespace bubblefighter::application {
class ServerApplication {
 public:
  ServerApplication(configuration::ServerConfig config,
                    const std::string& password);
  int Run();

 private:
  boost::asio::awaitable<void> Start();
  void Stop();
  void ConnectClient(std::uint64_t session_id);
  void ReceiveBytes(std::uint64_t session_id,
                    std::span<const std::uint8_t> bytes);
  void Notify(std::uint64_t session_id, const protocol::LambdaPacket& packet);
  boost::asio::awaitable<std::vector<game::SettledPlayer>> Settle(
      game::CompletedGame game);

  boost::asio::io_context context_{1};
  boost::asio::signal_set signals_;
  database::MySqlDatabase database_;
  authentication::AccountRepository accounts_;
  progression::ProgressionPolicy progression_;
  characters::CharacterRepository characters_;
  inventory::InventoryRepository inventory_;
  shop::ShopRepository shop_;
  game::GameSettlementRepository settlements_;
  game::ChannelDirectory channels_;
  game::ChatDirectory chats_;
  game::RoomDirectory rooms_;
  game::PlazaDirectory plazas_;
  bool relay_enabled_;
  relay::RelayDirectory relay_directory_;
  relay::RelayListener relay_listener_;
  game::GameRoundCoordinator rounds_;
  authentication::AuthenticationListener authentication_listener_;
  network::TcpListener listener_;
  std::unordered_map<std::uint64_t, std::shared_ptr<game::GameSession>>
      clients_;
  bool stopping_ = false;
  int exit_code_ = 0;
};
}  // namespace bubblefighter::application

#endif  // BUBBLEFIGHTER_SERVER_APPLICATION_SERVER_APPLICATION_H_
