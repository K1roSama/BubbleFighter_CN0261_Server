#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_SESSION_SETTING_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_SESSION_SETTING_H_

#include <array>
#include <limits>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
// Names containing an offset refer to the original x86 object, never a wire
// offset. Unconfirmed fields remain explicit until their business meaning is
// established in IDA.
struct GameSetting {
  std::uint32_t mode = 1;
  std::uint32_t sub_mode = 0;
  std::array<std::u16string, 4> strings16;
  std::array<std::uint32_t, 5> values128{};
  bool flag4 = true;
  bool flag5 = true;
  std::array<std::uint32_t, 8> values148{
      0, 0, 0, 0, 1, std::numeric_limits<std::uint32_t>::max(), 0, 0};
  std::array<std::u16string, 6> strings180;
  std::uint32_t value348 = 0;
  std::vector<std::u16string> strings352;
  bool flag368 = false;
  std::uint32_t value372 = 0;
  bool flag376 = false;
  bool flag377 = false;
  std::uint32_t value380 = 0;
  std::uint32_t value384 = 0;
  std::u16string string388;
  std::u16string string416;
  bool flag444 = false;
  std::uint32_t value448 = 0;

  bool operator==(const GameSetting&) const = default;
};

struct SessionSetting {
  std::uint32_t state = 0;
  std::uint16_t room_id = 0;
  std::u16string title;
  // 0x6D3770 / 0x8783B0 display (memberCount - value42) / capacity in a room
  // card.
  std::uint16_t member_count = 0;
  std::uint16_t value42 = 0;
  std::uint16_t capacity = 0;
  bool password_required = false;
  std::u16string password;
  std::uint32_t kind = 0;
  GameSetting game;
  std::uint32_t value532 = std::numeric_limits<std::uint32_t>::max();
  std::uint64_t value536 = std::numeric_limits<std::uint64_t>::max();
  std::u16string string544;
  std::array<std::uint32_t, 4> values572{};
  bool flag588 = false;

  bool operator==(const SessionSetting&) const = default;
};

// These are server parsing limits, not claims about the original operator's
// policy.
inline constexpr std::size_t kSessionStringLimit = 256;
inline constexpr std::size_t kGameStringCountLimit = 16;

GameSetting ReadGameSetting(PacketReader& reader);
SessionSetting ReadSessionSetting(PacketReader& reader);
void WriteGameSetting(PacketWriter& writer, const GameSetting& setting);
void WriteSessionSetting(PacketWriter& writer, const SessionSetting& setting);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_SESSION_SETTING_H_
