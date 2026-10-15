#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_CHARACTER_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_CHARACTER_MESSAGES_H_

#include "features/characters/character_repository.h"
#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
struct CreateCharacterRequest {
  std::u16string nickname;
  std::uint32_t character_type;
  std::uint32_t starter_selection;
};

std::u16string ReadNicknameRequest(const LambdaPacket& packet);
CreateCharacterRequest ReadCreateCharacterRequest(const LambdaPacket& packet);
LambdaPacket CreateNicknameReply(characters::CreationResult result,
                                 std::u16string_view nickname);
LambdaPacket CreateCharacterReply(characters::CreationResult result);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_CHARACTER_MESSAGES_H_
