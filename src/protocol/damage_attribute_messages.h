#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_DAMAGE_ATTRIBUTE_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_DAMAGE_ATTRIBUTE_MESSAGES_H_

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
struct DamageAttributeReport {
  std::uint32_t character_object = 0;
  std::uint8_t kind = 0;
  std::uint16_t value = 0;
  std::uint32_t tick = 0;
};

bool IsDamageAttributeSwitch(std::uint8_t kind);
DamageAttributeReport ReadDamageAttributeReport(const LambdaPacket& packet,
                                                std::uint8_t source_slot);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_DAMAGE_ATTRIBUTE_MESSAGES_H_
