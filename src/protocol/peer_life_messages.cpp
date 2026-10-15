#include "protocol/peer_life_messages.h"

#include <bit>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
namespace {
bool ReadFlag(PacketReader& reader) {
  // Packed bool8 memory fields are serialized as u32 by the original 0xB147C0.
  return reader.ReadBoolean();
}

void ValidateObject(std::uint32_t object_id) {
  if (object_id < 1000000 || (object_id - 1000000) / 100000 >= 8) {
    throw std::invalid_argument(
        "Peer life object is outside the player partitions");
  }
}

void ReadCoordinate(PacketReader& reader, float minimum) {
  const auto value = std::bit_cast<float>(reader.ReadUnsigned());
  if (!std::isfinite(value) || value < minimum || value > 10000000.0f) {
    throw std::invalid_argument("Peer life vector or distance is invalid");
  }
}
}  // namespace

bool IsPeerLife(std::uint16_t message_id) {
  return message_id == 32787 || message_id == 32793 || message_id == 32903 ||
         message_id == 32807 || message_id == 32805 || message_id == 32836 ||
         message_id == 32801 || message_id == 32806 || message_id == 32788 ||
         message_id == 32790;
}

PeerLifePoint ReadPeerLifePoint(const LambdaPacket& packet,
                                std::uint8_t source_slot) {
  if (packet.message_id != 32920 || source_slot >= 8) {
    throw std::invalid_argument(
        "Unsupported peer life point packet or source slot");
  }

  PacketReader reader(packet.payload);
  PeerLifePoint report;
  report.character_object = reader.ReadUnsigned();
  ValidateObject(report.character_object);
  report.remaining_points = reader.ReadUnsigned();
  const auto effect = reader.ReadUnsigned();
  reader.RequireEnd();
  if ((report.character_object - 1000000) / 100000 != source_slot) {
    throw std::invalid_argument(
        "Life point report does not belong to its authenticated source");
  }
  // 0x59589D clamps the signed result to zero; 0x8C8880 writes all three fields
  // as four bytes.
  if (report.remaining_points > static_cast<std::uint32_t>(
                                    std::numeric_limits<std::int32_t>::max()) ||
      effect < 1 || effect > 2) {
    throw std::invalid_argument(
        "Life point report contains an invalid count or effect");
  }
  report.effect = static_cast<PeerLifePointEffect>(effect);
  return report;
}

PeerLifeEvent ReadPeerLife(const LambdaPacket& packet) {
  if (!IsPeerLife(packet.message_id)) {
    throw std::invalid_argument("Unsupported peer life packet");
  }
  PacketReader reader(packet.payload);
  const auto first = reader.ReadUnsigned();
  if (packet.message_id == 32788 || packet.message_id == 32790) {
    ValidateObject(first);
    PeerLifeEvent event;
    event.victim_object = first;
    event.kind = packet.message_id == 32788 ? PeerLifeKind::kStunned
                                            : PeerLifeKind::kEndStunned;
    if (packet.message_id == 32788) {
      event.actor_object = reader.ReadUnsigned();
      ValidateObject(event.actor_object);
      for (int axis = 0; axis < 3; ++axis) {
        ReadCoordinate(reader, -10000000.0f);
      }
      event.weapon_definition = reader.ReadUnsigned();
    }
    reader.RequireEnd();
    return event;
  }
  if (packet.message_id == 32805 || packet.message_id == 32836 ||
      packet.message_id == 32801) {
    ValidateObject(first);
    reader.RequireEnd();
    PeerLifeEvent event;
    event.kind = packet.message_id == 32801   ? PeerLifeKind::kBubbleSettled
                 : packet.message_id == 32805 ? PeerLifeKind::kDying
                                              : PeerLifeKind::kDead;
    event.victim_object = first;
    return event;
  }
  const auto second = reader.ReadUnsigned();
  ValidateObject(first);
  if (packet.message_id == 32806) {
    reader.RequireEnd();
    // 0x61E691 reports ordinary timed escape with no rescuer. -2 is the
    // separate item/skill path.
    if (second != 0xFFFFFFFFu && second != 0xFFFFFFFEu) {
      throw std::invalid_argument(
          "Self escape requires an original no-rescuer or item sentinel");
    }
    PeerLifeEvent event;
    event.kind = PeerLifeKind::kSelfPopOut;
    event.victim_object = first;
    event.actor_object = second;
    return event;
  }
  ValidateObject(second);
  PeerLifeEvent event;
  // Rescue reverses the object order used by Knocked, TryRescued and PopOut2.
  event.victim_object = packet.message_id == 32807 ? second : first;
  event.actor_object = packet.message_id == 32807 ? first : second;
  if (packet.message_id == 32787) {
    for (int axis = 0; axis < 3; ++axis) {
      ReadCoordinate(reader, -10000000.0f);
    }
    event.weapon_definition = reader.ReadUnsigned();
    event.direct_death = ReadFlag(reader);
    static_cast<void>(ReadFlag(reader));  // Original one-hit presentation flag.
    event.tick =
        reader.ReadUnsigned() &
        0xFFFFFFu;  // High byte is uninitialized in the original sender.
    if (ReadFlag(reader)) {
      // Room entry at 0x882D40 selects victim authority, independently of this
      // untrusted flag.
      throw std::invalid_argument(
          "Ordinary rooms require victim-reported knockdown");
    }
  } else if (packet.message_id == 32793) {
    event.kind = PeerLifeKind::kTryRescue;
  } else if (packet.message_id == 32903) {
    event.kind = PeerLifeKind::kPopOut;
    ReadCoordinate(reader, 0.0f);
  } else {
    event.kind = PeerLifeKind::kRescue;
    event.special_rescue = ReadFlag(reader);
  }
  reader.RequireEnd();
  return event;
}
}  // namespace bubblefighter::protocol
