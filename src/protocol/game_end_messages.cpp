#include "protocol/game_end_messages.h"

#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
LambdaPacket CreateGameEndReply(bool time_expired) {
  PacketWriter writer;
  // 0x538B70 reads +4 then +8. 0x8875C0 stores the SECOND field as the
  // end-stage reason (+912).
  writer.WriteUnsigned(
      0);  // The first field is unused by the supported ordinary handler.
  writer.WriteUnsigned(
      time_expired
          ? 2u
          : 1u);  // 0x5ECA20 selects time-over.png for 2, game_over.png for 1.
  return {705, writer.GetBytes()};
}

LambdaPacket CreateGameWinnerReply(std::uint16_t first_team_score,
                                   std::uint16_t second_team_score) {
  if (first_team_score > 200 || second_team_score > 200) {
    throw std::invalid_argument(
        "Final team score exceeds the supported room score limit");
  }

  // 0x888470 stores these bytes in the final HUD team counters; winner uses
  // ScoreData +80 separately.
  return {732,
          {static_cast<std::uint8_t>(first_team_score),
           static_cast<std::uint8_t>(second_team_score)}};
}
}  // namespace bubblefighter::protocol
