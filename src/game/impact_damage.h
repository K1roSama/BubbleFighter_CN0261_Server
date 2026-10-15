#ifndef BUBBLEFIGHTER_SERVER_GAME_IMPACT_DAMAGE_H_
#define BUBBLEFIGHTER_SERVER_GAME_IMPACT_DAMAGE_H_

#include <optional>

#include "game/game_weapon_catalog.h"

namespace bubblefighter::game {
struct ImpactDamageContext {
  std::uint32_t room_mode = 1;
  std::optional<std::uint8_t> attack_flags;
  std::uint16_t target_flags = 0;
  std::uint8_t defense_percent = 0;
  bool critical_cancelled = false;
  float back_attack_bonus = 0.0f;
  float back_reduction = 0.0f;
  float wall_reduction = 0.0f;
  // Required only when target flag 0x10 is set. A flag alone does not prove a
  // nonempty shield.
  std::optional<bool> shield_active;
};

struct ImpactDamage {
  float health_damage = 0.0f;
  float shield_damage = 0.0f;
  bool critical_applied = false;
  bool defense_consumed = false;
  bool cancel_consumed = false;
};

// Reproduces 0x5520B0 in ordinary modes. This is before the player's
// damage-application guards. Missing actor state returns no result; a numerical
// result does not authorize an HP change.
std::optional<ImpactDamage> CalculateImpactDamage(
    const WeaponDamageProfile& weapon, const ImpactDamageContext& context);
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_IMPACT_DAMAGE_H_
