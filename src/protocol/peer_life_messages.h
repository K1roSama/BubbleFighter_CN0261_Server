#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_LIFE_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_LIFE_MESSAGES_H_

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
enum class PeerLifeKind {
  kKnocked,
  kTryRescue,
  kPopOut,
  kRescue,
  kDying,
  kDead,
  kBubbleSettled,
  kSelfPopOut,
  kStunned,
  kEndStunned
};

struct PeerLifeEvent {
  PeerLifeKind kind = PeerLifeKind::kKnocked;
  std::uint32_t victim_object = 0;
  std::uint32_t actor_object = 0;
  std::uint32_t weapon_definition = 0;
  std::uint32_t tick = 0;
  bool direct_death = false;
  bool special_rescue = false;
};

enum class PeerLifePointEffect : std::uint32_t {
  kIncreasedLoss = 1,
  kReducedLoss = 2
};

struct PeerLifePoint {
  std::uint32_t character_object = 0;
  std::uint32_t remaining_points = 0;
  PeerLifePointEffect effect = PeerLifePointEffect::kIncreasedLoss;
};

bool IsPeerLife(std::uint16_t message_id);
PeerLifeEvent ReadPeerLife(const LambdaPacket& packet);
PeerLifePoint ReadPeerLifePoint(const LambdaPacket& packet,
                                std::uint8_t source_slot);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_LIFE_MESSAGES_H_
