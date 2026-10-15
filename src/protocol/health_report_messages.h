#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_HEALTH_REPORT_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_HEALTH_REPORT_MESSAGES_H_

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
struct HealthResetReport {
  std::uint32_t character_object = 0;
  std::uint32_t tick = 0;
};

struct HealthRecoveryReport {
  std::uint32_t character_object = 0;
  std::uint8_t kind = 0;
  std::uint32_t tick = 0;
};

struct HealthTraceEntry {
  std::uint8_t kind = 0;
  // A damage object for kind 0; a recovery reason for kind 1; other kinds
  // retain their raw identifier.
  std::uint32_t source = 0;
  float amount = 0.0f;
  float resulting_health = 0.0f;
};

// Client diagnostics only: these reports never authorize a server HP or
// life-state change.
HealthResetReport ReadHealthResetReport(const LambdaPacket& packet,
                                        std::uint8_t source_slot);
HealthRecoveryReport ReadHealthRecoveryReport(const LambdaPacket& packet,
                                              std::uint8_t source_slot);
std::vector<HealthTraceEntry> ReadHealthTraceReport(const LambdaPacket& packet);
LambdaPacket CreateHealthTraceRequest();
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_HEALTH_REPORT_MESSAGES_H_
