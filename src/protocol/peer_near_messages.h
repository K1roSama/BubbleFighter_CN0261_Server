#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_NEAR_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_NEAR_MESSAGES_H_

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
struct PeerNearAttack {
  std::uint32_t source_object = 0;
  std::uint32_t weapon_definition = 0;
  std::uint32_t damager_id = 0;
};

PeerNearAttack ReadPeerNearAttack(const LambdaPacket& packet,
                                  std::uint8_t source_slot,
                                  std::uint32_t equipped_definition);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_NEAR_MESSAGES_H_
