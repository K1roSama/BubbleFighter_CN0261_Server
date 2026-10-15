#ifndef BUBBLEFIGHTER_SERVER_GAME_DAMAGE_ATTRIBUTES_H_
#define BUBBLEFIGHTER_SERVER_GAME_DAMAGE_ATTRIBUTES_H_

#include <array>
#include <chrono>
#include <functional>
#include <optional>

#include "protocol/damage_attribute_messages.h"

namespace bubblefighter::game {
// Per-round client reports. Their presence alone does not authorize protection
// or damage.
class DamageAttributes {
 public:
  using Clock = std::chrono::steady_clock;
  using NowHandler = std::function<Clock::time_point()>;

  explicit DamageAttributes(NowHandler now = Clock::now);
  void Bind(std::uint32_t character_object);
  bool Apply(const protocol::DamageAttributeReport& report);
  void BeginPlay();
  void ClearForRespawn();
  // Receipt-time estimate of current reported states, not a historical or
  // authoritative hit decision.
  std::uint16_t GetFlags() const;

 private:
  struct Entry {
    std::uint32_t tick = 0;
    std::uint16_t value = 0;
    Clock::time_point expires_at{};
    bool active = true;
    bool has_tick = true;
  };

  std::optional<std::uint32_t> character_object_;
  bool bound_ = false;
  NowHandler now_;
  std::array<std::optional<Entry>, 12> entries_{};
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_DAMAGE_ATTRIBUTES_H_
