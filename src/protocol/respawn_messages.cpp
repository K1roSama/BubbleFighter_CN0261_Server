#include "protocol/respawn_messages.h"

#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
RespawnRequest ReadRespawnRequest(const LambdaPacket& packet) {
  if (packet.message_id != 713 && packet.message_id != 704 &&
      packet.message_id != 32904 && packet.message_id != 32925) {
    throw std::invalid_argument("Unsupported respawn request");
  }

  PacketReader reader(packet.payload);
  RespawnRequest request{reader.ReadUnsigned(), reader.ReadUnsigned()};
  if (request.object_id < 1000000 ||
      (request.object_id - 1000000) / 100000 >= 8) {
    throw std::invalid_argument(
        "Respawn object is outside the player partitions");
  }
  if ((packet.message_id == 713 || packet.message_id == 32904) &&
      request.value > 60000) {
    throw std::invalid_argument(
        "Requested respawn delay is outside the supported range");
  }
  if (packet.message_id == 704) {
    static_cast<void>(reader.ReadBoolean());
  }

  reader.RequireEnd();
  return request;
}

LambdaPacket CreateRespawnOffer(std::uint32_t object_id,
                                std::uint32_t spawn_index) {
  PacketWriter writer;
  writer.WriteUnsigned(object_id);
  writer.WriteUnsigned(spawn_index);
  writer.WriteUnsigned(1);  // 0x538CC0 / 0x5536E0: four-byte bool; reply with
                            // C704 using this index.
  return {708, writer.GetBytes()};
}

LambdaPacket CreateRespawnReply(std::uint32_t object_id,
                                std::uint32_t spawn_index) {
  PacketWriter writer;
  writer.WriteUnsigned(object_id);
  writer.WriteUnsigned(spawn_index);
  writer.WriteUnsigned(
      1);  // Server policy: clear in-round items on ordinary respawn.
  writer.WriteUnsigned(
      0);  // Mode 19 auxiliary data is not used by normal/item team rooms.
  return {707, writer.GetBytes()};
}
}  // namespace bubblefighter::protocol
