#include "protocol/game_result_messages.h"

#include <limits>
#include <set>
#include <stdexcept>

#include "features/characters/character_repository.h"
#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
namespace {
constexpr auto kUnset = std::numeric_limits<std::uint32_t>::max();

void WriteResultPlayer(PacketWriter& writer, const GameResultPlayer* player) {
  // ScoreData::Serialize / Deserialize: 0x419180 / 0x4195B0. Offsets refer to
  // the 380-byte client record.
  writer.WriteUnsigned(player ? player->slot : kUnset);   // +0
  writer.WriteUnsigned(player ? player->account_id : 0);  // +4
  writer.WriteString(player ? player->nickname : u"");    // +8
  writer.WriteUnsigned(player ? player->team : 0);        // +36
  writer.WriteUnsigned(player ? player->progress.level
                              : 0);  // +40: level, matching UserSlot +44.
  writer.WriteUnsigned(player ? player->score.bubble_count : 0);  // +44
  writer.WriteUnsigned(player ? player->score.pang_count : 0);    // +48
  writer.WriteUnsigned(0);                                        // +52
  writer.WriteUnsigned(player ? player->score.rescue_count : 0);  // +56
  for (int offset = 60; offset <= 96; offset += 4) {
    writer.WriteUnsigned(player && offset == 64
                             ? player->award.score
                             : 0);  // lbRedScore / lbBlueScore.
  }

  // +100..108 are not serialized.
  for (int offset = 112; offset <= 140; offset += 4) {
    writer.WriteUnsigned(0);
  }
  writer.WriteUnsigned(player ? player->progress.level_start : 0);  // +144
  writer.WriteUnsigned(player ? player->progress.experience : 0);   // +148
  writer.WriteUnsigned(player ? player->progress.next_level : 0);   // +152
  writer.WriteUnsigned(player ? player->experience_gain
                              : 0);                       // +156: lbGainExp.
  writer.WriteUnsigned(player ? player->lucci_gain : 0);  // +160: lbMyLucci.
  writer.WriteUnsigned(player ? 1 : 0);  // bool +164: still participating.
  writer.WriteUnsigned(player && player->level_up
                           ? 1
                           : 0);  // bool +165: result row level-up indicator.
  for (int index = 0; index < 4; ++index) {
    writer.WriteUnsigned(
        0);  // bool +166, +167, +220, +221: four bytes each, not one.
  }
  writer.WriteByte(0);      // +222 is the one-byte field.
  writer.WriteUnsigned(0);  // +172
  writer.WriteUnsigned(0);  // Empty string vector at +204.
  writer.WriteString(u"");  // +176
  for (int index = 0; index < 6; ++index) {
    writer.WriteUnsigned(
        0);  // +244, +224, +232, +236, +228, +304 (wire order).
  }
  writer.WriteUnsigned(0);  // bool +308
  writer.WriteUnsigned(0);  // +312 (32-bit float representation).
  writer.WriteUnsigned(
      player ? player->award.grade
             : kUnset);     // +316: Rank_list.smf frame, 0 = SSS ... 8 = F.
  writer.WriteUnsigned(0);  // +320
  writer.WriteUnsigned(0);  // bool +324
  writer.WriteUnsigned(0);  // bool +325
  writer.WriteUnsigned(0);  // +328 (32-bit float representation).
  writer.WriteUnsigned(0);  // +332 (32-bit float representation).
  writer.WriteUnsigned(0);  // bool +336
  for (int offset = 340; offset <= 360; offset += 4) {
    writer.WriteUnsigned(0);
  }
  writer.WriteShort(0);  // +364
  writer.WriteUnsigned(
      0);  // Nested object +368 writes its +4 scalar (0x88E6C0).
}
}  // namespace

LambdaPacket CreateGameResultReply(
    const std::array<std::uint16_t, 2>& team_scores,
    std::span<const GameResultPlayer> players) {
  if (team_scores[0] > 200 || team_scores[1] > 200 || players.empty() ||
      players.size() > 8) {
    throw std::invalid_argument(
        "Game result exceeds the supported ordinary room rules");
  }

  std::array<const GameResultPlayer*, 16> slots{};
  std::set<std::uint32_t> accounts;
  for (const auto& player : players) {
    if (player.slot >= 8 || slots[player.slot] || player.account_id == 0 ||
        !accounts.insert(player.account_id).second || player.team < 1 ||
        player.team > 2 ||
        characters::CheckNickname(player.nickname) !=
            characters::CreationResult::kSuccess ||
        player.progress.level < 1 || player.progress.level > 200 ||
        player.progress.level_start > player.progress.experience ||
        player.progress.experience > player.progress.next_level ||
        player.progress.next_level > 2147483647u ||
        player.experience_gain > player.progress.experience ||
        player.lucci_gain > 2147483647u || player.award.score > 2147483647u ||
        (player.award.grade > 8 && player.award.grade != kUnset) ||
        (player.award.medal != 0 &&
         (player.award.medal < 4 || player.award.medal > 9))) {
      throw std::invalid_argument(
          "Game result has an invalid player identity, team or slot");
    }
    slots[player.slot] = &player;
  }

  PacketWriter writer;
  writer.WriteUnsigned(17);  // Original ScoreData team table capacity,
                             // including special-mode entries.
  writer.WriteUnsigned(team_scores[0] == team_scores[1]
                           ? 0
                           : (team_scores[0] > team_scores[1] ? 1 : 2));
  writer.WriteUnsigned(0);  // bool at ScoreData +300.
  for (std::size_t team = 1; team <= 17; ++team) {
    writer.WriteUnsigned(0);  // ScoreData +8 array.
    writer.WriteUnsigned(team <= team_scores.size() ? team_scores[team - 1]
                                                    : 0);  // +80 array.
    writer.WriteUnsigned(0);  // +152 array (special-mode score).
    writer.WriteUnsigned(0);  // +224 array.
  }
  for (const auto* player : slots) {
    WriteResultPlayer(writer, player);
  }
  for (const auto* player : slots) {
    writer.WriteUnsigned(
        player ? player->award.medal
               : 0);  // +6384: ordinary medal type indexed by slot.
    writer.WriteUnsigned(kUnset);  // +6448 array.
  }
  return {729, writer.GetBytes()};
}
}  // namespace bubblefighter::protocol
