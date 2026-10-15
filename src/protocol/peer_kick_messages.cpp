#include "protocol/peer_kick_messages.h"

#include <bit>
#include <cmath>
#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
namespace {
std::uint32_t ReadObject(PacketReader& reader) {
  const auto object = reader.ReadUnsigned();
  if (object < 1000000 || (object - 1000000) / 100000 >= 8) {
    throw std::invalid_argument("Kick object is outside the player partitions");
  }

  return object;
}

bool ReadFlag(PacketReader& reader) {
  return reader
      .ReadBoolean();  // 0x538A40 calls the four-byte bool writer 0xB147C0.
}
}  // namespace

bool IsPeerKick(std::uint16_t message_id) {
  return message_id == 32791 || message_id == 32792 || message_id == 32794 ||
         message_id == 32795 || message_id == 32796 || message_id == 32835 ||
         message_id == 32837 || message_id == 32815 || message_id == 32816;
}

PeerKickEvent ReadPeerKick(const LambdaPacket& packet) {
  if (!IsPeerKick(packet.message_id)) {
    throw std::invalid_argument("Unsupported peer kick packet");
  }

  PacketReader reader(packet.payload);
  PeerKickEvent event;
  if (packet.message_id == 32815 || packet.message_id == 32816) {
    // Both original humiliation packets use actor, target (0x891C60 /
    // 0x538B70).
    event.kind = packet.message_id == 32815 ? PeerKickKind::kHumiliationStart
                                            : PeerKickKind::kHumiliationEnd;
    event.actor_object = ReadObject(reader);
    event.victim_object = ReadObject(reader);
  } else if (packet.message_id == 32835) {
    event.kind = PeerKickKind::kGotKicked;
    event.victim_object = ReadObject(reader);
  } else if (packet.message_id == 32792 || packet.message_id == 32837) {
    event.kind =
        packet.message_id == 32792 ? PeerKickKind::kFailed : PeerKickKind::kEnd;
    event.actor_object = ReadObject(reader);
  } else if (packet.message_id == 32791) {
    // KickStart reverses the two objects used by TryKicked and Kicked.
    event.kind = PeerKickKind::kStart;
    event.actor_object = ReadObject(reader);
    event.victim_object = ReadObject(reader);
    event.style = reader.ReadUnsigned();
    if (event.style != 1 && event.style != 2) {
      throw std::invalid_argument("Unsupported ordinary kick style");
    }
  } else {
    if (packet.message_id == 32795) {
      event.kind = PeerKickKind::kConfirmed;
      event.succeeded = ReadFlag(reader);
    } else if (packet.message_id == 32796) {
      event.kind = PeerKickKind::kProtected;
    }

    event.victim_object = ReadObject(reader);
    event.actor_object = ReadObject(reader);
    if (packet.message_id != 32794) {
      for (int axis = 0; axis < 3; ++axis) {
        const auto coordinate = std::bit_cast<float>(reader.ReadUnsigned());
        if (!std::isfinite(coordinate) || std::abs(coordinate) > 10000000.0f) {
          throw std::invalid_argument(
              "Kick direction contains an invalid coordinate");
        }
      }
    }
    if (packet.message_id == 32795) {
      event.special_kick = ReadFlag(reader);
    }
  }

  reader.RequireEnd();
  return event;
}
}  // namespace bubblefighter::protocol
