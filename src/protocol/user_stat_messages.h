#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_USER_STAT_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_USER_STAT_MESSAGES_H_

#include "features/characters/character_repository.h"
#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
LambdaPacket CreateUserStatReply(const characters::CharacterProfile& character,
                                 const characters::CharacterStats& stats);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_USER_STAT_MESSAGES_H_
