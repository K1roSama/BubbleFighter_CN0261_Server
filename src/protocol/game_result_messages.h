#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_GAME_RESULT_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_GAME_RESULT_MESSAGES_H_

#include <array>
#include <span>
#include <string>

#include "features/progression/progression_policy.h"
#include "protocol/score_messages.h"

namespace bubblefighter::protocol {
struct GameResultPlayer {
  std::uint8_t slot = 0;
  std::uint32_t account_id = 0;
  std::u16string nickname;
  std::uint8_t team = 0;
  PlayerBattleScore score;
  progression::CharacterProgress progress;
  std::uint32_t experience_gain = 0;
  std::uint32_t lucci_gain = 0;
  bool level_up = false;
  progression::ResultAward award;
};

// Ordinary team results. Rewards describe a committed settlement, never a
// client request.
LambdaPacket CreateGameResultReply(
    const std::array<std::uint16_t, 2>& team_scores,
    std::span<const GameResultPlayer> players);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_GAME_RESULT_MESSAGES_H_
