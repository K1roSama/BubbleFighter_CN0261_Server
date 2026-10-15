#include "game/health_trace_tracker.h"

#include <stdexcept>

namespace bubblefighter::game {
void HealthTraceTracker::Bind(std::uint32_t character_object) {
  if (character_object_ && *character_object_ != character_object) {
    throw std::invalid_argument(
        "Health reports must use the same complete character object as C3");
  }

  character_object_ = character_object;
}

bool HealthTraceTracker::RecordReset(std::uint32_t character_object,
                                     std::uint32_t tick) {
  Bind(character_object);
  tick &= 0xFFFFFFu;
  if (reset_tick_ == tick) {
    return false;
  }

  // This is an observed boundary, not an authorized life number or a trusted
  // freshness check.
  reset_tick_ = tick;
  ++reported_reset_;
  return true;
}

bool HealthTraceTracker::Request(HealthTraceReason reason) {
  if (end_requested_) {
    return false;
  }
  if (pending_) {
    end_queued_ = end_queued_ || reason == HealthTraceReason::kEnd;
    return false;
  }

  pending_ = HealthTraceReceipt{next_sequence_++, reason, reported_reset_,
                                reported_reset_};
  end_requested_ = reason == HealthTraceReason::kEnd;
  return true;
}

bool HealthTraceTracker::HasPending() const { return pending_.has_value(); }

HealthTraceReceipt HealthTraceTracker::Complete() {
  if (!pending_) {
    throw std::logic_error("Cannot accept an unsolicited health trace");
  }

  auto receipt = *pending_;
  receipt.received_reset = reported_reset_;
  pending_.reset();
  return receipt;
}

bool HealthTraceTracker::RequestQueuedEnd() {
  if (!end_queued_ || pending_) {
    return false;
  }

  end_queued_ = false;
  return Request(HealthTraceReason::kEnd);
}

std::uint32_t HealthTraceTracker::GetReportedReset() const {
  return reported_reset_;
}
}  // namespace bubblefighter::game
