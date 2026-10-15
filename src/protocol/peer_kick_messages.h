#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_KICK_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_KICK_MESSAGES_H_

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
enum class PeerKickKind {
  kStart,
  kTryKick,
  kConfirmed,
  kProtected,
  kGotKicked,
  kEnd,
  kFailed,
  kHumiliationStart,
  kHumiliationEnd
};

struct PeerKickEvent {
  PeerKickKind kind = PeerKickKind::kTryKick;
  std::uint32_t victim_object = 0;
  std::uint32_t actor_object = 0;
  std::uint32_t style = 0;
  bool succeeded = true;
  bool special_kick = false;
};

bool IsPeerKick(std::uint16_t message_id);
PeerKickEvent ReadPeerKick(const LambdaPacket& packet);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_PEER_KICK_MESSAGES_H_
