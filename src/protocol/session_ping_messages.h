#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_SESSION_PING_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_SESSION_PING_MESSAGES_H_

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
struct SessionPingResponse {
  std::uint32_t responder_slot;
  std::uint32_t requester_slot;
  std::uint32_t performance_sample;
  float stage_sample;
};

std::uint32_t ReadSessionPingRequest(const LambdaPacket& packet);
SessionPingResponse ReadSessionPingResponse(const LambdaPacket& packet);
LambdaPacket CreateSessionPingRequest(std::uint32_t requester_slot);
LambdaPacket CreateSessionPingReply(const SessionPingResponse& response);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_SESSION_PING_MESSAGES_H_
