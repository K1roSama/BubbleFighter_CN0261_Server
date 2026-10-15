#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_SESSION_MEMBER_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_SESSION_MEMBER_H_

#include <array>
#include <optional>

#include "features/characters/character_repository.h"
#include "features/inventory/inventory_repository.h"
#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
struct SessionMember {
  std::optional<characters::CharacterProfile> character;
  std::vector<inventory::InventoryItem> items;
  std::uint8_t team = 0;
  bool ready = false;
  bool closed = false;
  std::array<bool, 2> activity_flags{};  // UserSlot +126 / +127, C505's
                                         // additional activity indicators.
  // UserSlot +152: zero for a normal start; nonzero invokes the intrusion
  // loading/spawn path.
  std::uint32_t intrusion_state = 0;
};

void WriteSessionMember(PacketWriter& writer, const SessionMember& member,
                        std::uint32_t owner_slot);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_SESSION_MEMBER_H_
