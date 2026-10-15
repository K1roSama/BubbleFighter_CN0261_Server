#ifndef BUBBLEFIGHTER_SERVER_GAME_PLAYER_DAMAGE_H_
#define BUBBLEFIGHTER_SERVER_GAME_PLAYER_DAMAGE_H_

#include <cstdint>

namespace bubblefighter::game {
struct PlayerDamageContext {
  std::int32_t character_state = 0;
  float current_health = 0.0f;
  std::uint16_t target_flags = 0;
  bool can_damage_stunned = false;
};

struct PlayerDamage {
  bool allowed = false;
  bool depleted = false;
  float health_after_impact = 0.0f;
  float remaining_health = 0.0f;
};

// 0x593540's flag checks only. Reported flags do not establish a trusted
// protection state.
bool HasPlayerDamageProtection(std::uint16_t target_flags,
                               bool can_damage_stunned);

// 0x593540's player guard and 0x575090's numeric HP transition, assuming the HP
// setter succeeds. Team checks, actor-state provenance, skills and death side
// effects belong to the application layer.
PlayerDamage CalculatePlayerDamage(float damage,
                                   const PlayerDamageContext& context);
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_PLAYER_DAMAGE_H_
