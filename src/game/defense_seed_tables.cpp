#include "game/defense_seed_tables.h"

#include <openssl/rand.h>

#include <cmath>
#include <stdexcept>

namespace bubblefighter::game {
DefenseSeedTables CreateDefenseSeedTables(const DefenseProfile& profile) {
  std::array<std::uint32_t, 8> entropy{};
  if (RAND_bytes(reinterpret_cast<unsigned char*>(entropy.data()),
                 static_cast<int>(sizeof(entropy))) != 1) {
    throw std::runtime_error("Could not initialize defense seed randomness");
  }
  std::seed_seq seed(entropy.begin(), entropy.end());
  std::mt19937 random(seed);
  return CreateDefenseSeedTables(profile, random);
}

DefenseSeedTables CreateDefenseSeedTables(const DefenseProfile& profile,
                                          std::mt19937& random) {
  if (!std::isfinite(profile.minimum) || !std::isfinite(profile.maximum) ||
      !std::isfinite(profile.critical_cancel_ratio) || profile.minimum < 0.0f ||
      profile.minimum > profile.maximum || profile.maximum > 1.0f ||
      profile.critical_cancel_ratio < 0.0f ||
      profile.critical_cancel_ratio > 1.0f) {
    throw std::invalid_argument(
        "Defense seed profile has an invalid ratio or range");
  }
  // 0x591B60 multiplies stored float32 by double 100 and truncates. 0xB182E0
  // excludes the upper bound.
  const auto minimum =
      static_cast<int>(static_cast<double>(profile.minimum) * 100.0);
  const auto maximum =
      static_cast<int>(static_cast<double>(profile.maximum) * 100.0);
  std::uniform_int_distribution<int> defense(
      minimum, maximum > minimum ? maximum - 1 : minimum);
  std::bernoulli_distribution cancel(
      static_cast<double>(profile.critical_cancel_ratio));
  DefenseSeedTables tables;
  for (std::size_t index = 0; index < kDefenseSeedCount; ++index) {
    tables.defense[index] = static_cast<std::uint8_t>(defense(random));
    if (cancel(random)) {
      tables.critical_cancel[index / 32] |= 1u << (index % 32);
    }
  }
  return tables;
}
}  // namespace bubblefighter::game
