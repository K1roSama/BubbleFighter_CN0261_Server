#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_RELAY_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_RELAY_MESSAGES_H_

#include <array>

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
// The original serializes three u32, two u16 and one u32. Keep the 20 echoed
// bytes opaque.
using RelaySessionKey = std::array<std::uint8_t, 20>;

struct RelayRegistration {
  RelaySessionKey key{};
  std::uint8_t slot = 0;
  std::uint32_t peer_id = 0;
};

RelayRegistration ReadRelayRegistration(const LambdaPacket& packet);
LambdaPacket CreateRelayEndpointReply(
    const std::array<std::uint8_t, 4>& address, std::uint16_t port,
    const RelaySessionKey& key);
LambdaPacket CreateRelayRegistrationReply();
LambdaPacket CreateRelayLagReply(const LambdaPacket& packet);
void ValidateRelayPingReply(const LambdaPacket& packet);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_RELAY_MESSAGES_H_
