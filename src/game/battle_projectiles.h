#ifndef BUBBLEFIGHTER_SERVER_GAME_BATTLE_PROJECTILES_H_
#define BUBBLEFIGHTER_SERVER_GAME_BATTLE_PROJECTILES_H_

#include <array>
#include <bitset>
#include <chrono>
#include <cstdint>
#include <functional>
#include <map>
#include <optional>

namespace bubblefighter::game {
enum class ProjectileKind { kOrdinary, kGrenade, kSplash };

struct ProjectileAttack {
  std::uint32_t weapon_definition = 0;
  // Ordinary gun flags are on the wire; the original P32773 Near constructor
  // supplies zero.
  std::optional<std::uint8_t> flags;
  ProjectileKind kind = ProjectileKind::kOrdinary;
};

// One instance per round. It validates ownership and replay, not physical
// collision or HP.
class BattleProjectiles {
 public:
  using Clock = std::chrono::steady_clock;
  using NowHandler = std::function<Clock::time_point()>;

  explicit BattleProjectiles(NowHandler now = Clock::now);
  bool Register(std::uint8_t source_slot, std::uint32_t first_id,
                std::uint8_t count, bool single_target = false,
                ProjectileAttack attack = {});
  bool Hit(std::uint8_t source_slot, std::uint32_t projectile_id,
           std::uint8_t target_slot, bool same_team = false);
  bool Explode(std::uint8_t source_slot, std::uint32_t projectile_id,
               std::optional<std::uint8_t> target_slot, bool apply_damage,
               bool same_team = false);
  bool Remove(std::uint8_t source_slot, std::uint32_t projectile_id);
  bool Detonate(std::uint8_t source_slot, std::uint32_t projectile_id,
                std::uint32_t splash_id);
  std::optional<ProjectileAttack> ConfirmHit(std::uint8_t target_slot,
                                             std::uint32_t projectile_id);
  void DiscardHitReports(std::uint8_t target_slot);

 private:
  struct Projectile {
    Clock::time_point expires_at;
    std::bitset<8> hit_slots;
    bool single_target = false;
    ProjectileAttack attack;
  };

  struct HitReports {
    ProjectileAttack attack;
    std::bitset<8> pending_slots;
    std::array<Clock::time_point, 8> expires_at{};
  };

  void Prune();
  static void ValidateOwner(std::uint8_t source_slot,
                            std::uint32_t projectile_id);

  NowHandler now_;
  std::array<std::optional<std::uint32_t>, 8> next_ids_{};
  std::array<std::size_t, 8> active_counts_{};
  std::map<std::uint32_t, Projectile> projectiles_;
  // Deletion can reach the relay before the victim's C13 on another TCP
  // connection.
  std::map<std::uint32_t, HitReports> hit_reports_;
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_BATTLE_PROJECTILES_H_
