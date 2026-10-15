#ifndef BUBBLEFIGHTER_SERVER_RELAY_RELAY_DIRECTORY_H_
#define BUBBLEFIGHTER_SERVER_RELAY_RELAY_DIRECTORY_H_

#include <chrono>
#include <map>
#include <optional>
#include <string>

#include "protocol/relay_messages.h"

namespace bubblefighter::relay {
struct RelayMember {
  std::uint64_t owner_session_id = 0;
  std::uint32_t room_id = 0;
  std::uint8_t slot = 0;
  std::uint32_t peer_id = 0;
  std::string peer_address;
};

// Application-owned reservations for a frozen room roster; all calls use one
// Asio event loop. The application must revoke reservations when a member
// leaves or the game generation changes.
class RelayDirectory {
 public:
  using Clock = std::chrono::steady_clock;

  std::optional<protocol::RelaySessionKey> Reserve(
      RelayMember member, Clock::time_point now = Clock::now(),
      std::chrono::milliseconds lifetime = std::chrono::seconds(30));
  std::optional<RelayMember> Bind(
      std::uint64_t connection_id,
      const protocol::RelayRegistration& registration,
      const std::string& peer_address, Clock::time_point now = Clock::now());
  std::optional<RelayMember> FindConnection(std::uint64_t connection_id) const;
  std::optional<std::uint64_t> FindOwnerConnection(
      std::uint64_t owner_session_id) const;
  std::optional<std::uint64_t> Revoke(std::uint64_t owner_session_id);
  void Release(std::uint64_t connection_id);
  void Clear();

 private:
  struct Reservation {
    RelayMember member;
    protocol::RelaySessionKey key;
    Clock::time_point expires_at;
    std::uint64_t connection_id = 0;
  };

  void Expire(Clock::time_point now);
  std::map<std::uint64_t, Reservation> reservations_;
  std::map<std::uint64_t, std::uint64_t> owners_by_connection_;
};
}  // namespace bubblefighter::relay

#endif  // BUBBLEFIGHTER_SERVER_RELAY_RELAY_DIRECTORY_H_
