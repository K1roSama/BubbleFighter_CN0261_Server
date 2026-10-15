#include "game/game_settlement.h"

#include <openssl/rand.h>

#include <algorithm>
#include <limits>
#include <regex>
#include <set>
#include <stdexcept>

namespace bubblefighter::game {
namespace {
using boost::mysql::field;
using database::SqlCommand;

void Validate(const CompletedGame& game) {
  if (!std::regex_match(game.id, std::regex("[0-9a-f]{32}")) ||
      game.room_id == 0 || game.tracking_id == 0 ||
      (game.mode != 1 && game.mode != 3) ||
      (game.sub_mode != 0 && game.sub_mode != 1 && game.sub_mode != 3 &&
       game.sub_mode != 4) ||
      game.map_name.empty() || game.map_name.size() > 128 ||
      game.map_name.find('\0') != std::string::npos ||
      (game.reason != 1 && game.reason != 2) || game.team_scores[0] > 200 ||
      game.team_scores[1] > 200 || game.players.size() < 2 ||
      game.players.size() > 8) {
    throw std::invalid_argument("Invalid completed ordinary game");
  }
  std::set<std::uint8_t> slots;
  std::set<std::uint32_t> character_ids;
  std::set<std::uint32_t> accounts;
  for (const auto& player : game.players) {
    if (player.slot >= 8 || !slots.insert(player.slot).second ||
        player.character.id == 0 ||
        !character_ids.insert(player.character.id).second ||
        player.character.account_id == 0 ||
        !accounts.insert(player.character.account_id).second ||
        player.team < 1 || player.team > 2 ||
        bubblefighter::characters::CheckNickname(player.character.nickname) !=
            characters::CreationResult::kSuccess) {
      throw std::invalid_argument("Invalid completed game participant");
    }
  }
}
}  // namespace

std::string CreateGameResultId() {
  std::array<unsigned char, 16> bytes{};
  if (RAND_bytes(bytes.data(), static_cast<int>(bytes.size())) != 1) {
    throw std::runtime_error("Could not generate a game result identity");
  }
  constexpr char digits[] = "0123456789abcdef";
  std::string value;
  for (const auto byte : bytes) {
    value.push_back(digits[byte >> 4]);
    value.push_back(digits[byte & 15]);
  }
  return value;
}

std::uint8_t WinningTeam(const CompletedGame& game) {
  return game.team_scores[0] == game.team_scores[1]
             ? 0
             : (game.team_scores[0] > game.team_scores[1] ? 1 : 2);
}

GameSettlementRepository::GameSettlementRepository(
    database::MySqlDatabase& database,
    const progression::ProgressionPolicy& policy)
    : database_(database), policy_(policy) {}

boost::asio::awaitable<std::vector<SettledPlayer>>
GameSettlementRepository::Settle(CompletedGame game) {
  Validate(game);
  // Every writer locks character parents in increasing order, matching
  // shop/equipment parent locks.
  std::sort(game.players.begin(), game.players.end(),
            [](const CompletedPlayer& first, const CompletedPlayer& second) {
              return first.character.id < second.character.id;
            });
  std::vector<SettledPlayer> settled;
  co_await database_
      .ExecuteTransaction([this, &game,
                           &settled](database::
                                         SqlTransaction& transaction) -> boost::
                                                                          asio::
                                                                              awaitable<
                                                                                  void> {
                                                                                // The no-op duplicate update locks the result row, serializing retries before any wallet change.
                                                                                SqlCommand query{
                                                                                    "INSERT INTO game_results (id, room_id, tracking_id, mode, sub_mode, map_name, end_reason, "
                                                                                    "first_team_score, second_team_score, winner, policy_version, completed_at) "
                                                                                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, UTC_TIMESTAMP(6)) "
                                                                                    "ON DUPLICATE KEY UPDATE id = id",
                                                                                    {field(
                                                                                         game.id),
                                                                                     field(
                                                                                         game.room_id),
                                                                                     field(
                                                                                         game.tracking_id),
                                                                                     field(
                                                                                         game.mode),
                                                                                     field(
                                                                                         game.sub_mode),
                                                                                     field(
                                                                                         game.map_name),
                                                                                     field(
                                                                                         game.reason),
                                                                                     field(
                                                                                         game.team_scores
                                                                                             [0]),
                                                                                     field(
                                                                                         game.team_scores
                                                                                             [1]),
                                                                                     field(WinningTeam(
                                                                                         game)),
                                                                                     field(
                                                                                         policy_
                                                                                             .GetVersion())}};
                                                                                co_await transaction
                                                                                    .Execute(
                                                                                        std::move(
                                                                                            query));
                                                                                query = {
                                                                                    "SELECT room_id, tracking_id, mode, sub_mode, map_name, end_reason, first_team_score, "
                                                                                    "second_team_score, winner FROM game_results WHERE id = ? FOR UPDATE",
                                                                                    {field(
                                                                                        game.id)},
                                                                                    {},
                                                                                    1};
                                                                                const auto header =
                                                                                    co_await transaction
                                                                                        .Execute(
                                                                                            std::move(
                                                                                                query));
                                                                                const auto row =
                                                                                    header
                                                                                        .rows()
                                                                                            [0];
                                                                                if (row[0].as_uint64() !=
                                                                                        game.room_id ||
                                                                                    row[1].as_uint64() !=
                                                                                        game.tracking_id ||
                                                                                    row[2].as_uint64() !=
                                                                                        game.mode ||
                                                                                    row[3].as_uint64() !=
                                                                                        game.sub_mode ||
                                                                                    row[4].as_string() !=
                                                                                        game.map_name ||
                                                                                    row[5].as_uint64() !=
                                                                                        game.reason ||
                                                                                    row[6].as_uint64() !=
                                                                                        game.team_scores
                                                                                            [0] ||
                                                                                    row[7].as_uint64() !=
                                                                                        game.team_scores
                                                                                            [1] ||
                                                                                    row[8].as_uint64() !=
                                                                                        WinningTeam(
                                                                                            game)) {
                                                                                  throw std::
                                                                                      invalid_argument(
                                                                                          "Game result identity was reused for a different outcome");
                                                                                }
                                                                                query = {
                                                                                    "SELECT slot, character_id, account_id, nickname, team, bubble_count, pang_count, rescue_count, "
                                                                                    "experience_after, lucci_after, cash, experience_gain, lucci_gain, level, level_start, "
                                                                                    "next_level, level_before, score, grade, medal "
                                                                                    "FROM game_result_players LEFT JOIN game_result_awards "
                                                                                    "ON game_result_awards.game_result_player_id = game_result_players.id "
                                                                                    "WHERE game_result_id = ? ORDER BY character_id FOR UPDATE",
                                                                                    {field(
                                                                                        game.id)}};
                                                                                const auto previous =
                                                                                    co_await transaction
                                                                                        .Execute(
                                                                                            std::move(
                                                                                                query));
                                                                                if (!previous
                                                                                         .rows()
                                                                                         .empty()) {
                                                                                  if (previous
                                                                                          .rows()
                                                                                          .size() !=
                                                                                      game.players
                                                                                          .size()) {
                                                                                    throw std::
                                                                                        invalid_argument(
                                                                                            "Game result identity was reused with a different roster");
                                                                                  }
                                                                                  for (
                                                                                      std::size_t
                                                                                          index =
                                                                                              0;
                                                                                      index <
                                                                                      game.players
                                                                                          .size();
                                                                                      ++index) {
                                                                                    const auto saved =
                                                                                        previous
                                                                                            .rows()
                                                                                                [index];
                                                                                    const auto& player =
                                                                                        game.players
                                                                                            [index];
                                                                                    if (saved[0].as_uint64() !=
                                                                                            player
                                                                                                .slot ||
                                                                                        saved[1].as_uint64() !=
                                                                                            player
                                                                                                .character
                                                                                                .id ||
                                                                                        saved[2].as_uint64() !=
                                                                                            player
                                                                                                .character
                                                                                                .account_id ||
                                                                                        saved[3].as_string() !=
                                                                                            characters::EncodeNickname(
                                                                                                player
                                                                                                    .character
                                                                                                    .nickname) ||
                                                                                        saved[4].as_uint64() !=
                                                                                            player
                                                                                                .team ||
                                                                                        saved[5].as_uint64() !=
                                                                                            player
                                                                                                .score
                                                                                                .bubble_count ||
                                                                                        saved[6].as_uint64() !=
                                                                                            player
                                                                                                .score
                                                                                                .pang_count ||
                                                                                        saved[7].as_uint64() !=
                                                                                            player
                                                                                                .score
                                                                                                .rescue_count) {
                                                                                      throw std::
                                                                                          invalid_argument(
                                                                                              "Game result identity was reused with different player data");
                                                                                    }
                                                                                    characters::CharacterStats stats{
                                                                                        static_cast<
                                                                                            std::
                                                                                                uint32_t>(
                                                                                            saved[9]
                                                                                                .as_uint64()),
                                                                                        static_cast<
                                                                                            std::
                                                                                                uint32_t>(
                                                                                            saved[10]
                                                                                                .as_uint64())};
                                                                                    stats
                                                                                        .progress = {
                                                                                        static_cast<
                                                                                            std::
                                                                                                uint32_t>(
                                                                                            saved[8]
                                                                                                .as_uint64()),
                                                                                        static_cast<
                                                                                            std::
                                                                                                uint32_t>(
                                                                                            saved[13]
                                                                                                .as_uint64()),
                                                                                        static_cast<
                                                                                            std::
                                                                                                uint32_t>(
                                                                                            saved[14]
                                                                                                .as_uint64()),
                                                                                        static_cast<
                                                                                            std::
                                                                                                uint32_t>(
                                                                                            saved[15]
                                                                                                .as_uint64())};
                                                                                    progression::
                                                                                        ResultAward
                                                                                            award;
                                                                                    if (!saved[17]
                                                                                             .is_null()) {
                                                                                      award = {
                                                                                          static_cast<
                                                                                              std::
                                                                                                  uint32_t>(
                                                                                              saved[17]
                                                                                                  .as_uint64()),
                                                                                          static_cast<
                                                                                              std::
                                                                                                  uint32_t>(
                                                                                              saved[18]
                                                                                                  .as_uint64()),
                                                                                          static_cast<
                                                                                              std::
                                                                                                  uint32_t>(
                                                                                              saved[19]
                                                                                                  .as_uint64())};
                                                                                    }
                                                                                    settled
                                                                                        .push_back(
                                                                                            {player
                                                                                                 .slot,
                                                                                             player
                                                                                                 .character
                                                                                                 .id,
                                                                                             stats,
                                                                                             static_cast<
                                                                                                 std::
                                                                                                     uint32_t>(
                                                                                                 saved[11]
                                                                                                     .as_uint64()),
                                                                                             static_cast<
                                                                                                 std::
                                                                                                     uint32_t>(
                                                                                                 saved[12]
                                                                                                     .as_uint64()),
                                                                                             saved[13]
                                                                                                     .as_uint64() >
                                                                                                 saved[16]
                                                                                                     .as_uint64(),
                                                                                             award});
                                                                                  }
                                                                                  co_return;
                                                                                }

                                                                                for (
                                                                                    const auto&
                                                                                        player :
                                                                                    game.players) {
                                                                                  query = {
                                                                                      "SELECT account_id FROM characters WHERE id = ? FOR UPDATE",
                                                                                      {field(
                                                                                          player
                                                                                              .character
                                                                                              .id)},
                                                                                      {},
                                                                                      1};
                                                                                  const auto identity =
                                                                                      co_await transaction
                                                                                          .Execute(
                                                                                              std::move(
                                                                                                  query));
                                                                                  if (identity
                                                                                          .rows()
                                                                                              [0]
                                                                                              [0]
                                                                                          .as_uint64() !=
                                                                                      player
                                                                                          .character
                                                                                          .account_id) {
                                                                                    throw std::
                                                                                        invalid_argument(
                                                                                            "Completed game character identity does not match its account");
                                                                                  }
                                                                                }
                                                                                for (
                                                                                    const auto&
                                                                                        player :
                                                                                    game.players) {
                                                                                  query = {
                                                                                      "SELECT character_stats.lucci, character_stats.cash, character_progression.experience "
                                                                                      "FROM character_stats JOIN character_progression "
                                                                                      "ON character_progression.character_id = character_stats.character_id "
                                                                                      "WHERE character_stats.character_id = ? FOR UPDATE",
                                                                                      {field(
                                                                                          player
                                                                                              .character
                                                                                              .id)},
                                                                                      {},
                                                                                      1};
                                                                                  const auto current =
                                                                                      co_await transaction
                                                                                          .Execute(
                                                                                              std::move(
                                                                                                  query));
                                                                                  const auto before =
                                                                                      current
                                                                                          .rows()
                                                                                              [0];
                                                                                  const auto lucci = static_cast<
                                                                                      std::
                                                                                          uint32_t>(
                                                                                      before[0]
                                                                                          .as_uint64());
                                                                                  const auto experience = static_cast<
                                                                                      std::
                                                                                          uint32_t>(
                                                                                      before[2]
                                                                                          .as_uint64());
                                                                                  const auto reward =
                                                                                      policy_
                                                                                          .Reward(
                                                                                              player
                                                                                                  .team,
                                                                                              WinningTeam(
                                                                                                  game));
                                                                                  const auto award =
                                                                                      policy_
                                                                                          .EvaluateResult(
                                                                                              player
                                                                                                  .score
                                                                                                  .bubble_count,
                                                                                              player
                                                                                                  .score
                                                                                                  .pang_count,
                                                                                              player
                                                                                                  .score
                                                                                                  .rescue_count);
                                                                                  const auto previous_level =
                                                                                      policy_
                                                                                          .Describe(
                                                                                              experience)
                                                                                          .level;
                                                                                  characters::CharacterStats
                                                                                      stats;
                                                                                  stats
                                                                                      .cash = static_cast<
                                                                                      std::
                                                                                          uint32_t>(
                                                                                      before[1]
                                                                                          .as_uint64());
                                                                                  stats
                                                                                      .lucci = static_cast<
                                                                                      std::
                                                                                          uint32_t>(std::min(
                                                                                      static_cast<
                                                                                          std::
                                                                                              uint64_t>(
                                                                                          lucci) +
                                                                                          reward
                                                                                              .lucci,
                                                                                      static_cast<
                                                                                          std::
                                                                                              uint64_t>(
                                                                                          std::numeric_limits<
                                                                                              std::
                                                                                                  int32_t>::
                                                                                              max())));
                                                                                  stats
                                                                                      .progress =
                                                                                      policy_
                                                                                          .Advance(
                                                                                              experience,
                                                                                              reward
                                                                                                  .experience);
                                                                                  const auto experience_gain =
                                                                                      stats
                                                                                          .progress
                                                                                          .experience -
                                                                                      experience;
                                                                                  const auto lucci_gain =
                                                                                      stats
                                                                                          .lucci -
                                                                                      lucci;
                                                                                  query = {
                                                                                      "UPDATE character_stats SET lucci = ?, updated_at = UTC_TIMESTAMP(6) WHERE character_id = ?",
                                                                                      {field(
                                                                                           stats
                                                                                               .lucci),
                                                                                       field(
                                                                                           player
                                                                                               .character
                                                                                               .id)}};
                                                                                  co_await transaction
                                                                                      .Execute(
                                                                                          std::move(
                                                                                              query));
                                                                                  query = {
                                                                                      "UPDATE character_progression SET experience = ?, updated_at = UTC_TIMESTAMP(6) WHERE "
                                                                                      "character_id = ?",
                                                                                      {field(
                                                                                           stats
                                                                                               .progress
                                                                                               .experience),
                                                                                       field(
                                                                                           player
                                                                                               .character
                                                                                               .id)}};
                                                                                  co_await transaction
                                                                                      .Execute(
                                                                                          std::move(
                                                                                              query));
                                                                                  query = {
                                                                                      "INSERT INTO game_result_players (game_result_id, slot, character_id, account_id, nickname, "
                                                                                      "team, bubble_count, pang_count, rescue_count, experience_before, experience_after, "
                                                                                      "lucci_before, lucci_after, cash, experience_gain, lucci_gain, level, level_start, "
                                                                                      "next_level, level_before) "
                                                                                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
                                                                                      {field(
                                                                                           game.id),
                                                                                       field(
                                                                                           player
                                                                                               .slot),
                                                                                       field(
                                                                                           player
                                                                                               .character
                                                                                               .id),
                                                                                       field(
                                                                                           player
                                                                                               .character
                                                                                               .account_id),
                                                                                       field(characters::EncodeNickname(
                                                                                           player
                                                                                               .character
                                                                                               .nickname)),
                                                                                       field(
                                                                                           player
                                                                                               .team),
                                                                                       field(
                                                                                           player
                                                                                               .score
                                                                                               .bubble_count),
                                                                                       field(
                                                                                           player
                                                                                               .score
                                                                                               .pang_count),
                                                                                       field(
                                                                                           player
                                                                                               .score
                                                                                               .rescue_count),
                                                                                       field(
                                                                                           experience),
                                                                                       field(
                                                                                           stats
                                                                                               .progress
                                                                                               .experience),
                                                                                       field(
                                                                                           lucci),
                                                                                       field(
                                                                                           stats
                                                                                               .lucci),
                                                                                       field(
                                                                                           stats
                                                                                               .cash),
                                                                                       field(
                                                                                           experience_gain),
                                                                                       field(
                                                                                           lucci_gain),
                                                                                       field(
                                                                                           stats
                                                                                               .progress
                                                                                               .level),
                                                                                       field(
                                                                                           stats
                                                                                               .progress
                                                                                               .level_start),
                                                                                       field(
                                                                                           stats
                                                                                               .progress
                                                                                               .next_level),
                                                                                       field(
                                                                                           previous_level)},
                                                                                      1};
                                                                                  co_await transaction
                                                                                      .Execute(
                                                                                          std::move(
                                                                                              query));
                                                                                  query = {
                                                                                      "INSERT INTO game_result_awards (game_result_player_id, score, grade, medal) "
                                                                                      "SELECT id, ?, ?, ? FROM game_result_players WHERE game_result_id = ? AND slot = ?",
                                                                                      {field(
                                                                                           award
                                                                                               .score),
                                                                                       field(
                                                                                           award
                                                                                               .grade),
                                                                                       field(
                                                                                           award
                                                                                               .medal),
                                                                                       field(
                                                                                           game.id),
                                                                                       field(
                                                                                           player
                                                                                               .slot)},
                                                                                      1};
                                                                                  co_await transaction
                                                                                      .Execute(
                                                                                          std::move(
                                                                                              query));
                                                                                  settled
                                                                                      .push_back(
                                                                                          {player
                                                                                               .slot,
                                                                                           player
                                                                                               .character
                                                                                               .id,
                                                                                           stats,
                                                                                           experience_gain,
                                                                                           lucci_gain,
                                                                                           stats.progress
                                                                                                   .level >
                                                                                               previous_level,
                                                                                           award});
                                                                                }
                                                                              });
  co_return settled;
}
}  // namespace bubblefighter::game
