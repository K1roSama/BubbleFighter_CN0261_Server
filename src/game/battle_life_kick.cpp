#include <stdexcept>

#include "game/battle_life.h"

namespace bubblefighter::game {
namespace {
// Correlation window for authenticated contact and victim response, not a
// collision distance or animation duration.
constexpr auto kKickWindow = std::chrono::seconds(5);
}  // namespace

void BattleLife::ClearKickAttempts(std::uint8_t actor_slot) {
  players_[actor_slot].kick_target.reset();
  players_[actor_slot].kick_style = 0;
  players_[actor_slot].humiliation_target.reset();
  for (auto& player : players_) {
    player.kick_requests[actor_slot].reset();
    if (player.humiliation_target == players_[actor_slot].object_id) {
      player.humiliation_target.reset();
    }
  }
}

bool BattleLife::ApplyKick(std::uint8_t source_slot,
                           const protocol::PeerKickEvent& event) {
  if (event.kind == protocol::PeerKickKind::kHumiliationStart ||
      event.kind == protocol::PeerKickKind::kHumiliationEnd) {
    ValidateReporter(source_slot, event.actor_object);
    auto& actor = players_[source_slot];
    const auto victim_slot = Find(event.victim_object);
    const auto& victim = players_[victim_slot];
    if (actor.team == victim.team || actor.state != State::kActive ||
        !victim.dead_reported ||
        (victim.state != State::kDead &&
         victim.state != State::kRespawnWaiting)) {
      return false;
    }
    if (event.kind == protocol::PeerKickKind::kHumiliationStart) {
      if (actor.humiliation_target && actor.humiliation_deadline > now_()) {
        return false;
      }
      actor.humiliation_target = event.victim_object;
      actor.humiliation_deadline = now_() + kKickWindow;
      return true;
    }
    if (actor.humiliation_target != event.victim_object ||
        actor.humiliation_deadline <= now_()) {
      return false;
    }
    actor.humiliation_target.reset();
    return true;
  }
  if (event.kind == protocol::PeerKickKind::kGotKicked) {
    ValidateReporter(source_slot, event.victim_object);
    auto& victim = players_[source_slot];
    if (victim.state != State::kKicked) {
      return false;
    }

    victim.state = State::kDying;
    return true;
  }
  if (event.kind == protocol::PeerKickKind::kEnd ||
      event.kind == protocol::PeerKickKind::kFailed) {
    ValidateReporter(source_slot, event.actor_object);
    auto& actor = players_[source_slot];
    if (actor.state != State::kActive || !actor.kick_target) {
      return false;
    }

    ClearKickAttempts(source_slot);
    return true;
  }

  const auto victim_slot = Find(event.victim_object);
  const auto actor_slot = Find(event.actor_object);
  const auto actor_reports = event.kind == protocol::PeerKickKind::kStart ||
                             event.kind == protocol::PeerKickKind::kTryKick;
  if (source_slot != (actor_reports ? actor_slot : victim_slot) ||
      actor_slot == victim_slot) {
    throw std::invalid_argument(
        "Kick event was not sent by its authenticated reporter");
  }

  auto& victim = players_[victim_slot];
  auto& actor = players_[actor_slot];
  if (victim.team == actor.team) {
    throw std::invalid_argument("Ordinary kick requires opposing frozen teams");
  }
  if (victim.state != State::kBubbled || actor.state != State::kActive) {
    return false;
  }

  if (event.kind == protocol::PeerKickKind::kStart) {
    if (actor.kick_target == event.victim_object &&
        actor.kick_style == event.style) {
      return false;
    }

    actor.kick_target = event.victim_object;
    actor.kick_style = event.style;
    return true;
  }
  if (event.kind == protocol::PeerKickKind::kTryKick) {
    // Contact retries are needed while the victim is still in animation 305.
    // Automatic contact can emit TryKicked without a preceding KickStart.
    victim.kick_requests[actor_slot] = now_() + kKickWindow;
    actor.kick_target = event.victim_object;
    return true;
  }

  const auto request = victim.kick_requests[actor_slot];
  if (!request || *request <= now_() || !event.succeeded ||
      event.special_kick) {
    return false;
  }

  if (event.kind == protocol::PeerKickKind::kProtected) {
    // Original KickedNoPang applies a kick/bounce animation but keeps the
    // victim's bubble state.
    victim.kick_requests[actor_slot].reset();
    actor.kick_target = event.victim_object;
    return true;
  }
  if (event.kind == protocol::PeerKickKind::kConfirmed) {
    victim.state = State::kKicked;
    victim.kick_requests = {};
    victim.rescue_requests = {};
    victim.completed_rescuer.reset();
    actor.kick_target = event.victim_object;
    IncrementCounter(actor.score.pang_count);
    return true;
  }

  return false;
}
}  // namespace bubblefighter::game
