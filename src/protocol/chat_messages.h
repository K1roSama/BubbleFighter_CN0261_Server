#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_CHAT_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_CHAT_MESSAGES_H_

#include <string>
#include <string_view>

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
enum class ChatChannel : std::uint32_t {
  kNearby = 0,
  kTeam = 1,
  kParty = 2,
  kSystem = 4,
  kCouple = 5,
  kClan = 10
};

struct ModuleChat {
  std::uint32_t channel = 0;
  std::u16string text;
};

void ValidateChatText(std::u16string_view text);
ModuleChat ReadModuleChat(const LambdaPacket& packet);
std::u16string ReadPartyChat(const LambdaPacket& packet);
LambdaPacket CreateModuleChatReply(std::uint16_t slot,
                                   std::u16string_view nickname,
                                   std::uint32_t channel,
                                   std::u16string_view text);
LambdaPacket CreateChatNotice(std::u16string_view text);
LambdaPacket CreatePartyChatReply(std::uint32_t account_id,
                                  std::u16string_view text);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_CHAT_MESSAGES_H_
