#include "protocol/session_member.h"

#include <array>
#include <limits>
#include <stdexcept>

namespace bubblefighter::protocol {
namespace {
constexpr auto kUnset = std::numeric_limits<std::uint32_t>::max();
}  // namespace

void WriteSessionMember(PacketWriter& writer, const SessionMember& member,
                        std::uint32_t owner_slot) {
  if ((member.character &&
       (member.character->account_id == 0 || member.closed ||
        owner_slot >= 32 ||
        (member.team != 1 && member.team != 2 && member.team != 16) ||
        characters::CheckNickname(member.character->nickname) !=
            characters::CreationResult::kSuccess)) ||
      member.intrusion_state > 2 ||
      (!member.character && (!member.items.empty() || member.ready ||
                             member.intrusion_state != 0))) {
    throw std::invalid_argument(
        "Invalid session member identity or slot state");
  }

  // UserSlot::Serialize (0x41B060), defaults from 0x41AF40; peer transport is
  // negotiated separately.
  writer.WriteUnsigned(member.character ? 1 : 0);  // +4: occupied
  writer.WriteUnsigned(member.closed ? 1 : 0);     // bool +5: closed slot
  writer.WriteUnsigned(0);                         // bool +6
  // 0x88B0C0 compares UserSlot +129 with the receiving client's own slot before
  // registering its fighter.
  writer.WriteByte(member.character ? static_cast<std::uint8_t>(owner_slot)
                                    : 0xFF);
  writer.WriteUnsigned(
      member.character
          ? member.character->account_id
          : 0);  // +8: player identity, matching S3 and EquippedItemList.
  writer.WriteString(member.character ? member.character->nickname
                                      : u"");   // +12
  writer.WriteShort(member.character ? 1 : 0);  // +40
  writer.WriteShort(0);                         // +42
  writer.WriteByte(member.character ? member.team
                                    : 0);  // +124: team (S504 / 0x881150).
  writer.WriteUnsigned(member.character ? member.character->level
                                        : 0);  // +44: level icon (0x60EC40).
  writer.WriteUnsigned(member.ready ? 1 : 0);  // bool +125.
  writer.WriteUnsigned(member.activity_flags[0] ? 1 : 0);  // bool +126.
  writer.WriteUnsigned(member.activity_flags[1] ? 1 : 0);  // bool +127.
  for (int index = 0; index < 4; ++index) {
    writer.WriteUnsigned(0);  // bool +128, +60, +64, empty vector +68
  }
  for (const auto value :
       std::array<std::uint32_t, 7>{0, 0, kUnset, 0, 0, 0, kUnset}) {
    writer.WriteUnsigned(
        value);  // +84, +88, +108, bool +114, +136, bool +140, +144
  }
  for (int index = 0; index < 5; ++index) {
    writer.WriteShort(0xFFFF);  // +48..56
  }
  writer.WriteUnsigned(0);       // +156
  writer.WriteUnsigned(1);       // +160
  writer.WriteUnsigned(0);       // bool +148
  writer.WriteUnsigned(kUnset);  // +176
  writer.WriteString(u"");       // +180
  writer.WriteUnsigned(0);       // bool +208
  writer.WriteUnsigned(0);       // bool +209
  writer.WriteUnsigned(0);       // +212
  writer.WriteUnsigned(1);       // +216
  writer.WriteUnsigned(0);       // Nested scalar at +220.
  for (int index = 0; index < 8; ++index) {
    writer.WriteUnsigned(0);  // SocialSystemInfo at +228.
  }
  writer.WriteUnsigned(0);  // +132
  // 0x886210 hides nonzero slots and arms 0x561B10's 11000 ms intrusion timer
  // after S701. Normal starts keep zero so 0x53E320 creates every character
  // with the initial world.
  writer.WriteUnsigned(
      member.intrusion_state);  // +152; runtime intrusion is not yet supported.
  for (const auto value : std::array<std::uint32_t, 3>{0, kUnset, 0}) {
    writer.WriteUnsigned(value);  // +280, +284, +276
  }
}
}  // namespace bubblefighter::protocol
