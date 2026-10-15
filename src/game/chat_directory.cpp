#include "game/chat_directory.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace bubblefighter::game {
void ChatDirectory::Join(ChatIdentity identity) {
  if (identity.session_id == 0 || identity.account_id == 0 ||
      identity.nickname.empty()) {
    throw std::invalid_argument("Chat requires an authenticated identity");
  }
  identities_.insert_or_assign(identity.session_id, std::move(identity));
}

void ChatDirectory::Leave(std::uint64_t session_id) {
  identities_.erase(session_id);
}

std::optional<ChatIdentity> ChatDirectory::Find(
    std::uint64_t session_id) const {
  const auto found = identities_.find(session_id);
  return found == identities_.end() ? std::nullopt
                                    : std::optional(found->second);
}

std::optional<ChatIdentity> ChatDirectory::FindNickname(
    std::u16string_view nickname) const {
  std::optional<ChatIdentity> result;
  for (const auto& [sessionId, identity] : identities_) {
    if (identity.nickname == nickname) {
      if (result) {
        return std::nullopt;  // A duplicated login must not cause private
                              // messages to leak to both sockets.
      }
      result = identity;
    }
  }
  return result;
}

std::vector<ChatIdentity> ChatDirectory::Channel(
    std::uint64_t session_id) const {
  const auto sender = Find(session_id);
  std::vector<ChatIdentity> result;
  if (sender) {
    for (const auto& [id, identity] : identities_) {
      if (identity.channel_type == sender->channel_type &&
          identity.channel_index == sender->channel_index) {
        result.push_back(identity);
      }
    }
  }
  return result;
}

bool ChatDirectory::Admit(std::uint64_t session_id, std::u16string_view text,
                          Clock::time_point now) {
  const auto identity = Find(session_id);
  if (!identity) {
    return false;
  }
  std::erase_if(throttles_, [now](const auto& item) {
    return now - item.second.last_activity > std::chrono::minutes(2);
  });
  auto& state = throttles_[identity->account_id];
  state.last_activity = now;
  if (now < state.blocked_until) {
    return false;
  }
  while (!state.sent.empty() &&
         now - state.sent.front() >= std::chrono::seconds(2)) {
    state.sent.pop_front();
  }
  if (state.sent.empty()) {
    state.repeated = 0;
  }
  const auto repeated = state.previous == text ? state.repeated + 1 : 1;
  if (state.sent.size() >= 5 || repeated > 3) {
    state.blocked_until = now + std::chrono::seconds(60);
    state.sent.clear();
    state.repeated = 0;
    return false;
  }
  state.sent.push_back(now);
  state.previous = text;
  state.repeated = repeated;
  return true;
}
}  // namespace bubblefighter::game
