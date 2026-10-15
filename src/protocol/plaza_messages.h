#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_PLAZA_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_PLAZA_MESSAGES_H_

#include <array>
#include <string>

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
struct PlazaPosition {
  std::uint32_t sequence = 0;
  std::array<float, 3> coordinates{};
  std::array<std::uint8_t, 2> input_flags{};
  float heading = 0.0f;

  bool operator==(const PlazaPosition&) const = default;
};

struct SessionChat {
  bool team_only = false;
  std::u16string text;
};

PlazaPosition ReadPlazaPosition(const LambdaPacket& packet);
void ValidateSessionInitialization(const LambdaPacket& packet);
LambdaPacket CreatePlazaPositionReply(std::uint32_t account_id,
                                      const PlazaPosition& position);
LambdaPacket CreateSessionInitializationReply(std::uint32_t account_id);
SessionChat ReadSessionChat(const LambdaPacket& packet);
LambdaPacket CreateSessionChatReply(std::uint16_t slot,
                                    const SessionChat& chat);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_PLAZA_MESSAGES_H_
