#include "application/server_application.h"

#include <spdlog/spdlog.h>

#include <boost/asio/co_spawn.hpp>
#include <csignal>
#include <exception>
#include <stdexcept>

namespace bubblefighter::application {
ServerApplication::ServerApplication(configuration::ServerConfig config,
                                     const std::string& password)
    : signals_(context_, SIGINT, SIGTERM),
      database_(context_, std::move(config.database), password),
      accounts_(database_),
      progression_(
          progression::ProgressionPolicy::Load(config.progression_file)),
      characters_(database_, progression_),
      inventory_(database_),
      shop_(database_),
      settlements_(database_, progression_),
      rooms_(game::GameMapCatalog::Load(config.game_maps_file)),
      relay_enabled_(config.relay_enabled),
      relay_listener_(context_, config.relay, relay_directory_),
      rounds_(
          context_, rooms_, relay_directory_, relay_listener_,
          game::GameWeaponCatalog::Load(config.game_weapons_file),
          game::GameDefenseCatalog::Load(config.game_defense_file),
          game::GameCharacterCatalog::Load(config.game_characters_file),
          game::GameRespawnCatalog::Load(config.game_respawn_file),
          progression_,
          {boost::asio::ip::make_address_v4(config.relay_advertised_address),
           config.relay_enabled ? config.relay.listen_port
                                : static_cast<std::uint16_t>(0)},
          [this](std::uint64_t session_id,
                 const protocol::LambdaPacket& packet) {
            Notify(session_id, packet);
          },
          [this](std::uint64_t session_id) {
            return listener_.GetPeerEndpoint(session_id).address().to_string();
          },
          [this](game::CompletedGame game) { return Settle(std::move(game)); }),
      authentication_listener_(context_, std::move(config.authentication),
                               config.network.listen_port, accounts_),
      listener_(
          context_, std::move(config.network),
          [this](std::uint64_t session_id,
                 std::span<const std::uint8_t> bytes) {
            ReceiveBytes(session_id, bytes);
          },
          [this](std::uint64_t session_id) { ConnectClient(session_id); },
          [this](std::uint64_t session_id) {
            if (const auto client = clients_.find(session_id);
                client != clients_.end()) {
              client->second->Stop();
            }
            clients_.erase(session_id);
          }) {
  signals_.add(SIGBREAK);
  relay_listener_.SetHandlers(
      [this](const relay::RelayMember& member,
             const protocol::LambdaPacket& packet) {
        rounds_.HandleRelay(member, packet);
      },
      [this](std::uint64_t session_id) {
        rounds_.CancelForSession(session_id);
      });
}

int ServerApplication::Run() {
  signals_.async_wait([this](boost::system::error_code error, int signal) {
    if (!error) {
      spdlog::info("[Application] received signal={}", signal);
      Stop();
    }
  });

  boost::asio::co_spawn(context_, Start(), [this](std::exception_ptr failure) {
    if (failure && !stopping_) {
      exit_code_ = 1;
      try {
        std::rethrow_exception(failure);
      } catch (const boost::system::system_error& error) {
        spdlog::error(
            "[Application] startup failed: {} (code={}); check database "
            "authentication, TLS, schema and listener",
            error.code().message(), error.code().value());
      } catch (const std::exception& error) {
        spdlog::error("[Application] startup failed: {}", error.what());
      }
      Stop();
    }
  });

  try {
    context_.run();
  } catch (const std::exception&) {
    spdlog::error("[Application] event loop failed");
    exit_code_ = 1;
    Stop();
    context_.run();
  }

  spdlog::info("[Application] stopped exitCode={}", exit_code_);
  spdlog::default_logger()->flush();
  return exit_code_;
}

boost::asio::awaitable<void> ServerApplication::Start() {
  spdlog::info("[Database] checking connection and schema version");
  database_.Start();
  co_await database_.CheckConnection();
  co_await database_.CheckSchema();
  if (stopping_) {
    co_return;
  }

  listener_.Start();
  if (relay_enabled_) {
    relay_listener_.Start();
  }
  authentication_listener_.Start();
  spdlog::info("[Application] ready port={} protocol=cn0261-ticket-login",
               listener_.GetPort());
}

boost::asio::awaitable<std::vector<game::SettledPlayer>>
ServerApplication::Settle(game::CompletedGame game) {
  auto settled = co_await settlements_.Settle(std::move(game));
  // Refresh through each session's packet queue, after any outstanding older
  // status query. This also handles a participant leaving after the result was
  // frozen but before COMMIT.
  if (!stopping_) {
    const auto clients = clients_;
    for (const auto& [sessionId, client] : clients) {
      try {
        for (const auto& player : settled) {
          client->RefreshCharacter(player.character_id);
        }
      } catch (const std::exception&) {
        listener_.Disconnect(sessionId);
      }
    }
  }
  co_return settled;
}

void ServerApplication::ConnectClient(std::uint64_t session_id) {
  auto client = std::make_shared<game::GameSession>(
      context_, accounts_, characters_, inventory_, shop_, channels_, chats_,
      rooms_, rounds_, plazas_, session_id,
      listener_.GetPeerEndpoint(session_id).address().to_string(),
      listener_.GetServerEndpoint(session_id),
      [this, session_id](std::vector<std::uint8_t> bytes) {
        if (!listener_.Send(session_id, std::move(bytes))) {
          throw std::runtime_error("Client send queue is unavailable");
        }
      },
      [this, session_id]() { listener_.Disconnect(session_id); },
      [this, session_id]() { listener_.CloseAfterSend(session_id); },
      [this](std::uint64_t source_session_id) {
        const auto source = clients_.find(source_session_id);
        return source != clients_.end() && source->second->FinishTransfer();
      },
      [this](std::uint64_t destination_session_id,
             const protocol::LambdaPacket& packet) {
        Notify(destination_session_id, packet);
      },
      [this](std::uint64_t destination_session_id) {
        const auto destination = clients_.find(destination_session_id);
        if (!stopping_ && destination != clients_.end()) {
          const auto target = destination->second;
          target->ReturnToLobby();
        }
      });
  const auto [entry, inserted] = clients_.emplace(session_id, client);
  if (!inserted) {
    throw std::logic_error("Duplicate client session identifier");
  }

  entry->second->Start();
}

void ServerApplication::Notify(std::uint64_t session_id,
                               const protocol::LambdaPacket& packet) {
  const auto destination = clients_.find(session_id);
  if (stopping_ || destination == clients_.end()) {
    return;
  }
  const auto client = destination->second;
  try {
    client->SendNotification(packet);
  } catch (const std::exception&) {
    // Defer cleanup to disconnection so a notification cannot invalidate its
    // caller's room operation.
    listener_.Disconnect(session_id);
  }
}

void ServerApplication::ReceiveBytes(std::uint64_t session_id,
                                     std::span<const std::uint8_t> bytes) {
  try {
    const auto client = clients_.at(session_id);
    client->Receive(bytes);
  } catch (const std::exception& error) {
    spdlog::warn("[Protocol] session={} rejected: {}", session_id,
                 error.what());
    listener_.Disconnect(session_id);
  }
}

void ServerApplication::Stop() {
  if (stopping_) {
    return;
  }

  stopping_ = true;
  signals_.cancel();
  authentication_listener_.Stop();
  rounds_.Stop();
  relay_listener_.Stop();
  listener_.Stop();
  database_.Stop();
}
}  // namespace bubblefighter::application
