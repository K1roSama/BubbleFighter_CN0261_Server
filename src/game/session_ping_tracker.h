#ifndef BUBBLEFIGHTER_SERVER_GAME_SESSION_PING_TRACKER_H_
#define BUBBLEFIGHTER_SERVER_GAME_SESSION_PING_TRACKER_H_

#include <chrono>
#include <cstdint>
#include <map>
#include <optional>
#include <span>
#include <vector>

namespace bubblefighter::game {
// One tracker per room/plaza. Telemetry never selects hosts or changes game
// rules.
class SessionPingTracker {
 public:
  using Clock = std::chrono::steady_clock;

  bool Begin(std::uint64_t requester, std::uint32_t slot,
             std::span<const std::uint64_t> members,
             Clock::time_point now = Clock::now());
  std::optional<std::uint64_t> Complete(std::uint64_t responder,
                                        std::uint32_t responder_slot,
                                        std::uint32_t requester_slot,
                                        std::span<const std::uint64_t> members,
                                        Clock::time_point now = Clock::now());
  void Remove(std::uint64_t session_id);
  void Clear();

 private:
  struct Probe {
    Clock::time_point expires_at;
    Clock::time_point next_request_at;
    std::uint32_t requester_slot;
    std::vector<std::uint64_t> expected_responders;
  };

  std::map<std::uint64_t, Probe> probes_;
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_SESSION_PING_TRACKER_H_
