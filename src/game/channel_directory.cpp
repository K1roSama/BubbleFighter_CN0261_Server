#include "game/channel_directory.h"

#include <algorithm>
#include <utility>

namespace bubblefighter::game {
bool ChannelDirectory::Contains(std::uint32_t channel_type,
                                std::uint32_t channel_index) {
  // Type 60 is selected by the original lobby toolbar and accepted without rank
  // restrictions.
  return channel_type == 60 && channel_index < 2;
}

bool ChannelDirectory::Prepare(ChannelTransfer transfer,
                               std::string peer_address,
                               Clock::time_point now) {
  Expire(now);
  if (transfer.account.id == 0 || transfer.character.id == 0 ||
      transfer.character.account_id != transfer.account.id ||
      transfer.source_session_id == 0 || peer_address.empty() ||
      !Contains(transfer.channel_type, transfer.channel_index) ||
      pending_.contains(transfer.account.id)) {
    return false;
  }

  const auto account_id = transfer.account.id;
  pending_.emplace(account_id,
                   PendingTransfer{std::move(transfer), std::move(peer_address),
                                   now + std::chrono::seconds(15)});
  return true;
}

std::optional<ChannelTransfer> ChannelDirectory::Consume(
    std::uint32_t account_id, const std::string& peer_address,
    std::uint64_t destination_session_id, Clock::time_point now) {
  Expire(now);
  const auto found = pending_.find(account_id);
  if (found == pending_.end() || found->second.peer_address != peer_address ||
      destination_session_id == 0 ||
      destination_session_id == found->second.identity.source_session_id) {
    return std::nullopt;
  }

  auto transfer = std::move(found->second.identity);
  pending_.erase(found);
  return transfer;
}

bool ChannelDirectory::HasPending(std::uint64_t session_id,
                                  Clock::time_point now) {
  Expire(now);
  return std::any_of(
      pending_.begin(), pending_.end(), [session_id](const auto& entry) {
        return entry.second.identity.source_session_id == session_id;
      });
}

void ChannelDirectory::Cancel(std::uint64_t session_id) {
  std::erase_if(pending_, [session_id](const auto& entry) {
    return entry.second.identity.source_session_id == session_id;
  });
}

void ChannelDirectory::Expire(Clock::time_point now) {
  std::erase_if(pending_, [now](const auto& entry) {
    return entry.second.expires_at <= now;
  });
}
}  // namespace bubblefighter::game
