#include "protocol/chat_messages.h"

#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
void ValidateChatText(std::u16string_view text) {
  if (text.empty() || text.size() > 256) {
    throw std::invalid_argument(
        "Chat must contain between 1 and 256 UTF-16 code units");
  }
  bool visible = false;
  for (std::size_t index = 0; index < text.size(); ++index) {
    const auto unit = text[index];
    if (unit < 0x20 || (unit >= 0x7F && unit <= 0x9F) ||
        (unit >= 0xDC00 && unit <= 0xDFFF) || unit == 0x2028 ||
        unit == 0x2029) {
      throw std::invalid_argument(
          "Chat contains a control character or invalid UTF-16");
    }
    if (unit >= 0xD800 && unit <= 0xDBFF) {
      if (++index >= text.size() || text[index] < 0xDC00 ||
          text[index] > 0xDFFF) {
        throw std::invalid_argument("Chat contains an incomplete UTF-16 pair");
      }
    }
    visible = visible || (unit != u' ' && unit != 0x3000);
  }
  if (!visible) {
    throw std::invalid_argument("Chat must contain visible text");
  }
}

ModuleChat ReadModuleChat(const LambdaPacket& packet) {
  if (packet.message_id != 502) {
    throw std::invalid_argument("Expected RoomSendChatModuleMsg");
  }
  PacketReader reader(packet.payload);
  ModuleChat chat{reader.ReadUnsigned(), reader.ReadString(256)};
  reader.RequireEnd();
  ValidateChatText(chat.text);
  return chat;
}

std::u16string ReadPartyChat(const LambdaPacket& packet) {
  if (packet.message_id != 304) {
    throw std::invalid_argument("Expected SendPartyChatMsg");
  }
  PacketReader reader(packet.payload);
  auto text = reader.ReadString(256);
  reader.RequireEnd();
  ValidateChatText(text);
  return text;
}

LambdaPacket CreateModuleChatReply(std::uint16_t slot,
                                   std::u16string_view nickname,
                                   std::uint32_t channel,
                                   std::u16string_view text) {
  // 0x88F310 / 0x88F380: u16, string, u32, string, string, u32, u32, boolean32.
  if ((slot >= 32 && slot != 0xFFFD) || nickname.empty() ||
      nickname.size() > 16) {
    throw std::invalid_argument(
        "Chat requires a server identity or the original named-message sentinel");
  }
  ValidateChatText(text);
  PacketWriter writer;
  writer.WriteShort(slot);
  writer.WriteString(nickname);
  writer.WriteUnsigned(channel);
  writer.WriteString(u"");  // No unverified emote prefix.
  writer.WriteString(text);
  writer.WriteUnsigned(0xFFFFFFFFu);
  writer.WriteUnsigned(0);
  writer.WriteUnsigned(0);  // Ordinary player, never an administrative flag
                            // supplied by the sender.
  return {509, writer.GetBytes()};
}

LambdaPacket CreateChatNotice(std::u16string_view text) {
  // 0x885A10's -3 branch displays an explicit name without dereferencing a room
  // slot.
  return CreateModuleChatReply(
      0xFFFD, u"系统", static_cast<std::uint32_t>(ChatChannel::kSystem), text);
}

LambdaPacket CreatePartyChatReply(std::uint32_t account_id,
                                  std::u16string_view text) {
  if (account_id == 0) {
    throw std::invalid_argument("Party chat requires an authenticated account");
  }
  ValidateChatText(text);
  PacketWriter writer;
  writer.WriteUnsigned(account_id);
  writer.WriteString(u"");
  writer.WriteString(text);
  writer.WriteUnsigned(0xFFFFFFFFu);
  writer.WriteUnsigned(0);  // 0x9CCED0 uses a four-byte boolean, not a byte.
  return {305, writer.GetBytes()};
}
}  // namespace bubblefighter::protocol
