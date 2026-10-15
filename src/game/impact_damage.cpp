#include "game/impact_damage.h"

#include <cmath>
#include <initializer_list>
#include <stdexcept>

namespace bubblefighter::game {
namespace {
// Verified Config/ItemSpec4.xml, ItemSpecDefault / ItemSpecItem, IDs 112 and
// 104. Modes 1 / 3 load the same values through 0x6920E0 and 0x693D00; see
// ImpactDamageEvidence.json.
constexpr float kGiantAttackMultiplier = 2.0f;
constexpr float kGiantDefenseMultiplier = 0.1f;
constexpr float kFeverDefenseMultiplier = 0.8f;

bool HasFlag(std::uint16_t flags, std::uint16_t flag) {
  return (flags & flag) != 0;
}

float Multiply(float value, float multiplier) {
  // The x86 path stores each product to float, without combining subsequent
  // factors.
  return static_cast<float>(static_cast<double>(value) * multiplier);
}

float Reduce(float value, float reduction) {
  // 0x5527E4 and 0x55296E keep (1 - reduction) in the FPU until after
  // multiplication.
  return static_cast<float>((1.0 - static_cast<double>(reduction)) * value);
}

void Validate(const WeaponDamageProfile& weapon,
              const ImpactDamageContext& context) {
  for (const auto value :
       {weapon.base_damage, weapon.critical_multiplier, weapon.back_multiplier,
        weapon.wall_attack_multiplier, weapon.wall_defense_multiplier,
        context.back_attack_bonus, context.back_reduction,
        context.wall_reduction}) {
    if (!std::isfinite(value) || value < 0.0f) {
      throw std::invalid_argument(
          "Impact damage requires finite nonnegative attributes");
    }
  }
  if (!std::isfinite(weapon.unscoped_multiplier) ||
      context.defense_percent > 100 || context.back_reduction > 1.0f ||
      context.wall_reduction > 1.0f) {
    throw std::invalid_argument(
        "Impact damage has an invalid multiplier or defense percentage");
  }
}
}  // namespace

std::optional<ImpactDamage> CalculateImpactDamage(
    const WeaponDamageProfile& weapon, const ImpactDamageContext& context) {
  Validate(weapon, context);
  if ((context.room_mode != 1 && context.room_mode != 3) ||
      !context.attack_flags ||
      (HasFlag(context.target_flags, 0x10) && !context.shield_active)) {
    return std::nullopt;
  }

  const auto attack = *context.attack_flags;
  const auto target = context.target_flags;
  auto damage = weapon.base_damage;
  if (HasFlag(attack, 8)) {
    damage = Multiply(damage, kGiantAttackMultiplier);
  }
  if (HasFlag(attack, 2)) {
    damage = Multiply(damage, weapon.wall_attack_multiplier);
  }
  if (weapon.uses_scope && !HasFlag(attack, 4)) {
    damage = Multiply(damage, weapon.unscoped_multiplier < 0.0f
                                  ? 1.0f
                                  : weapon.unscoped_multiplier);
  }
  if (HasFlag(target, 8)) {
    const float multiplier = weapon.back_multiplier + context.back_attack_bonus;
    damage = Multiply(damage, multiplier);
  }
  if (HasFlag(target, 1)) {
    damage = Multiply(damage, weapon.wall_defense_multiplier);
  }

  ImpactDamage result;
  result.critical_applied = HasFlag(attack, 1);
  if (HasFlag(target, 0x10) && *context.shield_active) {
    // The shield takes the full pre-defense impact. Excess damage does not pass
    // through this branch.
    result.shield_damage = result.critical_applied
                               ? Multiply(damage, weapon.critical_multiplier)
                               : damage;
  } else {
    result.cancel_consumed = result.critical_applied;
    result.critical_applied =
        result.critical_applied && !context.critical_cancelled;
    result.defense_consumed = true;
    // 0x591B60 returns a stored float32 after multiplying the byte by double
    // 0.01.
    const auto sampled_reduction =
        static_cast<float>(static_cast<double>(context.defense_percent) * 0.01);
    const float reduction =
        sampled_reduction +
        (HasFlag(target, 8) ? context.back_reduction : 0.0f);
    if (reduction > 1.0f) {
      throw std::invalid_argument(
          "Combined impact defense exceeds the supported reduction range");
    }

    damage = Reduce(damage, reduction);
    if (HasFlag(target, 2)) {
      damage = Multiply(damage, kGiantDefenseMultiplier);
    }
    if (HasFlag(target, 4)) {
      damage = Multiply(damage, kFeverDefenseMultiplier);
    }
    if (HasFlag(target, 1)) {
      damage = Reduce(damage, context.wall_reduction);
    }
    result.health_damage = result.critical_applied
                               ? Multiply(damage, weapon.critical_multiplier)
                               : damage;
  }
  if (!std::isfinite(result.health_damage) ||
      !std::isfinite(result.shield_damage)) {
    throw std::invalid_argument("Combined impact damage is not finite");
  }
  return result;
}
}  // namespace bubblefighter::game
