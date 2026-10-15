#include "protocol/character_messages.h"

#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
std::u16string ReadNicknameRequest(const LambdaPacket& packet) {
  if (packet.message_id != 15) {
    throw std::invalid_argument("Expected CN0261 ReqCheckCreateUserName");
  }
  PacketReader reader(packet.payload);
  auto nickname = reader.ReadString(64);
  reader.RequireEnd();
  return nickname;
}

CreateCharacterRequest ReadCreateCharacterRequest(const LambdaPacket& packet) {
  if (packet.message_id != 16) {
    throw std::invalid_argument("Expected CN0261 ReqCreateUserInfo");
  }
  // VA 0x9015E0. Operator SDK metadata does not select account identity or
  // character ownership.
  PacketReader reader(packet.payload);
  CreateCharacterRequest result{reader.ReadString(64), reader.ReadUnsigned(),
                                reader.ReadUnsigned()};
  static_cast<void>(reader.ReadString(4096));
  static_cast<void>(reader.ReadUnsigned());
  reader.RequireEnd();
  return result;
}

LambdaPacket CreateNicknameReply(characters::CreationResult result,
                                 std::u16string_view nickname) {
  PacketWriter writer;
  writer.WriteUnsigned(static_cast<std::uint32_t>(result));
  writer.WriteString(nickname);
  return {13, writer.GetBytes()};
}

LambdaPacket CreateCharacterReply(characters::CreationResult result) {
  PacketWriter writer;
  writer.WriteUnsigned(static_cast<std::uint32_t>(result));
  return {14, writer.GetBytes()};
}
}  // namespace bubblefighter::protocol
