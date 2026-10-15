#include "game/player_damage.h"

#include <cmath>
#include <limits>
#include <stdexcept>

namespace bubblefighter::game {
namespace {
// 0xD87330 and 0xD86FE0 initialize the two security-wrapped HP scale values to
// 100.
constexpr double kHealthScale = 100.0;

bool HasPositiveHealth(float health) {
  const double scaled = static_cast<double>(health) * kHealthScale;
  // Avoid undefined integer conversion for unsupported input. Ordinary CN0261
  // HP is far below this bound.
  if (!std::isfinite(scaled) ||
      scaled < std::numeric_limits<std::int32_t>::min() ||
      scaled > std::numeric_limits<std::int32_t>::max()) {
    throw std::invalid_argument(
        "Player health exceeds the original signed conversion range");
  }

  return static_cast<std::int32_t>(scaled) > 0;
}
}  // namespace

bool HasPlayerDamageProtection(std::uint16_t target_flags,
                               bool can_damage_stunned) {
  return (target_flags & 0x17E0u) != 0 ||
         ((target_flags & 0x0800u) != 0 && !can_damage_stunned);
}

PlayerDamage CalculatePlayerDamage(float damage,
                                   const PlayerDamageContext& context) {
  if (!std::isfinite(damage) || damage < 0.0f) {
    throw std::invalid_argument("Player damage must be finite and nonnegative");
  }

  const auto alive = HasPositiveHealth(context.current_health);
  PlayerDamage result{false, false, context.current_health,
                      context.current_health};
  const auto category = context.character_state / 100;
  if ((category >= 3 && category <= 5) || !alive ||
      HasPlayerDamageProtection(context.target_flags,
                                context.can_damage_stunned)) {
    return result;
  }

  result.allowed = true;
  result.health_after_impact =
      static_cast<float>(static_cast<double>(context.current_health) - damage);
  result.depleted = !HasPositiveHealth(result.health_after_impact);
  result.remaining_health = result.depleted ? 0.0f : result.health_after_impact;
  return result;
}
}  // namespace bubblefighter::game
