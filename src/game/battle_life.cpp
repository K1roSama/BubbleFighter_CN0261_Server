#include "game/battle_life.h"

#include <limits>
#include <stdexcept>
#include <utility>

namespace bubblefighter::game {
namespace {
// Server correlation window; this is not an original protocol constant or a
// rescue distance check.
constexpr auto kRescueWindow = std::chrono::seconds(5);
}  // namespace

BattleLife::BattleLife(NowHandler now) : now_(std::move(now)) {
  if (!now_) {
    throw std::invalid_argument("Battle life clock is required");
  }
}

void BattleLife::Bind(std::uint8_t slot, std::uint32_t object_id,
                      std::uint8_t team) {
  if (slot >= players_.size() || object_id < 1000000 ||
      (object_id - 1000000) / 100000 != slot || team < 1 || team > 2) {
    throw std::invalid_argument(
        "Battle life requires a player object and a frozen room team");
  }
  auto& player = players_[slot];
  if (player.object_id != 0 &&
      (player.object_id != object_id || player.team != team)) {
    throw std::invalid_argument(
        "Character initialization changed its object or team within the round");
  }
  player.object_id = object_id;
  player.team = team;
}

std::uint8_t BattleLife::Find(std::uint32_t object_id) const {
  if (object_id < 1000000 ||
      (object_id - 1000000) / 100000 >= players_.size()) {
    throw std::invalid_argument("Life event has an invalid player object");
  }
  const auto slot = static_cast<std::uint8_t>((object_id - 1000000) / 100000);
  if (players_[slot].object_id != object_id) {
    throw std::invalid_argument(
        "Life event object was not initialized by this round's player");
  }
  return slot;
}

bool BattleLife::CanFire(std::uint8_t slot) const {
  return slot < players_.size() && players_[slot].object_id != 0 &&
         players_[slot].state == State::kActive;
}

bool BattleLife::ReturnToNormal(std::uint8_t slot) {
  if (slot >= players_.size() || players_[slot].object_id == 0) {
    return false;
  }
  auto& player = players_[slot];
  if (player.state == State::kStunRecovering) {
    player.state = State::kActive;
  }
  return player.state == State::kActive;
}

bool BattleLife::AreOpponents(std::uint8_t source_slot,
                              std::uint32_t target_object) const {
  if (source_slot >= players_.size() || players_[source_slot].object_id == 0) {
    throw std::invalid_argument(
        "Damage source requires an initialized round member");
  }

  return players_[source_slot].team != players_[Find(target_object)].team;
}

const protocol::PlayerBattleScore& BattleLife::GetScore(
    std::uint8_t slot) const {
  if (slot >= players_.size() || players_[slot].object_id == 0) {
    throw std::invalid_argument(
        "Player score requires an initialized round member");
  }

  return players_[slot].score;
}

void BattleLife::IncrementCounter(std::uint16_t& value) {
  if (value < std::numeric_limits<std::uint16_t>::max()) {
    ++value;
  }
}

bool BattleLife::Apply(std::uint8_t source_slot,
                       const protocol::PeerLifeEvent& event,
                       bool allow_self_damage) {
  const auto victim_slot = Find(event.victim_object);
  if (event.kind == protocol::PeerLifeKind::kEndStunned) {
    ValidateReporter(source_slot, event.victim_object);
    auto& victim = players_[victim_slot];
    if (victim.state != State::kStunned) {
      return false;
    }
    // P32790 enters original state 107; P32813 separately finishes its recovery
    // animation.
    victim.state = State::kStunRecovering;
    return true;
  }
  if (event.kind == protocol::PeerLifeKind::kBubbleSettled ||
      event.kind == protocol::PeerLifeKind::kSelfPopOut) {
    ValidateReporter(source_slot, event.victim_object);
    auto& victim = players_[victim_slot];
    if (victim.state != State::kBubbled) {
      return false;
    }
    if (event.kind == protocol::PeerLifeKind::kBubbleSettled) {
      if (victim.bubble_settled) {
        return false;
      }
      // P32801 advances the remote character from animation 305 to the kickable
      // bubble state 301.
      victim.bubble_settled = true;
      return true;
    }
    if (!victim.bubble_settled || event.actor_object != 0xFFFFFFFFu) {
      return false;
    }
    // Ordinary bubble timeout is a recovery, not a death or a teammate rescue
    // award.
    victim.state = State::kActive;
    victim.bubble_settled = false;
    victim.respawn_requested_at.reset();
    victim.rescue_requests = {};
    victim.kick_requests = {};
    ClearKickAttempts(victim_slot);
    victim.completed_rescuer.reset();
    return true;
  }
  if (event.kind == protocol::PeerLifeKind::kDying ||
      event.kind == protocol::PeerLifeKind::kDead) {
    ValidateReporter(source_slot, event.victim_object);
    auto& victim = players_[victim_slot];
    if (event.kind == protocol::PeerLifeKind::kDying) {
      if (victim.state != State::kBubbled) {
        return false;
      }
      victim.state = State::kDying;
    } else {
      if (victim.dead_reported || victim.state == State::kDead ||
          victim.state == State::kRespawnOffered ||
          victim.state == State::kRespawnConfirmed) {
        return false;
      }
      // Ordinary out-of-world handling (0x445960) sends P32836 without a
      // preceding knockdown. The packet identifies only its owner, so it cannot
      // establish an attacker or award a Pang.
      victim.dead_reported = true;
      if (victim.state != State::kRespawnWaiting) {
        victim.state = State::kDead;
      }
    }
    victim.rescue_requests = {};
    victim.kick_requests = {};
    ClearKickAttempts(victim_slot);
    victim.completed_rescuer.reset();
    return true;
  }
  const auto actor_slot = Find(event.actor_object);
  const auto reporter = event.kind == protocol::PeerLifeKind::kTryRescue
                            ? actor_slot
                            : victim_slot;
  const auto self_damage = allow_self_damage &&
                           (event.kind == protocol::PeerLifeKind::kKnocked ||
                            event.kind == protocol::PeerLifeKind::kStunned) &&
                           victim_slot == actor_slot;
  if (source_slot != reporter || (victim_slot == actor_slot && !self_damage)) {
    throw std::invalid_argument(
        "Life event was not sent by its authenticated reporter");
  }
  auto& victim = players_[victim_slot];
  auto& actor = players_[actor_slot];
  if (event.kind == protocol::PeerLifeKind::kStunned) {
    if ((!self_damage && victim.team == actor.team) ||
        victim.state != State::kActive) {
      return false;
    }
    victim.state = State::kStunned;
    ClearKickAttempts(victim_slot);
    return true;
  }
  if (event.kind == protocol::PeerLifeKind::kKnocked) {
    const auto tick = event.tick & 0xFFFFFFu;
    const auto elapsed =
        victim.knock_tick ? (tick - *victim.knock_tick) & 0xFFFFFFu : 1u;
    // Only a verified frozen grenade can opt into its resource's
    // explosionHitSelf policy.
    if ((!self_damage && !AreOpponents(actor_slot, event.victim_object)) ||
        victim.state != State::kActive || elapsed == 0 ||
        elapsed >= 0x800000u) {
      return false;
    }
    victim.knock_tick = tick;
    victim.dead_reported = false;
    victim.state = event.direct_death ? State::kDying : State::kBubbled;
    victim.bubble_settled = false;
    victim.rescue_requests = {};
    victim.kick_requests = {};
    ClearKickAttempts(victim_slot);
    victim.completed_rescuer.reset();
    if (!event.direct_death && victim.team != actor.team) {
      IncrementCounter(actor.score.bubble_count);
    }
    return true;
  }
  if (victim.team != actor.team) {
    throw std::invalid_argument(
        "Rescue requires members of the same frozen team");
  }
  const auto now = now_();
  if (event.kind == protocol::PeerLifeKind::kTryRescue) {
    if (victim.state != State::kBubbled || actor.state != State::kActive) {
      return false;
    }
    // Retries are valid: the original victim ignores attempts during state
    // 305's animation.
    victim.rescue_requests[actor_slot] = now + kRescueWindow;
    return true;
  }
  if (event.kind == protocol::PeerLifeKind::kPopOut) {
    const auto request = victim.rescue_requests[actor_slot];
    if (victim.state != State::kBubbled || actor.state != State::kActive ||
        !request || *request <= now) {
      return false;
    }
    victim.state = State::kActive;
    victim.bubble_settled = false;
    victim.respawn_requested_at.reset();
    victim.rescue_requests = {};
    victim.kick_requests = {};
    ClearKickAttempts(victim_slot);
    victim.completed_rescuer = event.actor_object;
    victim.completion_deadline = now + kRescueWindow;
    return true;
  }
  if (event.kind == protocol::PeerLifeKind::kRescue && !event.special_rescue &&
      victim.state == State::kActive &&
      victim.completed_rescuer == event.actor_object &&
      victim.completion_deadline > now) {
    victim.completed_rescuer.reset();
    IncrementCounter(actor.score.rescue_count);
    return true;
  }
  return false;
}

void BattleLife::ValidateReporter(std::uint8_t slot,
                                  std::uint32_t object_id) const {
  if (Find(object_id) != slot) {
    throw std::invalid_argument(
        "Peer event does not identify the character bound to this room slot");
  }
}

void BattleLife::RequestRespawn(std::uint8_t slot, std::uint32_t object_id) {
  ValidateReporter(slot, object_id);
  auto& player = players_[slot];
  if (player.state == State::kRespawnWaiting ||
      player.state == State::kRespawnOffered ||
      player.state == State::kRespawnConfirmed) {
    return;
  }
  // Main and relay sockets can arrive in either order. A request alone never
  // proves death.
  if (!player.respawn_requested_at ||
      now_() - *player.respawn_requested_at >= kRescueWindow) {
    player.respawn_requested_at = now_();
  }
}

bool BattleLife::BeginRespawn(std::uint8_t slot) {
  if (slot >= players_.size()) {
    return false;
  }
  auto& player = players_[slot];
  if ((player.state != State::kDying && player.state != State::kDead) ||
      !player.respawn_requested_at ||
      now_() - *player.respawn_requested_at >= kRescueWindow) {
    return false;
  }
  player.state = State::kRespawnWaiting;
  return true;
}

std::optional<std::uint32_t> BattleLife::OfferRespawn(
    std::uint8_t slot, std::uint32_t spawn_index) {
  if (slot >= players_.size() ||
      players_[slot].state != State::kRespawnWaiting) {
    return std::nullopt;
  }
  auto& player = players_[slot];
  if (spawn_index / 10 != player.team || spawn_index % 10 < 1 ||
      spawn_index % 10 > 8) {
    throw std::invalid_argument(
        "Respawn offer is outside the player's team spawn group");
  }
  player.spawn_index = spawn_index;
  player.state = State::kRespawnOffered;
  return player.object_id;
}

bool BattleLife::ConfirmRespawn(std::uint8_t slot, std::uint32_t object_id,
                                std::uint32_t spawn_index) {
  ValidateReporter(slot, object_id);
  auto& player = players_[slot];
  if (player.state != State::kRespawnOffered) {
    return false;
  }
  if (spawn_index != player.spawn_index) {
    throw std::invalid_argument(
        "Respawn confirmation changed the server-selected spawn index");
  }
  player.state = State::kRespawnConfirmed;
  return true;
}

bool BattleLife::CompleteRespawn(std::uint8_t slot, std::uint32_t object_id,
                                 std::uint32_t spawn_index) {
  ValidateReporter(slot, object_id);
  auto& player = players_[slot];
  if (player.state != State::kRespawnConfirmed) {
    return false;
  }
  if (spawn_index != player.spawn_index) {
    throw std::invalid_argument(
        "Relayed respawn did not match this life's authorization");
  }
  player.state = State::kActive;
  player.bubble_settled = false;
  player.dead_reported = false;
  player.respawn_requested_at.reset();
  player.rescue_requests = {};
  player.kick_requests = {};
  ClearKickAttempts(slot);
  player.completed_rescuer.reset();
  return true;
}
}  // namespace bubblefighter::game
