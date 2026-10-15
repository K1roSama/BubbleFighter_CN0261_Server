#include "protocol/login_messages.h"

#include <bit>
#include <chrono>
#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
std::string ReadLoginTicket(const LambdaPacket& packet) {
  if (packet.message_id != 3) {
    throw std::invalid_argument("Expected CN0261 ReqLogin");
  }
  // VA 0x6DBDE0: three UTF-16 strings, a vector of uint32, then two uint32
  // scalars.
  PacketReader reader(packet.payload);
  const auto identity = reader.ReadString(64);
  static_cast<void>(reader.ReadString(4096));
  static_cast<void>(reader.ReadString(4096));
  const auto count = reader.ReadUnsigned();
  if (count > 4096 || count > reader.Remaining() / 4) {
    throw std::invalid_argument("ReqLogin vector is out of bounds");
  }
  for (std::uint32_t index = 0; index < count; ++index) {
    static_cast<void>(reader.ReadUnsigned());
  }
  static_cast<void>(reader.ReadUnsigned());
  static_cast<void>(reader.ReadUnsigned());
  reader.RequireEnd();

  std::string token;
  for (const auto code_unit : identity) {
    if (code_unit > 127) {
      throw std::invalid_argument("ReqLogin identity encoding is invalid");
    }
    token.push_back(static_cast<char>(code_unit));
  }
  return token;
}

LambdaPacket CreateLoginReply(std::uint32_t result, std::uint32_t account_id,
                              std::u16string_view account_name,
                              std::uint32_t first_challenge,
                              std::uint32_t second_challenge,
                              std::u16string_view nickname) {
  // VA 0x8CF030 and 0x89E7F0. Challenges are echoed verbatim by C102 before
  // initialization.
  PacketWriter writer;
  writer.WriteUnsigned(result);
  writer.WriteString(account_name);
  writer.WriteUnsigned(account_id);
  writer.WriteString(nickname);
  writer.WriteUnsigned(first_challenge);
  writer.WriteUnsigned(second_challenge);
  writer.WriteUnsigned(0);
  writer.WriteUnsigned(0);
  const auto seconds = std::chrono::duration<double>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count();
  const auto date = std::bit_cast<std::uint64_t>(25569.0 + seconds / 86400.0);
  writer.WriteUnsigned(static_cast<std::uint32_t>(date));
  writer.WriteUnsigned(static_cast<std::uint32_t>(date >> 32));
  writer.WriteString(u"");
  writer.WriteUnsigned(0);
  return {3, writer.GetBytes()};
}

void ValidateInitialInfo(const LambdaPacket& packet,
                         std::uint32_t first_challenge,
                         std::uint32_t second_challenge) {
  if (packet.message_id != 102) {
    throw std::invalid_argument("Expected CN0261 InitialInfo");
  }
  PacketReader reader(packet.payload);
  if (reader.ReadUnsigned() != first_challenge ||
      reader.ReadUnsigned() != second_challenge) {
    throw std::invalid_argument(
        "InitialInfo login challenge does not match this connection");
  }
  const auto count = reader.ReadUnsigned();
  if (count != 5) {
    throw std::invalid_argument("InitialInfo requires five client settings");
  }
  for (std::uint32_t index = 0; index < count; ++index) {
    static_cast<void>(reader.ReadUnsigned());
  }
  reader.RequireEnd();
}
}  // namespace bubblefighter::protocol
