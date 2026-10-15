#include "protocol/relay_messages.h"

#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
RelayRegistration ReadRelayRegistration(const LambdaPacket& packet) {
  if (packet.message_id != 1) {
    throw std::invalid_argument("Expected ReqJoinRelaySession");
  }

  PacketReader reader(packet.payload);
  RelayRegistration registration;
  for (auto& byte : registration.key) {
    byte = reader.ReadByte();
  }
  registration.slot = reader.ReadByte();
  registration.peer_id = reader.ReadUnsigned();
  reader.RequireEnd();
  return registration;
}

LambdaPacket CreateRelayEndpointReply(
    const std::array<std::uint8_t, 4>& address, std::uint16_t port,
    const RelaySessionKey& key) {
  if (port == 0) {
    throw std::invalid_argument("Relay endpoint port must not be zero");
  }

  PacketWriter writer;
  writer.WriteBytes(address);
  writer.WriteShort(port);
  writer.WriteBytes(key);
  return {601, writer.GetBytes()};
}

LambdaPacket CreateRelayRegistrationReply() {
  // 0x896050 case 13 causes C601 true. Case 1 starts a different, much larger
  // exchange.
  return {13, {}};
}

LambdaPacket CreateRelayLagReply(const LambdaPacket& packet) {
  if (packet.message_id != 20006) {
    throw std::invalid_argument("Expected ReqLagDisPing");
  }

  PacketReader reader(packet.payload);
  const auto value = reader.ReadUnsigned();
  reader.RequireEnd();
  PacketWriter writer;
  writer.WriteUnsigned(value);
  return {21, writer.GetBytes()};
}

void ValidateRelayPingReply(const LambdaPacket& packet) {
  if (packet.message_id != 6 || !packet.payload.empty()) {
    throw std::invalid_argument("Expected empty Relay RepPing");
  }
}
}  // namespace bubblefighter::protocol
