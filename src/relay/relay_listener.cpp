#include "relay/relay_listener.h"

#include <spdlog/spdlog.h>

#include <stdexcept>
#include <utility>

namespace bubblefighter::relay {
RelayListener::RelayListener(boost::asio::io_context& context,
                             configuration::NetworkConfig config,
                             RelayDirectory& directory)
    : context_(context),
      directory_(directory),
      listener_(
          context, std::move(config),
          [this](std::uint64_t connection_id,
                 std::span<const std::uint8_t> bytes) {
            Receive(connection_id, bytes);
          },
          [this](std::uint64_t connection_id) { Connect(connection_id); },
          [this](std::uint64_t connection_id) { Disconnect(connection_id); }) {}

void RelayListener::Start() {
  listener_.Start();
  stopped_ = false;
}

void RelayListener::Stop() {
  stopped_ = true;
  for (const auto& [id, connection] : connections_) {
    static_cast<void>(id);
    connection->registration_deadline.cancel();
  }
  directory_.Clear();
  listener_.Stop();
}

void RelayListener::Revoke(std::uint64_t owner_session_id) {
  if (const auto connection = directory_.Revoke(owner_session_id)) {
    listener_.Disconnect(*connection);
  }
}

void RelayListener::SetHandlers(
    std::function<void(const RelayMember&, const protocol::LambdaPacket&)>
        packet,
    std::function<void(std::uint64_t)> disconnected) {
  packet_handler_ = std::move(packet);
  disconnected_handler_ = std::move(disconnected);
}

bool RelayListener::SendToOwner(std::uint64_t owner_session_id,
                                const protocol::LambdaPacket& packet) {
  const auto id = directory_.FindOwnerConnection(owner_session_id);
  if (stopped_ || !id) {
    return false;
  }
  const auto found = connections_.find(*id);
  if (found == connections_.end()) {
    return false;
  }
  try {
    Send(*id, *found->second, packet);
    return true;
  } catch (const std::exception&) {
    listener_.Disconnect(*id);
    return false;
  }
}

std::uint16_t RelayListener::GetPort() const { return listener_.GetPort(); }

void RelayListener::Connect(std::uint64_t connection_id) {
  auto connection = std::make_shared<Connection>(context_);
  connections_.emplace(connection_id, connection);
  // TcpConnection::BF4CC0 consumes this preamble even though Relay OnGreeting
  // is a no-op.
  if (!listener_.Send(connection_id,
                      protocol::LambdaPacketCodec::CreateGreeting())) {
    throw std::runtime_error("Relay greeting could not be queued");
  }

  connection->registration_deadline.expires_after(std::chrono::seconds(5));
  connection->registration_deadline.async_wait(
      [this, connection_id, connection](boost::system::error_code error) {
        if (!error && !stopped_ && !connection->registered) {
          spdlog::info("[Relay] connection={} registration timeout",
                       connection_id);
          listener_.Disconnect(connection_id);
        }
      });
}

void RelayListener::Receive(std::uint64_t connection_id,
                            std::span<const std::uint8_t> bytes) {
  if (stopped_) {
    return;
  }

  std::uint16_t message_id = 0;
  std::size_t payload_bytes = 0;
  try {
    const auto connection = connections_.at(connection_id);
    for (const auto& packet : connection->codec.Feed(bytes)) {
      message_id = packet.message_id;
      payload_bytes = packet.payload.size();
      if (!connection->registered) {
        const auto registration = protocol::ReadRelayRegistration(packet);
        const auto member = directory_.Bind(
            connection_id, registration,
            listener_.GetPeerEndpoint(connection_id).address().to_string());
        if (!member) {
          throw std::invalid_argument("Relay registration is not authorized");
        }
        connection->registered = true;
        connection->registration_deadline.cancel();
        Send(connection_id, *connection,
             protocol::CreateRelayRegistrationReply());
        spdlog::info(
            "[Relay] connection={} ownerSession={} roomId={} slot={} registered",
            connection_id, member->owner_session_id, member->room_id,
            member->slot);
      } else if (!directory_.FindConnection(connection_id)) {
        throw std::invalid_argument("Relay reservation has been revoked");
      } else if (packet.message_id == 6) {
        protocol::ValidateRelayPingReply(packet);
      } else if (packet.message_id == 20006) {
        Send(connection_id, *connection, protocol::CreateRelayLagReply(packet));
      } else {
        if (!packet_handler_) {
          throw std::invalid_argument(
              "Unsupported Relay packet for registration transport");
        }
        packet_handler_(*directory_.FindConnection(connection_id), packet);
      }
    }
  } catch (const std::exception& error) {
    spdlog::warn(
        "[Relay] connection={} messageId={} payloadBytes={} rejected: {}",
        connection_id, message_id, payload_bytes, error.what());
    listener_.Disconnect(connection_id);
  }
}

void RelayListener::Disconnect(std::uint64_t connection_id) {
  const auto member = directory_.FindConnection(connection_id);
  directory_.Release(connection_id);
  if (const auto found = connections_.find(connection_id);
      found != connections_.end()) {
    found->second->registration_deadline.cancel();
    connections_.erase(found);
  }
  if (!stopped_ && member && disconnected_handler_) {
    disconnected_handler_(member->owner_session_id);
  }
}

void RelayListener::Send(std::uint64_t connection_id, Connection& connection,
                         const protocol::LambdaPacket& packet) {
  if (!listener_.Send(connection_id, connection.codec.Encode(packet))) {
    throw std::runtime_error("Relay send queue is unavailable");
  }
}
}  // namespace bubblefighter::relay
