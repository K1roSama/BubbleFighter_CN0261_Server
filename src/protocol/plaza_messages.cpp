#include "protocol/plaza_messages.h"

#include <bit>
#include <cmath>
#include <stdexcept>

#include "protocol/chat_messages.h"
#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
namespace {
float ReadCoordinate(PacketReader& reader) {
  const auto value = std::bit_cast<float>(reader.ReadUnsigned());
  if (!std::isfinite(value) || std::abs(value) > 10000000.0f) {
    throw std::invalid_argument(
        "Plaza position contains an invalid floating point value");
  }
  return value;
}

}  // namespace

PlazaPosition ReadPlazaPosition(const LambdaPacket& packet) {
  if (packet.message_id != 511) {
    throw std::invalid_argument("Expected SendSessionChrPos");
  }
  PacketReader reader(packet.payload);
  PlazaPosition position;
  position.sequence = reader.ReadUnsigned();
  for (auto& coordinate : position.coordinates) {
    coordinate = ReadCoordinate(reader);
  }
  for (auto& flag : position.input_flags) {
    flag = reader.ReadByte();
  }
  position.heading = ReadCoordinate(reader);
  reader.RequireEnd();
  return position;
}

void ValidateSessionInitialization(const LambdaPacket& packet) {
  if (packet.message_id != 521 || !packet.payload.empty()) {
    throw std::invalid_argument("Expected empty SendSessionInit");
  }
}

LambdaPacket CreatePlazaPositionReply(std::uint32_t account_id,
                                      const PlazaPosition& position) {
  if (account_id == 0) {
    throw std::invalid_argument(
        "Plaza position requires an authenticated account");
  }
  PacketWriter writer;
  writer.WriteUnsigned(account_id);
  writer.WriteUnsigned(position.sequence);
  for (const auto coordinate : position.coordinates) {
    writer.WriteUnsigned(std::bit_cast<std::uint32_t>(coordinate));
  }
  for (const auto flag : position.input_flags) {
    writer.WriteByte(flag);
  }
  writer.WriteUnsigned(std::bit_cast<std::uint32_t>(position.heading));
  return {514, writer.GetBytes()};
}

LambdaPacket CreateSessionInitializationReply(std::uint32_t account_id) {
  if (account_id == 0) {
    throw std::invalid_argument(
        "Session initialization requires an authenticated account");
  }
  PacketWriter writer;
  writer.WriteUnsigned(account_id);
  return {524, writer.GetBytes()};
}

SessionChat ReadSessionChat(const LambdaPacket& packet) {
  if (packet.message_id != 501) {
    throw std::invalid_argument("Expected RoomSendChatMsg");
  }
  PacketReader reader(packet.payload);
  SessionChat chat{reader.ReadBoolean(), reader.ReadString(256)};
  reader.RequireEnd();
  ValidateChatText(chat.text);
  return chat;
}

LambdaPacket CreateSessionChatReply(std::uint16_t slot,
                                    const SessionChat& chat) {
  if (slot >= 32) {
    throw std::invalid_argument("Session chat requires an occupied user slot");
  }
  ValidateChatText(chat.text);
  PacketWriter writer;
  writer.WriteShort(slot);
  writer.WriteUnsigned(chat.team_only ? 1 : 0);
  writer.WriteString(u"");  // No emote command prefix.
  writer.WriteString(chat.text);
  writer.WriteUnsigned(0xFFFFFFFFu);  // Normal white text; team colors are
                                      // selected by the original handler.
  writer.WriteUnsigned(
      0);  // Ordinary player chat, not an administrative message.
  return {508, writer.GetBytes()};
}
}  // namespace bubblefighter::protocol
