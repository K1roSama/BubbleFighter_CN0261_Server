#include "protocol/lobby_messages.h"

#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
ChannelRequest ReadChannelRequest(const LambdaPacket& packet) {
  if (packet.message_id != 12 && packet.message_id != 13) {
    throw std::invalid_argument(
        "Expected ReqMoveChannel or ReqMoveChannelByType");
  }

  PacketReader reader(packet.payload);
  const auto type = reader.ReadUnsigned();
  const auto index = packet.message_id == 12 ? reader.ReadUnsigned() : 0;
  reader.RequireEnd();
  return {type, index};
}

MoveIntoRequest ReadMoveIntoRequest(const LambdaPacket& packet) {
  if (packet.message_id != 14) {
    throw std::invalid_argument("Expected ReqMoveInto");
  }

  PacketReader reader(packet.payload);
  const auto identity = reader.ReadUnsigned();
  const auto enter_lobby = reader.ReadUnsigned();
  reader.RequireEnd();
  if (identity == 0 || enter_lobby > 1) {
    throw std::invalid_argument(
        "ReqMoveInto identity or lobby flag is invalid");
  }
  return {identity, enter_lobby != 0};
}

SessionListRequest ReadSessionListRequest(const LambdaPacket& packet) {
  if (packet.message_id != 401) {
    throw std::invalid_argument("Expected ReqListSession");
  }

  PacketReader reader(packet.payload);
  const auto page = reader.ReadShort();
  const auto count = reader.ReadShort();
  reader.RequireEnd();
  if (count == 0 || count > 6) {
    throw std::invalid_argument("ReqListSession count must be between 1 and 6");
  }
  return {page, count};
}

LambdaPacket CreateChannelReply(std::uint32_t result,
                                const std::array<std::uint8_t, 4>& address,
                                std::uint16_t port) {
  PacketWriter writer;
  writer.WriteUnsigned(result);
  // NetAddress (0x537870): IPv4 bytes are already in network order; port uses
  // the normal u16 writer.
  writer.WriteBytes(address);
  writer.WriteShort(port);
  return {9, writer.GetBytes()};
}

LambdaPacket CreateMoveIntoReply(bool success, std::uint32_t channel_type,
                                 std::uint32_t channel_index) {
  PacketWriter writer;
  writer.WriteUnsigned(success ? 1 : 0);
  writer.WriteUnsigned(channel_type);
  writer.WriteUnsigned(channel_index);
  return {11, writer.GetBytes()};
}

LambdaPacket CreateGotoLobbyReply() {
  PacketWriter writer;
  writer.WriteUnsigned(0);  // No additional notice; 0x8A6BE0 changes stage
                            // independently of this flag.
  return {109, writer.GetBytes()};
}

LambdaPacket CreateSessionListReply(
    std::uint32_t page, bool has_more,
    std::span<const std::vector<std::uint8_t>> settings) {
  if (settings.size() > 6) {
    throw std::invalid_argument(
        "Session list exceeds the original six room cards");
  }
  PacketWriter writer;
  writer.WriteUnsigned(page);
  writer.WriteUnsigned(has_more ? 1 : 0);
  writer.WriteUnsigned(static_cast<std::uint32_t>(settings.size()));
  for (const auto& setting : settings) {
    writer.WriteBytes(setting);
  }
  return {101, writer.GetBytes()};
}
}  // namespace bubblefighter::protocol
