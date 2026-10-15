#ifndef BUBBLEFIGHTER_SERVER_GAME_HEALTH_TRACE_TRACKER_H_
#define BUBBLEFIGHTER_SERVER_GAME_HEALTH_TRACE_TRACKER_H_

#include <cstdint>
#include <optional>

namespace bubblefighter::game {
enum class HealthTraceReason { kRespawn, kEnd };

struct HealthTraceReceipt {
  std::uint32_t sequence = 0;
  HealthTraceReason reason = HealthTraceReason::kRespawn;
  std::uint32_t requested_reset = 0;
  std::uint32_t received_reset = 0;
};

// Bounds diagnostics to one outstanding request and one deferred end request.
// It never controls HP or respawn.
class HealthTraceTracker {
 public:
  void Bind(std::uint32_t character_object);
  bool RecordReset(std::uint32_t character_object, std::uint32_t tick);
  bool Request(HealthTraceReason reason);
  bool HasPending() const;
  HealthTraceReceipt Complete();
  bool RequestQueuedEnd();
  std::uint32_t GetReportedReset() const;

 private:
  std::optional<std::uint32_t> character_object_;
  std::optional<std::uint32_t> reset_tick_;
  std::optional<HealthTraceReceipt> pending_;
  std::uint32_t reported_reset_ = 0;
  std::uint32_t next_sequence_ = 1;
  bool end_queued_ = false;
  bool end_requested_ = false;
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_HEALTH_TRACE_TRACKER_H_
