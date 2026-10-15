#include "protocol/game_start_messages.h"

#include <limits>
#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
GameStartRequest ReadGameStartRequest(const LambdaPacket& packet) {
  if (packet.message_id != 503) {
    throw std::invalid_argument("Expected RoomReqEnterGame");
  }
  PacketReader reader(packet.payload);
  GameStartRequest request;
  request.flag4 = reader.ReadBoolean();
  for (auto& value : request.strings8) {
    value = reader.ReadString(256);
  }
  request.difficulty = reader.ReadUnsigned();
  for (auto& value : request.values100) {
    value = reader.ReadUnsigned();
  }
  for (std::size_t index = 0; index < 3; ++index) {
    request.event_flags[index] = reader.ReadBoolean();
  }
  // 0x89BF20 leaves object +156 uninitialized; 0x41A760/0xB147C0 zero-extend
  // that byte onto the wire. C107 initializes this field for tutorials, but an
  // ordinary C503 must not use it to select a game mode.
  request.value156 = reader.ReadUnsigned();
  if (request.value156 > 0xFFu) {
    throw std::invalid_argument(
        "RoomReqEnterGame legacy field exceeds the original byte range");
  }
  for (std::size_t index = 3; index < request.event_flags.size(); ++index) {
    request.event_flags[index] = reader.ReadBoolean();
  }
  reader.RequireEnd();
  return request;
}

void ValidateMapLoadedRequest(const LambdaPacket& packet) {
  if (packet.message_id != 701) {
    throw std::invalid_argument("Expected GameReportMapLoadingComplete");
  }
  PacketReader reader(packet.payload);
  for (int index = 0; index < 4; ++index) {
    static_cast<void>(reader.ReadUnsigned());
  }
  reader.RequireEnd();
}

bool ReadRelayConnectionReport(const LambdaPacket& packet) {
  if (packet.message_id != 601) {
    throw std::invalid_argument("Expected ReportRelayConnected");
  }
  PacketReader reader(packet.payload);
  const auto connected = reader.ReadBoolean();
  reader.RequireEnd();
  return connected;
}

void ValidatePeerTableReport(const LambdaPacket& packet) {
  if (packet.message_id != 607 || !packet.payload.empty()) {
    throw std::invalid_argument("Expected empty ReportTableSetHolePunchingEnd");
  }
}

LambdaPacket CreateGameEntryReply(GameStartResult result,
                                  std::u16string_view map,
                                  std::uint32_t state) {
  PacketWriter writer;
  writer.WriteUnsigned(static_cast<std::uint32_t>(result));
  writer.WriteString(map);
  writer.WriteUnsigned(0);  // Special-mode inputs +36/+40 are unused by
                            // supported ordinary rooms.
  writer.WriteUnsigned(0);
  writer.WriteUnsigned(state);
  writer.WriteUnsigned(0);
  writer.WriteUnsigned(0);
  writer.WriteUnsigned(0);
  return {510, writer.GetBytes()};
}

LambdaPacket CreateSessionSignatureReply(std::uint32_t mode) {
  if (mode == 0 || mode > std::numeric_limits<std::uint16_t>::max()) {
    throw std::invalid_argument(
        "Session signature mode must fit the original nonzero u16 field");
  }
  // S522 / 0x88AEF0 installs SessionSignatureData; 0x896050 compares its mode
  // to GameSession +96. 0x891130/0x891180: u16 mode, opaque byte, u32
  // byte-vector length and vector contents.
  PacketWriter writer;
  writer.WriteShort(static_cast<std::uint16_t>(mode));
  writer.WriteByte(0);
  writer.WriteUnsigned(
      0);  // No operator-specific opaque signature bytes are assigned.
  return {522, writer.GetBytes()};
}

LambdaPacket CreatePeerIdentityReply(std::uint32_t peer_id) {
  PacketWriter writer;
  writer.WriteShort(0);  // TCP relay only; no UDP probe endpoint is advertised.
  writer.WriteUnsigned(peer_id);
  writer.WriteUnsigned(0);
  return {607, writer.GetBytes()};
}

LambdaPacket CreatePeerTableReply(std::uint32_t slot, std::uint32_t peer_id) {
  if (slot >= 8 || peer_id == 0 || peer_id == 0xFFFFFFFFu) {
    throw std::invalid_argument("Peer table identity is invalid");
  }
  PacketWriter writer;
  writer.WriteUnsigned(slot);
  writer.WriteUnsigned(peer_id);
  writer.WriteUnsigned(
      0);  // No direct peer addresses: all supported movement uses the relay.
  writer.WriteShort(0);
  writer.WriteUnsigned(0);
  writer.WriteShort(0);
  return {610, writer.GetBytes()};
}

LambdaPacket CreateMapLoadedReply(std::uint32_t slot) {
  if (slot >= 8) {
    throw std::invalid_argument("Map loading slot is invalid");
  }
  PacketWriter writer;
  writer.WriteUnsigned(slot);
  return {701, writer.GetBytes()};
}

LambdaPacket CreateEverybodyLoadedReply() {
  PacketWriter writer;
  writer.WriteUnsigned(1);  // GameSession +16699 selects Relay transport.
  writer.WriteUnsigned(0);  // GameSession +16700 selects SuperPeer transport.
  writer.WriteUnsigned(0);
  return {702, writer.GetBytes()};
}

LambdaPacket CreateGameCountdownReply(std::uint32_t milliseconds) {
  PacketWriter writer;
  writer.WriteUnsigned(milliseconds);
  return {703, writer.GetBytes()};
}

LambdaPacket CreateGameStartReply(std::uint32_t duration_milliseconds,
                                  std::uint32_t tracking_id) {
  PacketWriter writer;
  writer.WriteUnsigned(duration_milliseconds);
  writer.WriteUnsigned(
      0);  // Telemetry +8; no operator-specific value is assigned.
  writer.WriteUnsigned(tracking_id);
  return {704, writer.GetBytes()};
}
}  // namespace bubblefighter::protocol
