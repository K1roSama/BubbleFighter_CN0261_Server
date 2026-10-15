#include "protocol/damage_attribute_messages.h"

#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
bool IsDamageAttributeSwitch(std::uint8_t kind) {
  return kind == 0 || kind == 10 || kind == 11;
}

DamageAttributeReport ReadDamageAttributeReport(const LambdaPacket& packet,
                                                std::uint8_t source_slot) {
  if (packet.message_id != 11 || source_slot >= 8) {
    throw std::invalid_argument(
        "Expected a damage attribute report from a supported player slot");
  }

  // 0x5374C0 writes 4/1/2/4 bytes; 0x545550 only initializes the clock's low 24
  // bits.
  PacketReader reader(packet.payload);
  DamageAttributeReport report;
  report.character_object = reader.ReadUnsigned();
  report.kind = reader.ReadByte();
  report.value = reader.ReadShort();
  report.tick = reader.ReadUnsigned() & 0xFFFFFFu;
  reader.RequireEnd();
  if (report.character_object < 1000000 ||
      (report.character_object - 1000000) / 100000 != source_slot ||
      report.kind >= 12 ||
      (IsDamageAttributeSwitch(report.kind) && report.value > 1)) {
    throw std::invalid_argument(
        "Damage attribute owner, kind or switch value is invalid");
  }

  return report;
}
}  // namespace bubblefighter::protocol
