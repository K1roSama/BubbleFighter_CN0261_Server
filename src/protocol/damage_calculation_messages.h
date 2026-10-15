#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_DAMAGE_CALCULATION_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_DAMAGE_CALCULATION_MESSAGES_H_

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
struct DamageCalculationReport {
  std::uint32_t target_object = 0;
  std::uint32_t projectile_id = 0;
  std::uint16_t target_flags = 0;
  std::uint32_t tick = 0;
  std::uint16_t defense_index = 0;
  std::uint16_t cancel_index = 0;
};

DamageCalculationReport ReadDamageCalculationReport(const LambdaPacket& packet,
                                                    std::uint8_t target_slot);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_DAMAGE_CALCULATION_MESSAGES_H_
