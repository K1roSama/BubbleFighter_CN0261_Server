#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_LOGIN_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_LOGIN_MESSAGES_H_

#include <cstdint>
#include <string>
#include <string_view>

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
std::string ReadLoginTicket(const LambdaPacket& packet);
LambdaPacket CreateLoginReply(std::uint32_t result, std::uint32_t account_id,
                              std::u16string_view account_name,
                              std::uint32_t first_challenge,
                              std::uint32_t second_challenge,
                              std::u16string_view nickname = u"");
void ValidateInitialInfo(const LambdaPacket& packet,
                         std::uint32_t first_challenge,
                         std::uint32_t second_challenge);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_LOGIN_MESSAGES_H_
