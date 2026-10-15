#include "protocol/peer_damage_messages.h"

#include <bit>
#include <cmath>
#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
PeerDetonation ReadPeerDetonation(const LambdaPacket& packet) {
  if (packet.message_id != 32900) {
    throw std::invalid_argument("Unsupported peer grenade detonation packet");
  }
  PacketReader reader(packet.payload);
  PeerDetonation detonation;
  detonation.projectile_id = reader.ReadUnsigned();
  for (int component = 0; component < 9; ++component) {
    const auto value = std::bit_cast<float>(reader.ReadUnsigned());
    if (!std::isfinite(value) || std::abs(value) > 10000000.0f) {
      throw std::invalid_argument(
          "Detonation contains an invalid vector component");
    }
  }
  detonation.splash_id = reader.ReadUnsigned();
  static_cast<void>(reader.ReadUnsigned());  // Original low-24-bit clock; upper
                                             // byte is unspecified.
  reader.RequireEnd();
  if (detonation.projectile_id >= 800000 || detonation.splash_id >= 800000 ||
      detonation.projectile_id / 100000 != detonation.splash_id / 100000) {
    throw std::invalid_argument(
        "Detonation objects must belong to the same player partition");
  }
  return detonation;
}

bool IsPeerExplosion(std::uint16_t message_id) {
  return message_id == 32776 || message_id == 32923;
}

PeerExplosion ReadPeerExplosion(const LambdaPacket& packet) {
  if (!IsPeerExplosion(packet.message_id)) {
    throw std::invalid_argument("Unsupported peer bullet explosion packet");
  }
  PacketReader reader(packet.payload);
  PeerExplosion explosion;
  explosion.projectile_id = reader.ReadUnsigned();
  const auto target = reader.ReadUnsigned();
  if (explosion.projectile_id >= 800000) {
    throw std::invalid_argument(
        "Exploded projectile is outside the supported player partitions");
  }
  // The original receiver destroys the bullet without a victim when the target
  // is -1.
  if (target != 0xFFFFFFFFu) {
    if (target < 1000000 || (target - 1000000) / 100000 >= 8) {
      throw std::invalid_argument(
          "Explosion target is outside the supported player partitions");
    }
    explosion.target_object = target;
    explosion.target_slot =
        static_cast<std::uint8_t>((target - 1000000) / 100000);
  }
  for (int axis = 0; axis < 3; ++axis) {
    if (packet.message_id == 32923) {
      if ((reader.ReadShort() & 0x7C00) == 0x7C00) {
        throw std::invalid_argument(
            "Explosion contains a nonfinite half precision coordinate");
      }
    } else {
      const auto value = std::bit_cast<float>(reader.ReadUnsigned());
      if (!std::isfinite(value) || std::abs(value) > 10000000.0f) {
        throw std::invalid_argument(
            "Explosion contains an invalid collision coordinate");
      }
    }
  }
  // 0x538620 / 0x538560 write this bool as four bytes, after the three
  // coordinates.
  explosion.apply_damage = reader.ReadBoolean();
  reader.RequireEnd();
  return explosion;
}

PeerHit ReadPeerHit(const LambdaPacket& packet) {
  if (packet.message_id != 32777) {
    throw std::invalid_argument("Unsupported peer hit packet");
  }
  PacketReader reader(packet.payload);
  PeerHit hit;
  hit.projectile_id = reader.ReadUnsigned();
  hit.target_object = reader.ReadUnsigned();
  if (hit.projectile_id >= 800000 || hit.target_object < 1000000 ||
      (hit.target_object - 1000000) / 100000 >= 8) {
    throw std::invalid_argument(
        "Hit identifier is outside the supported player partitions");
  }
  hit.target_slot =
      static_cast<std::uint8_t>((hit.target_object - 1000000) / 100000);
  for (int index = 0; index < 3; ++index) {
    const auto coordinate = std::bit_cast<float>(reader.ReadUnsigned());
    if (!std::isfinite(coordinate) || std::abs(coordinate) > 10000000.0f) {
      throw std::invalid_argument(
          "Hit contains an invalid collision coordinate");
    }
  }
  reader.RequireEnd();
  return hit;
}

std::uint32_t ReadPeerProjectileRemoval(const LambdaPacket& packet) {
  if (packet.message_id != 32906) {
    throw std::invalid_argument("Unsupported peer projectile removal packet");
  }
  PacketReader reader(packet.payload);
  const auto projectile_id = reader.ReadUnsigned();
  if (projectile_id >= 800000) {
    throw std::invalid_argument(
        "Removed projectile is outside the supported player partitions");
  }
  reader.RequireEnd();
  return projectile_id;
}
}  // namespace bubblefighter::protocol
