#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_DAMAGE_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_DAMAGE_MESSAGES_H_

#include <optional>

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
struct PeerHit {
  std::uint32_t projectile_id = 0;
  std::uint32_t target_object = 0;
  std::uint8_t target_slot = 0;
};

struct PeerExplosion {
  std::uint32_t projectile_id = 0;
  std::optional<std::uint32_t> target_object;
  std::optional<std::uint8_t> target_slot;
  bool apply_damage = false;
};

struct PeerDetonation {
  std::uint32_t projectile_id = 0;
  std::uint32_t splash_id = 0;
};

bool IsPeerExplosion(std::uint16_t message_id);
PeerDetonation ReadPeerDetonation(const LambdaPacket& packet);
PeerExplosion ReadPeerExplosion(const LambdaPacket& packet);
PeerHit ReadPeerHit(const LambdaPacket& packet);
std::uint32_t ReadPeerProjectileRemoval(const LambdaPacket& packet);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_DAMAGE_MESSAGES_H_
