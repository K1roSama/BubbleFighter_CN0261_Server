#include "game/battle_projectiles.h"

#include <stdexcept>
#include <utility>

namespace bubblefighter::game {
namespace {
// Server resource limits, not constants recovered from the original protocol.
constexpr std::size_t kMaxProjectilesPerSlot = 512;
constexpr auto kProjectileLifetime = std::chrono::seconds(30);
constexpr auto kHitReportLifetime = std::chrono::seconds(30);
constexpr std::size_t kMaxHitReports = 4096;
}  // namespace

BattleProjectiles::BattleProjectiles(NowHandler now) : now_(std::move(now)) {
  if (!now_) {
    throw std::invalid_argument("Projectile clock is required");
  }
}

void BattleProjectiles::ValidateOwner(std::uint8_t source_slot,
                                      std::uint32_t projectile_id) {
  if (source_slot >= 8 || projectile_id / 100000 != source_slot) {
    throw std::invalid_argument(
        "Projectile does not belong to the authenticated room slot");
  }
}

void BattleProjectiles::Prune() {
  const auto now = now_();
  for (auto entry = projectiles_.begin(); entry != projectiles_.end();) {
    if (entry->second.expires_at <= now) {
      --active_counts_[entry->first / 100000];
      entry = projectiles_.erase(entry);
    } else {
      ++entry;
    }
  }
  for (auto entry = hit_reports_.begin(); entry != hit_reports_.end();) {
    for (std::size_t slot = 0; slot < 8; ++slot) {
      if (entry->second.expires_at[slot] <= now) {
        entry->second.pending_slots.reset(slot);
      }
    }
    if (entry->second.pending_slots.none()) {
      entry = hit_reports_.erase(entry);
    } else {
      ++entry;
    }
  }
}

bool BattleProjectiles::Register(std::uint8_t source_slot,
                                 std::uint32_t first_id, std::uint8_t count,
                                 bool single_target, ProjectileAttack attack) {
  ValidateOwner(source_slot, first_id);
  if (count == 0 || count > 64 ||
      (first_id + count - 1) / 100000 != source_slot ||
      (single_target && count != 1) ||
      (attack.kind != ProjectileKind::kOrdinary &&
       (count != 1 || single_target))) {
    throw std::invalid_argument(
        "Projectile batch crosses its owner partition or has an invalid size");
  }
  if (next_ids_[source_slot] && first_id < *next_ids_[source_slot]) {
    throw std::invalid_argument(
        "Projectile identifier was replayed or overlaps an earlier shot");
  }

  Prune();
  if (active_counts_[source_slot] + count > kMaxProjectilesPerSlot) {
    return false;
  }
  const auto expires_at = now_() + kProjectileLifetime;
  for (std::uint32_t index = 0; index < count; ++index) {
    projectiles_.emplace(first_id + index,
                         Projectile{expires_at, {}, single_target, attack});
  }
  active_counts_[source_slot] += count;
  next_ids_[source_slot] = first_id + count;
  return true;
}

bool BattleProjectiles::Hit(std::uint8_t source_slot,
                            std::uint32_t projectile_id,
                            std::uint8_t target_slot, bool same_team) {
  ValidateOwner(source_slot, projectile_id);
  if (target_slot >= 8) {
    throw std::invalid_argument("Projectile target slot is outside the room");
  }
  Prune();
  const auto found = projectiles_.find(projectile_id);
  // 0x5ACD70 always rejects the gun owner's own actor but permits some friendly
  // bubble collisions. 0x683AD0 rejects all same-team ordinary near collisions,
  // before the single target is consumed.
  if (found == projectiles_.end()) {
    return false;
  }
  const auto kind = found->second.attack.kind;
  // E02's splash can hit its owner but not teammates; its flying shell does not
  // apply direct gun damage.
  if (kind == ProjectileKind::kGrenade ||
      (kind == ProjectileKind::kSplash
           ? (same_team && source_slot != target_slot)
           : source_slot == target_slot) ||
      found->second.hit_slots.test(target_slot) ||
      (found->second.single_target &&
       (same_team || found->second.hit_slots.any()))) {
    return false;
  }
  if (!hit_reports_.contains(projectile_id) &&
      hit_reports_.size() >= kMaxHitReports) {
    return false;
  }
  auto& reports = hit_reports_[projectile_id];
  reports.attack = found->second.attack;
  reports.pending_slots.set(target_slot);
  reports.expires_at[target_slot] = now_() + kHitReportLifetime;
  found->second.hit_slots.set(target_slot);
  return true;
}

bool BattleProjectiles::Explode(std::uint8_t source_slot,
                                std::uint32_t projectile_id,
                                std::optional<std::uint8_t> target_slot,
                                bool apply_damage, bool same_team) {
  ValidateOwner(source_slot, projectile_id);
  if (target_slot && *target_slot >= 8) {
    throw std::invalid_argument("Explosion target slot is outside the room");
  }
  Prune();
  const auto found = projectiles_.find(projectile_id);
  if (found == projectiles_.end() || found->second.single_target ||
      found->second.attack.kind != ProjectileKind::kOrdinary ||
      target_slot == source_slot) {
    return false;
  }
  if (target_slot && apply_damage &&
      !Hit(source_slot, projectile_id, *target_slot, same_team)) {
    return false;
  }
  // The original ExplodeBullet receiver always destroys the bullet. Retain any
  // victim C13 permission.
  return Remove(source_slot, projectile_id);
}

std::optional<ProjectileAttack> BattleProjectiles::ConfirmHit(
    std::uint8_t target_slot, std::uint32_t projectile_id) {
  if (target_slot >= 8 || projectile_id >= 800000) {
    throw std::invalid_argument(
        "Damage confirmation victim or projectile is outside the room");
  }
  Prune();
  const auto found = hit_reports_.find(projectile_id);
  if (found == hit_reports_.end() ||
      !found->second.pending_slots.test(target_slot)) {
    return {};
  }
  const auto attack = found->second.attack;
  found->second.pending_slots.reset(target_slot);
  if (found->second.pending_slots.none()) {
    hit_reports_.erase(found);
  }
  return attack;
}

bool BattleProjectiles::Detonate(std::uint8_t source_slot,
                                 std::uint32_t projectile_id,
                                 std::uint32_t splash_id) {
  ValidateOwner(source_slot, projectile_id);
  ValidateOwner(source_slot, splash_id);
  Prune();
  const auto found = projectiles_.find(projectile_id);
  if (found == projectiles_.end() ||
      found->second.attack.kind != ProjectileKind::kGrenade) {
    return false;
  }
  auto attack = found->second.attack;
  attack.kind = ProjectileKind::kSplash;
  // Splash distance and stun effects require a separate calculation; never
  // apply the ordinary gun formula.
  attack.flags.reset();
  if (!Register(source_slot, splash_id, 1, false, attack)) {
    return false;
  }
  return Remove(source_slot, projectile_id);
}

void BattleProjectiles::DiscardHitReports(std::uint8_t target_slot) {
  if (target_slot >= 8) {
    throw std::invalid_argument(
        "Damage confirmation victim is outside the room");
  }
  for (auto& [id, reports] : hit_reports_) {
    static_cast<void>(id);
    reports.pending_slots.reset(target_slot);
  }
  Prune();
}

bool BattleProjectiles::Remove(std::uint8_t source_slot,
                               std::uint32_t projectile_id) {
  ValidateOwner(source_slot, projectile_id);
  Prune();
  if (projectiles_.erase(projectile_id) == 0) {
    return false;
  }
  --active_counts_[source_slot];
  return true;
}
}  // namespace bubblefighter::game
