#ifndef BUBBLEFIGHTER_SERVER_RELAY_RELAY_LISTENER_H_
#define BUBBLEFIGHTER_SERVER_RELAY_RELAY_LISTENER_H_

#include <boost/asio/steady_timer.hpp>
#include <memory>
#include <unordered_map>

#include "network/tcp_listener.h"
#include "relay/relay_directory.h"

namespace bubblefighter::relay {
// Registration and liveness transport. Game forwarding requires a separate
// verified message policy.
class RelayListener {
 public:
  RelayListener(boost::asio::io_context& context,
                configuration::NetworkConfig config, RelayDirectory& directory);
  void Start();
  void Stop();
  void Revoke(std::uint64_t owner_session_id);
  void SetHandlers(
      std::function<void(const RelayMember&, const protocol::LambdaPacket&)>
          packet,
      std::function<void(std::uint64_t)> disconnected);
  bool SendToOwner(std::uint64_t owner_session_id,
                   const protocol::LambdaPacket& packet);
  std::uint16_t GetPort() const;

 private:
  struct Connection {
    explicit Connection(boost::asio::io_context& context)
        : registration_deadline(context) {}

    protocol::LambdaPacketCodec codec{protocol::PacketEndpoint::kServer};
    boost::asio::steady_timer registration_deadline;
    bool registered = false;
  };

  void Connect(std::uint64_t connection_id);
  void Receive(std::uint64_t connection_id,
               std::span<const std::uint8_t> bytes);
  void Disconnect(std::uint64_t connection_id);
  void Send(std::uint64_t connection_id, Connection& connection,
            const protocol::LambdaPacket& packet);

  boost::asio::io_context& context_;
  RelayDirectory& directory_;
  network::TcpListener listener_;
  std::unordered_map<std::uint64_t, std::shared_ptr<Connection>> connections_;
  bool stopped_ = true;
  std::function<void(const RelayMember&, const protocol::LambdaPacket&)>
      packet_handler_;
  std::function<void(std::uint64_t)> disconnected_handler_;
};
}  // namespace bubblefighter::relay

#endif  // BUBBLEFIGHTER_SERVER_RELAY_RELAY_LISTENER_H_
