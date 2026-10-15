#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_SCORE_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_SCORE_MESSAGES_H_

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
struct PlayerBattleScore {
  std::uint16_t bubble_count = 0;
  std::uint16_t pang_count = 0;
  std::uint16_t rescue_count = 0;

  bool operator==(const PlayerBattleScore&) const = default;
};

LambdaPacket CreatePlayerScoreReply(std::uint8_t slot,
                                    const PlayerBattleScore& score,
                                    std::uint32_t personal_score);
LambdaPacket CreateTeamScoreReply(std::uint8_t team, std::uint16_t score);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_SCORE_MESSAGES_H_
