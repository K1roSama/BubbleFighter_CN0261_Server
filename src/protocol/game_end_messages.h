#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_GAME_END_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_GAME_END_MESSAGES_H_

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
LambdaPacket CreateGameEndReply(bool time_expired);
LambdaPacket CreateGameWinnerReply(std::uint16_t first_team_score,
                                   std::uint16_t second_team_score);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_GAME_END_MESSAGES_H_
