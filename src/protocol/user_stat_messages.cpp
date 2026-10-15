#include "protocol/user_stat_messages.h"

#include <limits>
#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
namespace {
void WriteFighterInfo(PacketWriter& writer,
                      const characters::CharacterProfile& character,
                      std::uint32_t level) {
  // FighterInfo::Serialize (0x40D1D0). Unmapped fields retain constructor
  // defaults from 0x40CF80.
  writer.WriteUnsigned(character.account_id);  // +4: identity
  writer.WriteString(character.nickname);      // +8
  writer.WriteUnsigned(0);                     // +36
  writer.WriteUnsigned(level);  // +40 -> UserInfo +104, displayed level.
  writer.WriteUnsigned(0);      // +44
  writer.WriteUnsigned(0);      // +48, low half of a 64-bit field.
  writer.WriteUnsigned(0);      // +52
  writer.WriteUnsigned(0);      // +56
  writer.WriteDouble(0.0);      // +64
  writer.WriteUnsigned(0);      // +72
  writer.WriteUnsigned(0);      // bool +76
  for (int index = 0; index < 49; ++index) {
    writer.WriteUnsigned(
        0);  // +80 through +272, no completed battle statistics yet.
  }
  writer.WriteUnsigned(0);     // Empty vector at +284.
  writer.WriteUnsigned(1000);  // +276 constructor default.
  writer.WriteUnsigned(1);     // +280 constructor default.
  writer.WriteUnsigned(0);     // +300
  writer.WriteUnsigned(0);     // Nested scalar at +304.
  writer.WriteUnsigned(0);     // ArenaRating at +312, IEEE-754 float 0.
  for (int index = 0; index < 8; ++index) {
    writer.WriteUnsigned(0);  // SocialSystemInfo at +320.
  }
  for (int index = 0; index < 8; ++index) {
    writer.WriteUnsigned(0);  // +372 through +400.
  }
  writer.WriteString(u"");  // +404
  writer.WriteUnsigned(0);  // +368 is serialized last.
}
}  // namespace

LambdaPacket CreateUserStatReply(const characters::CharacterProfile& character,
                                 const characters::CharacterStats& stats) {
  constexpr auto maximum =
      static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max());
  if (character.account_id == 0 ||
      characters::CheckNickname(character.nickname) !=
          characters::CreationResult::kSuccess ||
      stats.lucci > maximum || stats.cash > maximum ||
      stats.progress.level < 1 || stats.progress.level > 200 ||
      stats.progress.level_start > stats.progress.experience ||
      stats.progress.experience > stats.progress.next_level ||
      stats.progress.next_level > maximum) {
    throw std::invalid_argument(
        "UserStat identity or signed client value is invalid");
  }

  // S105 (0x8CEC20), applied to UserInfo by 0x8A0CC0.
  PacketWriter writer;
  writer.WriteUnsigned(0);  // UserInfo +100, appearance color path 0x5C58D0 /
                            // 0x5CA260. Default color.
  writer.WriteUnsigned(stats.progress.experience);   // UserInfo +112
  writer.WriteUnsigned(stats.progress.level_start);  // UserInfo +108
  writer.WriteUnsigned(stats.progress.next_level);   // UserInfo +116
  writer.WriteUnsigned(stats.lucci);  // UserInfo +120, moneyType 2.
  writer.WriteUnsigned(stats.cash);   // UserInfo +124, moneyType 1 / 3.
  writer.WriteUnsigned(0);            // UserInfo +128 boolean.
  writer.WriteUnsigned(0);            // UserInfo +132
  writer.WriteString(u"");            // Packet +36
  writer.WriteUnsigned(0);            // Packet +64
  WriteFighterInfo(writer, character, stats.progress.level);
  return {105, writer.GetBytes()};
}
}  // namespace bubblefighter::protocol
