#include "game/session_ping_tracker.h"

#include <algorithm>

namespace bubblefighter::game {
bool SessionPingTracker::Begin(std::uint64_t requester, std::uint32_t slot,
                               std::span<const std::uint64_t> members,
                               Clock::time_point now) {
  if (requester == 0 || members.size() > 32 || slot >= members.size() ||
      members[slot] != requester) {
    return false;
  }
  std::erase_if(probes_, [now](const auto& entry) {
    return entry.second.expires_at <= now;
  });
  const auto previous = probes_.find(requester);
  if (previous != probes_.end() && previous->second.next_request_at > now) {
    return false;
  }

  // Server limits; the original protocol carries no sequence or nonce.
  probes_[requester] = {now + std::chrono::seconds(5),
                        now + std::chrono::milliseconds(500),
                        slot,
                        {members.begin(), members.end()}};
  return true;
}

std::optional<std::uint64_t> SessionPingTracker::Complete(
    std::uint64_t responder, std::uint32_t responder_slot,
    std::uint32_t requester_slot, std::span<const std::uint64_t> members,
    Clock::time_point now) {
  if (responder == 0 || members.size() > 32 ||
      responder_slot >= members.size() || requester_slot >= members.size() ||
      members[responder_slot] != responder || members[requester_slot] == 0) {
    return {};
  }
  const auto requester = members[requester_slot];
  const auto found = probes_.find(requester);
  if (found == probes_.end()) {
    return {};
  }
  auto& probe = found->second;
  if (probe.expires_at <= now) {
    probes_.erase(found);
    return {};
  }
  if (probe.requester_slot != requester_slot ||
      responder_slot >= probe.expected_responders.size() ||
      probe.expected_responders[responder_slot] != responder) {
    return {};
  }

  probe.expected_responders[responder_slot] = 0;
  return requester;
}

void SessionPingTracker::Remove(std::uint64_t session_id) {
  probes_.erase(session_id);
  for (auto& [requester, probe] : probes_) {
    static_cast<void>(requester);
    std::replace(probe.expected_responders.begin(),
                 probe.expected_responders.end(), session_id, std::uint64_t{0});
  }
}

void SessionPingTracker::Clear() { probes_.clear(); }
}  // namespace bubblefighter::game
