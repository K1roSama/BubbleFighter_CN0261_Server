#ifndef BUBBLEFIGHTER_SERVER_GAME_GAME_WEAPON_CATALOG_H_
#define BUBBLEFIGHTER_SERVER_GAME_GAME_WEAPON_CATALOG_H_

#include <cstdint>
#include <filesystem>
#include <map>
#include <string_view>

namespace bubblefighter::game {
struct WeaponDamageProfile {
  float base_damage = 0.0f;
  float critical_multiplier = 1.0f;
  float back_multiplier = 1.0f;
  float unscoped_multiplier = 1.0f;
  float wall_attack_multiplier = 1.0f;
  float wall_defense_multiplier = 1.0f;
  bool uses_scope = false;
  bool can_damage_stunned = false;
};

struct GameWeapon {
  std::uint32_t definition_id = 0;
  std::uint8_t weapon_slot = 0;
  // Zero means this weapon does not use the ordinary gppFire messages.
  std::uint8_t projectile_count = 0;
  WeaponDamageProfile damage;
  bool grenade = false;
};

class GameWeaponCatalog {
 public:
  static GameWeaponCatalog Load(const std::filesystem::path& path);
  static GameWeaponCatalog Parse(std::string_view text);
  const GameWeapon* Find(std::uint32_t definition_id) const;

 private:
  GameWeaponCatalog() = default;
  std::map<std::uint32_t, GameWeapon> weapons_;
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_GAME_WEAPON_CATALOG_H_
