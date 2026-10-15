#include "protocol/battle_state_messages.h"

#include <bit>
#include <cmath>
#include <set>
#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
namespace {
void ValidateFloat(std::uint32_t bits, float minimum, float maximum) {
  const auto value = std::bit_cast<float>(bits);
  if (!std::isfinite(value) || value < minimum || value > maximum) {
    throw std::invalid_argument(
        "Battle state contains an invalid floating point value");
  }
}

void ValidateOwnedObject(std::uint32_t object_id, std::uint8_t source_slot) {
  if (source_slot >= 8 || object_id < 1000000 ||
      (object_id - 1000000) / 100000 != source_slot) {
    throw std::invalid_argument(
        "Battle state object is outside the authenticated slot partition");
  }
}

std::uint32_t ReadTarget(PacketReader& reader, std::uint8_t source_slot) {
  const auto target = reader.ReadUnsigned();
  if (source_slot >= 8 || target >= 8 || target == source_slot) {
    throw std::invalid_argument(
        "Battle synchronization requires another room slot");
  }
  return target;
}

LambdaPacket RewriteSource(const LambdaPacket& packet, std::uint16_t message_id,
                           std::uint8_t source_slot) {
  PacketWriter writer;
  writer.WriteUnsigned(source_slot);
  writer.WriteBytes(std::span<const std::uint8_t>(packet.payload).subspan(4));
  return {message_id, writer.GetBytes()};
}
}  // namespace

DirectedBattleState ReadUserState(const LambdaPacket& packet,
                                  std::uint8_t source_slot) {
  if (packet.message_id != 2015) {
    throw std::invalid_argument("Expected UserState initialization");
  }
  PacketReader reader(packet.payload);
  const auto target = ReadTarget(reader, source_slot);
  const auto state = reader.ReadUnsigned();
  // Ordinary ChrPlayer state dispatch uses categories 1xx through 5xx; zero is
  // uninitialized.
  if (state != 0 && (state < 100 || state >= 600)) {
    throw std::invalid_argument("Unsupported character state category");
  }
  ValidateFloat(reader.ReadUnsigned(), 0.0f, 100000.0f);
  static_cast<void>(reader.ReadUnsigned());
  for (int index = 0; index < 3; ++index) {
    ValidateFloat(reader.ReadUnsigned(), -10000000.0f, 10000000.0f);
  }
  // 0x5610D0 exports GameWorld's per-slot allocation counter, not ChrPlayer
  // +404.
  const auto next_object_id = reader.ReadUnsigned();
  ValidateOwnedObject(next_object_id, source_slot);
  const auto selected_item = reader.ReadUnsigned();
  if (selected_item > 1 && selected_item != 0xFFFFFFFFu) {
    throw std::invalid_argument("UserState item slot is invalid");
  }
  // 0x5610D0 always exports two entries. The receiver indexes them without
  // checking their size.
  if (reader.ReadUnsigned() != 2) {
    throw std::invalid_argument(
        "UserState must contain exactly two item slots");
  }
  for (int index = 0; index < 4; ++index) {
    static_cast<void>(reader.ReadUnsigned());
  }
  reader.RequireEnd();
  return {target, RewriteSource(packet, 976, source_slot)};
}

DirectedBattleState ReadMapItemState(const LambdaPacket& packet,
                                     std::uint8_t source_slot) {
  if (packet.message_id != 2017) {
    throw std::invalid_argument("Expected GoMapItemSync initialization");
  }
  PacketReader reader(packet.payload);
  const auto target = ReadTarget(reader, source_slot);
  const auto count = reader.ReadUnsigned();
  // Server resource bound, not a claimed original-server limit.
  if (count > 256 ||
      reader.Remaining() != static_cast<std::size_t>(count) * 20) {
    throw std::invalid_argument(
        "Map item synchronization count or length is invalid");
  }
  std::set<std::uint32_t> identifiers;
  for (std::uint32_t index = 0; index < count; ++index) {
    const auto object_id = reader.ReadUnsigned();
    ValidateOwnedObject(object_id, source_slot);
    if (!identifiers.insert(object_id).second) {
      throw std::invalid_argument(
          "Map item synchronization contains duplicate objects");
    }
    for (int axis = 0; axis < 3; ++axis) {
      ValidateFloat(reader.ReadUnsigned(), -10000000.0f, 10000000.0f);
    }
    if (reader.ReadUnsigned() > 1) {
      throw std::invalid_argument("Unsupported map item synchronization kind");
    }
  }
  reader.RequireEnd();
  return {target, RewriteSource(packet, 981, source_slot)};
}
}  // namespace bubblefighter::protocol
