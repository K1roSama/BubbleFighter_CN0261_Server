#ifndef BUBBLEFIGHTER_SERVER_GAME_GAME_SETTLEMENT_H_
#define BUBBLEFIGHTER_SERVER_GAME_GAME_SETTLEMENT_H_

#include <array>
#include <string>
#include <vector>

#include "features/characters/character_repository.h"
#include "protocol/score_messages.h"

namespace bubblefighter::game {
struct CompletedPlayer {
  std::uint8_t slot = 0;
  characters::CharacterProfile character;
  std::uint8_t team = 0;
  protocol::PlayerBattleScore score;
};

struct CompletedGame {
  std::string id;
  std::uint16_t room_id = 0;
  std::uint32_t tracking_id = 0;
  std::uint32_t mode = 0;
  std::uint32_t sub_mode = 0;
  std::string map_name;
  std::uint8_t reason = 0;
  std::array<std::uint16_t, 2> team_scores{};
  std::vector<CompletedPlayer> players;
};

struct SettledPlayer {
  std::uint8_t slot = 0;
  std::uint32_t character_id = 0;
  characters::CharacterStats stats;
  std::uint32_t experience_gain = 0;
  std::uint32_t lucci_gain = 0;
  bool level_up = false;
  progression::ResultAward award;
};

std::string CreateGameResultId();
std::uint8_t WinningTeam(const CompletedGame& game);

class GameSettlementRepository {
 public:
  GameSettlementRepository(database::MySqlDatabase& database,
                           const progression::ProgressionPolicy& policy);
  boost::asio::awaitable<std::vector<SettledPlayer>> Settle(CompletedGame game);

 private:
  database::MySqlDatabase& database_;
  const progression::ProgressionPolicy& policy_;
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_GAME_SETTLEMENT_H_
