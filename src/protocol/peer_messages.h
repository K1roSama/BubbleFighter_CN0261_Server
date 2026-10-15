#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_MESSAGES_H_

#include <optional>
#include <string_view>

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
struct PeerMovement {
  std::uint32_t source_object = 0;
  std::optional<std::uint32_t> target_object;
  // WatchingOnMe addresses a transport peer; ordinary movement has no
  // destination.
  std::optional<std::uint32_t> destination;
};

PeerMovement ReadPeerMovement(const LambdaPacket& packet,
                              std::uint8_t source_slot);
LambdaPacket CreatePeerPingReply(const LambdaPacket& packet,
                                 std::uint8_t source_slot);
bool IsPeerAction(std::uint16_t message_id);
std::uint32_t ReadPeerActionObject(
    const LambdaPacket& packet, std::uint8_t source_slot,
    std::span<const std::uint32_t> weapon_definitions);
std::uint32_t ReadRelayPeerRequest(const LambdaPacket& packet);
std::uint32_t ReadRelayUseReport(const LambdaPacket& packet,
                                 std::uint8_t source_slot,
                                 std::u16string_view resource);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_MESSAGES_H_
