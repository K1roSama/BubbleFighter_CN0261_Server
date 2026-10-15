#include "protocol/damage_calculation_messages.h"

#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
DamageCalculationReport ReadDamageCalculationReport(const LambdaPacket& packet,
                                                    std::uint8_t target_slot) {
  if (packet.message_id != 13 || target_slot >= 8) {
    throw std::invalid_argument(
        "Expected a damage calculation report from a supported victim slot");
  }

  // 0x537540 writes 4/4/2/4/2/2 bytes; 0x551D20 initializes only the clock's
  // low 24 bits.
  PacketReader reader(packet.payload);
  DamageCalculationReport report;
  report.target_object = reader.ReadUnsigned();
  report.projectile_id = reader.ReadUnsigned();
  report.target_flags = reader.ReadShort();
  report.tick = reader.ReadUnsigned() & 0xFFFFFFu;
  report.defense_index = reader.ReadShort();
  report.cancel_index = reader.ReadShort();
  reader.RequireEnd();
  if (report.target_object < 1000000 ||
      (report.target_object - 1000000) / 100000 != target_slot ||
      report.projectile_id >= 800000 || report.defense_index >= 1000 ||
      report.cancel_index >= 1000) {
    throw std::invalid_argument(
        "Damage calculation victim, projectile or seed index is invalid");
  }

  return report;
}
}  // namespace bubblefighter::protocol
