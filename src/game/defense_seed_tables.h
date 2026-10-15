#ifndef BUBBLEFIGHTER_SERVER_GAME_DEFENSE_SEED_TABLES_H_
#define BUBBLEFIGHTER_SERVER_GAME_DEFENSE_SEED_TABLES_H_

#include <array>
#include <random>

#include "game/game_defense_catalog.h"

namespace bubblefighter::game {
inline constexpr std::size_t kDefenseSeedCount = 1000;
inline constexpr std::size_t kCancelSeedWords = 32;

struct DefenseSeedTables {
  std::array<std::uint8_t, kDefenseSeedCount> defense{};
  std::array<std::uint32_t, kCancelSeedWords> critical_cancel{};
};

DefenseSeedTables CreateDefenseSeedTables(const DefenseProfile& profile);
DefenseSeedTables CreateDefenseSeedTables(const DefenseProfile& profile,
                                          std::mt19937& random);
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_DEFENSE_SEED_TABLES_H_
