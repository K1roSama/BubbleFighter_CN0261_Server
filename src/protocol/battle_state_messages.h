#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_BATTLE_STATE_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_BATTLE_STATE_MESSAGES_H_

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
struct DirectedBattleState {
  std::uint32_t target_slot = 0;
  LambdaPacket reply;
};

// C2015 targets a recipient; S976 identifies the authenticated source instead.
DirectedBattleState ReadUserState(const LambdaPacket& packet,
                                  std::uint8_t source_slot);
DirectedBattleState ReadMapItemState(const LambdaPacket& packet,
                                     std::uint8_t source_slot);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_BATTLE_STATE_MESSAGES_H_
