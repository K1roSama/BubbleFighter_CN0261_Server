#include "protocol/health_report_messages.h"

#include <bit>
#include <cmath>
#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
HealthResetReport ReadHealthResetReport(const LambdaPacket& packet,
                                        std::uint8_t source_slot) {
  if (packet.message_id != 14 || source_slot >= 8) {
    throw std::invalid_argument(
        "Expected a health reset report from a supported player slot");
  }

  // 0x551F00 / 0x538BA0: u32 actor and u32 clock, with only the low 24 clock
  // bits initialized.
  PacketReader reader(packet.payload);
  HealthResetReport report;
  report.character_object = reader.ReadUnsigned();
  report.tick = reader.ReadUnsigned() & 0xFFFFFFu;
  reader.RequireEnd();
  if (report.character_object < 1000000 ||
      (report.character_object - 1000000) / 100000 != source_slot) {
    throw std::invalid_argument(
        "Health reset actor does not belong to the reporting slot");
  }

  return report;
}

HealthRecoveryReport ReadHealthRecoveryReport(const LambdaPacket& packet,
                                              std::uint8_t source_slot) {
  if (packet.message_id != 16 || source_slot >= 8) {
    throw std::invalid_argument(
        "Expected a health recovery report from a supported player slot");
  }

  // 0x537650: u32 actor, u8 reason, u32 clock. 0x551E20 sets only the clock's
  // low 24 bits.
  PacketReader reader(packet.payload);
  HealthRecoveryReport report;
  report.character_object = reader.ReadUnsigned();
  report.kind = reader.ReadByte();
  report.tick = reader.ReadUnsigned() & 0xFFFFFFu;
  reader.RequireEnd();
  if (report.character_object < 1000000 ||
      (report.character_object - 1000000) / 100000 != source_slot ||
      report.kind > 13) {
    throw std::invalid_argument(
        "Health recovery actor or reason is outside the verified range");
  }

  return report;
}

std::vector<HealthTraceEntry> ReadHealthTraceReport(
    const LambdaPacket& packet) {
  if (packet.message_id != 18 ||
      packet.payload.size() > LambdaPacketCodec::kMaxPayloadSize) {
    throw std::invalid_argument("Expected a bounded health trace report");
  }

  // 0x8CB160 and 0x566490 serialize u32 count followed by packed 1/4/4/4-byte
  // records.
  PacketReader reader(packet.payload);
  const auto count = reader.ReadUnsigned();
  if (reader.Remaining() % 13 != 0 || count != reader.Remaining() / 13) {
    throw std::invalid_argument(
        "Health trace count does not match its complete records");
  }

  std::vector<HealthTraceEntry> result;
  result.reserve(count);
  for (std::uint32_t index = 0; index < count; ++index) {
    HealthTraceEntry entry;
    entry.kind = reader.ReadByte();
    entry.source = reader.ReadUnsigned();
    entry.amount = std::bit_cast<float>(reader.ReadUnsigned());
    entry.resulting_health = std::bit_cast<float>(reader.ReadUnsigned());
    if (!std::isfinite(entry.amount) ||
        !std::isfinite(entry.resulting_health)) {
      throw std::invalid_argument("Health trace floats must be finite");
    }

    // Negative post-impact HP is legitimate before the following
    // death-normalization record.
    result.push_back(entry);
  }
  reader.RequireEnd();
  return result;
}

LambdaPacket CreateHealthTraceRequest() {
  // 0x896050, server case 8, calls 0x53B6F0 without reading any request fields.
  return {8, {}};
}
}  // namespace bubblefighter::protocol
