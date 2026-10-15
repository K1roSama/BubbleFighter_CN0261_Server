#ifndef BUBBLEFIGHTER_SERVER_GAME_BATTLE_LIFE_H_
#define BUBBLEFIGHTER_SERVER_GAME_BATTLE_LIFE_H_

#include <array>
#include <chrono>
#include <functional>
#include <optional>

#include "protocol/peer_kick_messages.h"
#include "protocol/peer_life_messages.h"
#include "protocol/score_messages.h"

namespace bubblefighter::game {
// Tracks victim-reported ordinary knockdown/rescue, not authoritative damage or
// collision.
class BattleLife {
 public:
  using Clock = std::chrono::steady_clock;
  using NowHandler = std::function<Clock::time_point()>;

  explicit BattleLife(NowHandler now = Clock::now);
  void Bind(std::uint8_t slot, std::uint32_t object_id, std::uint8_t team);
  bool Apply(std::uint8_t source_slot, const protocol::PeerLifeEvent& event,
             bool allow_self_damage = false);
  bool ApplyKick(std::uint8_t source_slot,
                 const protocol::PeerKickEvent& event);
  bool CanFire(std::uint8_t slot) const;
  bool ReturnToNormal(std::uint8_t slot);
  // Frozen team relation only; in-flight attacks may outlive their owner's
  // active state.
  bool AreOpponents(std::uint8_t source_slot,
                    std::uint32_t target_object) const;
  const protocol::PlayerBattleScore& GetScore(std::uint8_t slot) const;
  void ValidateReporter(std::uint8_t slot, std::uint32_t object_id) const;
  void RequestRespawn(std::uint8_t slot, std::uint32_t object_id);
  bool BeginRespawn(std::uint8_t slot);
  std::optional<std::uint32_t> OfferRespawn(std::uint8_t slot,
                                            std::uint32_t spawn_index);
  bool ConfirmRespawn(std::uint8_t slot, std::uint32_t object_id,
                      std::uint32_t spawn_index);
  bool CompleteRespawn(std::uint8_t slot, std::uint32_t object_id,
                       std::uint32_t spawn_index);

 private:
  enum class State {
    kActive,
    kStunned,
    kStunRecovering,
    kBubbled,
    kKicked,
    kDying,
    kDead,
    kRespawnWaiting,
    kRespawnOffered,
    kRespawnConfirmed
  };

  struct Player {
    std::uint32_t object_id = 0;
    std::uint8_t team = 0;
    State state = State::kActive;
    bool bubble_settled = false;
    std::optional<std::uint32_t> knock_tick;
    std::array<std::optional<Clock::time_point>, 8> rescue_requests{};
    std::array<std::optional<Clock::time_point>, 8> kick_requests{};
    std::optional<std::uint32_t> kick_target;
    std::uint32_t kick_style = 0;
    std::optional<std::uint32_t> humiliation_target;
    Clock::time_point humiliation_deadline{};
    std::optional<std::uint32_t> completed_rescuer;
    Clock::time_point completion_deadline{};
    bool dead_reported = false;
    std::optional<Clock::time_point> respawn_requested_at;
    std::uint32_t spawn_index = 0;
    protocol::PlayerBattleScore score;
  };

  std::uint8_t Find(std::uint32_t object_id) const;
  void ClearKickAttempts(std::uint8_t actor_slot);
  static void IncrementCounter(std::uint16_t& value);

  NowHandler now_;
  std::array<Player, 8> players_{};
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_BATTLE_LIFE_H_
