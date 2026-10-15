#include "protocol/score_messages.h"

#include <algorithm>
#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
LambdaPacket CreatePlayerScoreReply(std::uint8_t slot,
                                    const PlayerBattleScore& score,
                                    std::uint32_t personal_score) {
  if (slot >= 8) {
    throw std::invalid_argument(
        "Player score slot is outside the supported room roster");
  }

  PacketWriter writer;
  writer.WriteByte(0);        // No team score update (0x887FA0).
  writer.WriteShort(0xFFFF);  // Unchanged team score sentinel.
  writer.WriteByte(slot);
  writer.WriteByte(
      0xFF);  // No second player; the original receiver reads a signed slot.
  writer.WriteUnsigned(4);
  writer.WriteByte(0);  // ScoreData player +44: lbBubbleCnt / lbRedBubbleCnt.
  writer.WriteShort(score.bubble_count);
  writer.WriteByte(24);  // Player +48: lbPangCnt / lbRedPangCnt.
  writer.WriteShort(score.pang_count);
  writer.WriteByte(10);  // Player +56: lbHelpCnt / lbRedHelpCnt.
  writer.WriteShort(score.rescue_count);
  writer.WriteByte(29);  // 0x8881B0: player +64, copied to the ordinary Tab
                         // score column by 0x7B2D90.
  writer.WriteShort(
      static_cast<std::uint16_t>(std::min(personal_score, 65535u)));
  writer.WriteUnsigned(0);
  return {986, writer.GetBytes()};
}

LambdaPacket CreateTeamScoreReply(std::uint8_t team, std::uint16_t score) {
  if (team < 1 || team > 2 || score > 200) {
    throw std::invalid_argument(
        "Team score is outside the supported ordinary room rules");
  }

  PacketWriter writer;
  writer.WriteByte(team);
  writer.WriteShort(score);
  writer.WriteByte(0xFF);
  writer.WriteByte(0xFF);
  writer.WriteUnsigned(0);
  writer.WriteUnsigned(0);
  return {986, writer.GetBytes()};
}
}  // namespace bubblefighter::protocol
