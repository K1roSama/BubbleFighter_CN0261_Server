#include "protocol/peer_near_messages.h"

#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
PeerNearAttack ReadPeerNearAttack(const LambdaPacket& packet,
                                  std::uint8_t source_slot,
                                  std::uint32_t equipped_definition) {
  if (packet.message_id != 32773) {
    throw std::invalid_argument(
        "Expected ordinary near weapon damage object creation");
  }
  PacketReader reader(packet.payload);
  PeerNearAttack attack;
  attack.source_object = reader.ReadUnsigned();
  attack.weapon_definition = reader.ReadShort();
  // Original Character +408 attribute; retain the byte without treating it as
  // trusted team identity.
  static_cast<void>(reader.ReadByte());
  attack.damager_id = reader.ReadUnsigned();
  // The sender initializes only the low 24 bits. Forward the original four
  // bytes unchanged.
  static_cast<void>(reader.ReadUnsigned());
  reader.RequireEnd();
  if (source_slot >= 8 || attack.source_object < 1000000 ||
      (attack.source_object - 1000000) / 100000 != source_slot ||
      attack.damager_id / 100000 != source_slot) {
    throw std::invalid_argument(
        "Near weapon objects do not belong to the authenticated room slot");
  }
  if (attack.weapon_definition == 0 ||
      attack.weapon_definition != equipped_definition) {
    throw std::invalid_argument(
        "Near weapon is not in the frozen equipped slot");
  }
  return attack;
}
}  // namespace bubblefighter::protocol
