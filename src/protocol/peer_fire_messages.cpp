#include "protocol/peer_fire_messages.h"

#include <algorithm>
#include <bit>
#include <cmath>
#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
namespace {
std::uint8_t ReadObjectSlot(std::uint32_t object_id) {
  if (object_id < 1000000 || (object_id - 1000000) / 100000 >= 8) {
    throw std::invalid_argument(
        "Fire object is outside the supported player partitions");
  }
  return static_cast<std::uint8_t>((object_id - 1000000) / 100000);
}

void ReadVectorComponent(PacketReader& reader, bool compressed) {
  if (compressed) {
    // 0xB33B70 and 0xB33CF0 encode/decode IEEE 754 binary16, including
    // subnormals.
    if ((reader.ReadShort() & 0x7C00) == 0x7C00) {
      throw std::invalid_argument(
          "Fire contains a nonfinite half precision value");
    }
  } else {
    const auto value = std::bit_cast<float>(reader.ReadUnsigned());
    if (!std::isfinite(value) || std::abs(value) > 10000000.0f) {
      throw std::invalid_argument(
          "Fire contains an invalid floating point value");
    }
  }
}
}  // namespace

bool IsPeerFire(std::uint16_t message_id) {
  return message_id == 32898 || message_id == 32901 || message_id == 32924;
}

PeerFire ReadPeerFire(const LambdaPacket& packet, std::uint8_t source_slot,
                      std::span<const std::uint32_t> weapon_definitions) {
  if (!IsPeerFire(packet.message_id)) {
    throw std::invalid_argument("Unsupported peer fire packet");
  }
  PacketReader reader(packet.payload);
  PeerFire fire;
  fire.source_object = reader.ReadUnsigned();
  if (ReadObjectSlot(fire.source_object) != source_slot) {
    throw std::invalid_argument(
        "Fire source does not belong to the authenticated room slot");
  }
  if (packet.message_id == 32898) {
    const auto definition = reader.ReadShort();
    if (definition == 0 ||
        std::find(weapon_definitions.begin(), weapon_definitions.end(),
                  definition) == weapon_definitions.end()) {
      throw std::invalid_argument(
          "Fire weapon is not in the frozen equipped inventory");
    }
    fire.weapon_definition = definition;
    fire.weapon_slot = static_cast<std::uint8_t>(
        std::find(weapon_definitions.begin(), weapon_definitions.end(),
                  definition) -
        weapon_definitions.begin());
    static_cast<void>(reader.ReadByte());
  } else {
    const auto weapon_slot = reader.ReadByte();
    if (weapon_slot >= weapon_definitions.size() ||
        weapon_definitions[weapon_slot] == 0) {
      throw std::invalid_argument(
          "Fire weapon slot is not a supported equipped slot");
    }
    fire.weapon_definition = weapon_definitions[weapon_slot];
    fire.weapon_slot = weapon_slot;
  }
  for (int index = 0; index < 6; ++index) {
    ReadVectorComponent(reader, packet.message_id == 32924);
  }
  const auto target_object = reader.ReadUnsigned();
  if (target_object != 0xFFFFFFFFu) {
    fire.target_object = target_object;
    fire.target_slot = ReadObjectSlot(target_object);
  }
  fire.attack_flags = reader.ReadByte();
  fire.first_projectile_id = reader.ReadUnsigned();
  if (fire.first_projectile_id / 100000 != source_slot) {
    throw std::invalid_argument(
        "Fire projectile does not belong to the authenticated room slot");
  }
  // Only the low 24 bits are initialized by the original sender. Preserve the
  // whole wire value.
  static_cast<void>(reader.ReadUnsigned());
  if (packet.message_id != 32898) {
    static_cast<void>(reader.ReadShort());
  }
  reader.RequireEnd();
  return fire;
}
}  // namespace bubblefighter::protocol
