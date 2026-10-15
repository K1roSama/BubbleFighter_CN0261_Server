#include "protocol/session_ping_messages.h"

#include <bit>
#include <cmath>
#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
namespace {
void ValidateSlot(std::uint32_t slot) {
  if (slot >= 32) {
    throw std::invalid_argument(
        "Session ping slot is outside the supported range");
  }
}

void ValidateResponse(const SessionPingResponse& response) {
  ValidateSlot(response.responder_slot);
  ValidateSlot(response.requester_slot);
  if (!std::isfinite(response.stage_sample) || response.stage_sample < 0.0f ||
      response.stage_sample > 1000000.0f) {
    throw std::invalid_argument("Session ping stage sample is invalid");
  }
}
}  // namespace

std::uint32_t ReadSessionPingRequest(const LambdaPacket& packet) {
  if (packet.message_id != 517) {
    throw std::invalid_argument("Expected ReqPingUser");
  }
  PacketReader reader(packet.payload);
  const auto slot = reader.ReadUnsigned();
  reader.RequireEnd();
  ValidateSlot(slot);
  return slot;
}

SessionPingResponse ReadSessionPingResponse(const LambdaPacket& packet) {
  if (packet.message_id != 518) {
    throw std::invalid_argument("Expected RepPingUser");
  }
  PacketReader reader(packet.payload);
  SessionPingResponse response{reader.ReadUnsigned(), reader.ReadUnsigned(),
                               reader.ReadUnsigned(),
                               std::bit_cast<float>(reader.ReadUnsigned())};
  reader.RequireEnd();
  ValidateResponse(response);
  return response;
}

LambdaPacket CreateSessionPingRequest(std::uint32_t requester_slot) {
  ValidateSlot(requester_slot);
  PacketWriter writer;
  writer.WriteUnsigned(requester_slot);
  return {517, writer.GetBytes()};
}

LambdaPacket CreateSessionPingReply(const SessionPingResponse& response) {
  ValidateResponse(response);
  PacketWriter writer;
  writer.WriteUnsigned(response.responder_slot);
  writer.WriteUnsigned(response.performance_sample);
  writer.WriteUnsigned(std::bit_cast<std::uint32_t>(response.stage_sample));
  return {518, writer.GetBytes()};
}
}  // namespace bubblefighter::protocol
