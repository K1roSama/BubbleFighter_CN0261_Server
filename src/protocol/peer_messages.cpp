#include "protocol/peer_messages.h"

#include <algorithm>
#include <bit>
#include <cmath>
#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
namespace {
void ValidateObject(std::uint32_t object_id, std::uint8_t slot) {
  if (slot >= 8 || object_id < 1000000 ||
      (object_id - 1000000) / 100000 != slot) {
    throw std::invalid_argument(
        "Peer object does not belong to the authenticated room slot");
  }
}
}  // namespace

PeerMovement ReadPeerMovement(const LambdaPacket& packet,
                              std::uint8_t source_slot) {
  PacketReader reader(packet.payload);
  PeerMovement movement;
  movement.source_object = reader.ReadUnsigned();
  ValidateObject(movement.source_object, source_slot);
  if (packet.message_id == 32885) {
    for (int index = 0; index < 3; ++index) {
      static_cast<void>(reader.ReadShort());
    }
    for (int index = 0; index < 3; ++index) {
      static_cast<void>(reader.ReadByte());
    }
    static_cast<void>(reader.ReadShort());
  } else if (packet.message_id == 32922) {
    static_cast<void>(reader.ReadShort());
    const auto flags = reader.ReadByte();
    if ((flags & 0xC0) != 0) {
      throw std::invalid_argument("Unsupported optimized movement flags");
    }
    for (unsigned int bit = 0; bit < 6; ++bit) {
      if ((flags & (1u << bit)) != 0) {
        if (bit < 3) {
          static_cast<void>(reader.ReadShort());
        } else {
          static_cast<void>(reader.ReadByte());
        }
      }
    }
  } else if (packet.message_id == 32884) {
    movement.target_object = reader.ReadUnsigned();
    ValidateObject(*movement.target_object,
                   static_cast<std::uint8_t>(
                       (*movement.target_object - 1000000) / 100000));
    static_cast<void>(reader.ReadBoolean());
    movement.destination = reader.ReadUnsigned();
  } else {
    throw std::invalid_argument("Unsupported peer movement packet");
  }
  reader.RequireEnd();
  return movement;
}

LambdaPacket CreatePeerPingReply(const LambdaPacket& packet,
                                 std::uint8_t source_slot) {
  if (packet.message_id != 32840 || source_slot >= 8) {
    throw std::invalid_argument(
        "Expected peer ping from an authenticated room slot");
  }

  PacketReader reader(packet.payload);
  const auto slot = reader.ReadUnsigned();
  static_cast<void>(
      reader.ReadUnsigned());  // Original timeGetTime sample; preserve its
                               // complete bit pattern.
  reader.RequireEnd();
  if (slot != source_slot) {
    throw std::invalid_argument(
        "Peer ping slot does not match the authenticated sender");
  }

  return packet;
}

std::uint32_t ReadRelayPeerRequest(const LambdaPacket& packet) {
  if (packet.message_id != 8 && packet.message_id != 9) {
    throw std::invalid_argument("Expected relay peer registration request");
  }
  PacketReader reader(packet.payload);
  const auto peer = reader.ReadUnsigned();
  reader.RequireEnd();
  return peer;
}

bool IsPeerAction(std::uint16_t message_id) {
  return message_id == 32779 || message_id == 32780 || message_id == 32781 ||
         message_id == 32782 || message_id == 32785 || message_id == 32810 ||
         message_id == 32811 || message_id == 32813;
}

std::uint32_t ReadPeerActionObject(
    const LambdaPacket& packet, std::uint8_t source_slot,
    std::span<const std::uint32_t> weapon_definitions) {
  if (!IsPeerAction(packet.message_id)) {
    throw std::invalid_argument("Unsupported peer action");
  }
  PacketReader reader(packet.payload);
  std::uint32_t source_object = 0;
  if (packet.message_id == 32810) {
    const auto weapon = reader.ReadUnsigned();
    const auto weapon_slot = reader.ReadUnsigned();
    source_object = reader.ReadUnsigned();
    ValidateObject(source_object, source_slot);
    static_cast<void>(reader.ReadUnsigned());
    if (weapon_slot >= weapon_definitions.size() || weapon == 0 ||
        weapon_definitions[weapon_slot] != weapon) {
      throw std::invalid_argument(
          "Weapon update is not in the frozen equipped inventory");
    }
  } else {
    source_object = reader.ReadUnsigned();
    ValidateObject(source_object, source_slot);
    if (packet.message_id == 32779 || packet.message_id == 32785) {
      static_cast<void>(reader.ReadBoolean());
    } else if (packet.message_id == 32780) {
      static_cast<void>(reader.ReadUnsigned());
      static_cast<void>(reader.ReadByte());
      static_cast<void>(reader.ReadByte());
    } else if (packet.message_id == 32781) {
      const auto value = std::bit_cast<float>(reader.ReadUnsigned());
      if (!std::isfinite(value) || std::abs(value) > 10000000.0f) {
        throw std::invalid_argument(
            "Jump contains an invalid floating point value");
      }
      static_cast<void>(reader.ReadBoolean());
    } else if (packet.message_id == 32811) {
      static_cast<void>(reader.ReadUnsigned());
    }
  }
  reader.RequireEnd();
  return source_object;
}

std::uint32_t ReadRelayUseReport(const LambdaPacket& packet,
                                 std::uint8_t source_slot,
                                 std::u16string_view resource) {
  if (packet.message_id != 3) {
    throw std::invalid_argument("Expected ReportUseRelay");
  }
  PacketReader reader(packet.payload);
  const auto version = reader.ReadUnsigned();
  const auto enabled = reader.ReadBoolean();
  const auto map = reader.ReadString(256);
  const auto character_object = reader.ReadUnsigned();
  ValidateObject(character_object, source_slot);
  static_cast<void>(reader.ReadString(256));
  static_cast<void>(reader.ReadUnsigned());
  static_cast<void>(reader.ReadUnsigned());
  reader.RequireEnd();
  if (version != 717 || !enabled || map != resource) {
    throw std::invalid_argument(
        "Relay report does not match the supported game");
  }
  return character_object;
}
}  // namespace bubblefighter::protocol
