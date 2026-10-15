#ifndef BUBBLEFIGHTER_SERVER_GAME_CLIENT_CONNECTION_H_
#define BUBBLEFIGHTER_SERVER_GAME_CLIENT_CONNECTION_H_

#include <cstdint>
#include <functional>

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::game {
class ClientConnection {
 public:
  using SendHandler = std::function<void(std::vector<std::uint8_t>)>;
  using PacketHandler = std::function<void(const protocol::LambdaPacket&)>;

  ClientConnection(SendHandler send, PacketHandler packet);

  void Start();
  void Receive(std::span<const std::uint8_t> bytes);
  void Send(const protocol::LambdaPacket& packet);
  bool HasCompletedHandshake() const;

 private:
  void HandleFirstConnect(const protocol::LambdaPacket& packet);

  protocol::LambdaPacketCodec codec_{protocol::PacketEndpoint::kServer};
  SendHandler send_;
  PacketHandler packet_;
  bool started_ = false;
  bool initial_request_received_ = false;
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_CLIENT_CONNECTION_H_
