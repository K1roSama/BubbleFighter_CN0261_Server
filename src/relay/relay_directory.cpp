#include "relay/relay_directory.h"

#include <openssl/crypto.h>
#include <openssl/rand.h>

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace bubblefighter::relay {
std::optional<protocol::RelaySessionKey> RelayDirectory::Reserve(
    RelayMember member, Clock::time_point now,
    std::chrono::milliseconds lifetime) {
  if (lifetime <= std::chrono::milliseconds::zero() ||
      lifetime > std::chrono::minutes(5)) {
    return std::nullopt;
  }

  Expire(now);
  if (member.owner_session_id == 0 || member.room_id == 0 ||
      member.room_id > 32767 || member.slot >= 8 || member.peer_id == 0 ||
      member.peer_id == 0xFFFFFFFFu || member.peer_address.empty() ||
      reservations_.size() >= 8192 ||
      reservations_.contains(member.owner_session_id)) {
    return std::nullopt;
  }
  for (const auto& [owner, reservation] : reservations_) {
    static_cast<void>(owner);
    if (reservation.member.room_id == member.room_id &&
        (reservation.member.slot == member.slot ||
         reservation.member.peer_id == member.peer_id)) {
      return std::nullopt;
    }
  }

  protocol::RelaySessionKey key{};
  bool duplicate = false;
  do {
    if (RAND_bytes(key.data(), static_cast<int>(key.size())) != 1) {
      throw std::runtime_error("Relay credential generation failed");
    }
    duplicate = std::any_of(
        reservations_.begin(), reservations_.end(),
        [&key](const auto& entry) { return entry.second.key == key; });
  } while (duplicate);

  const auto owner = member.owner_session_id;
  reservations_.emplace(owner,
                        Reservation{std::move(member), key, now + lifetime});
  return key;
}

std::optional<RelayMember> RelayDirectory::Bind(
    std::uint64_t connection_id,
    const protocol::RelayRegistration& registration,
    const std::string& peer_address, Clock::time_point now) {
  Expire(now);
  if (connection_id == 0 || owners_by_connection_.contains(connection_id)) {
    return std::nullopt;
  }

  for (auto& [owner, reservation] : reservations_) {
    if (CRYPTO_memcmp(reservation.key.data(), registration.key.data(),
                      registration.key.size()) == 0) {
      const auto& member = reservation.member;
      if (reservation.connection_id != 0 || member.slot != registration.slot ||
          member.peer_id != registration.peer_id ||
          member.peer_address != peer_address) {
        return std::nullopt;
      }

      owners_by_connection_.emplace(connection_id, owner);
      reservation.connection_id = connection_id;
      return member;
    }
  }
  return std::nullopt;
}

std::optional<RelayMember> RelayDirectory::FindConnection(
    std::uint64_t connection_id) const {
  const auto found = owners_by_connection_.find(connection_id);
  if (found == owners_by_connection_.end()) {
    return std::nullopt;
  }
  return reservations_.at(found->second).member;
}

std::optional<std::uint64_t> RelayDirectory::FindOwnerConnection(
    std::uint64_t owner_session_id) const {
  const auto found = reservations_.find(owner_session_id);
  if (found == reservations_.end() || found->second.connection_id == 0) {
    return std::nullopt;
  }
  return found->second.connection_id;
}

std::optional<std::uint64_t> RelayDirectory::Revoke(
    std::uint64_t owner_session_id) {
  const auto found = reservations_.find(owner_session_id);
  if (found == reservations_.end()) {
    return std::nullopt;
  }

  const auto connection = found->second.connection_id;
  reservations_.erase(found);
  owners_by_connection_.erase(connection);
  return connection == 0 ? std::nullopt : std::optional(connection);
}

void RelayDirectory::Release(std::uint64_t connection_id) {
  const auto found = owners_by_connection_.find(connection_id);
  if (found != owners_by_connection_.end()) {
    Revoke(found->second);
  }
}

void RelayDirectory::Clear() {
  owners_by_connection_.clear();
  reservations_.clear();
}

void RelayDirectory::Expire(Clock::time_point now) {
  std::erase_if(reservations_, [now](const auto& entry) {
    return entry.second.connection_id == 0 && entry.second.expires_at <= now;
  });
}
}  // namespace bubblefighter::relay
