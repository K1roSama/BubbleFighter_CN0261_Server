#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_FIRE_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_FIRE_MESSAGES_H_

#include <optional>

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
bool IsPeerFire(std::uint16_t message_id);

struct PeerFire {
  std::uint32_t source_object = 0;
  std::uint32_t weapon_definition = 0;
  std::uint32_t first_projectile_id = 0;
  std::uint8_t weapon_slot = 0;
  std::uint8_t attack_flags = 0;
  // A homing target is not a transport destination.
  std::optional<std::uint32_t> target_object;
  std::optional<std::uint8_t> target_slot;
};

PeerFire ReadPeerFire(const LambdaPacket& packet, std::uint8_t source_slot,
                      std::span<const std::uint32_t> weapon_definitions);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_FIRE_MESSAGES_H_
