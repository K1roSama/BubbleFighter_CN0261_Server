#ifndef BUBBLEFIGHTER_SERVER_GAME_PLAZA_DIRECTORY_H_
#define BUBBLEFIGHTER_SERVER_GAME_PLAZA_DIRECTORY_H_

#include <array>
#include <map>

#include "game/session_ping_tracker.h"
#include "protocol/plaza_messages.h"
#include "protocol/room_messages.h"

namespace bubblefighter::game {
inline constexpr std::size_t kPlazaCapacity = 16;

struct PlazaMember {
  std::uint64_t session_id = 0;
  protocol::SessionMember profile;
  bool initialized = false;
  std::optional<protocol::PlazaPosition> position;
};

struct PlazaSnapshot {
  std::uint64_t instance_id = 0;
  std::uint32_t channel_type = 0;
  std::uint32_t channel_index = 0;
  std::uint32_t master_slot = 0;
  std::array<PlazaMember, kPlazaCapacity> members;

  protocol::SessionSetting GetSetting() const;
};

struct PlazaChange {
  PlazaSnapshot plaza;
  std::uint32_t slot = 0;
  bool changed = false;
};

// Ephemeral presence, serialized on the application's Asio event loop.
class PlazaDirectory {
 public:
  std::optional<PlazaChange> UpdateLevel(std::uint32_t character_id,
                                         std::uint32_t level);
  PlazaChange Enter(std::uint64_t session_id, protocol::SessionMember profile,
                    std::uint32_t channel_type, std::uint32_t channel_index);
  std::optional<PlazaChange> Find(std::uint64_t session_id) const;
  std::optional<PlazaChange> RequestPing(
      std::uint64_t session_id, std::uint32_t slot,
      SessionPingTracker::Clock::time_point now =
          SessionPingTracker::Clock::now());
  std::optional<std::uint64_t> CompletePing(
      std::uint64_t session_id, std::uint32_t responder_slot,
      std::uint32_t requester_slot,
      SessionPingTracker::Clock::time_point now =
          SessionPingTracker::Clock::now());
  std::optional<PlazaChange> Leave(std::uint64_t session_id);
  std::optional<PlazaChange> Initialize(std::uint64_t session_id);
  std::optional<PlazaChange> Move(std::uint64_t session_id,
                                  const protocol::PlazaPosition& position);
  std::optional<PlazaChange> UpdateInventory(
      std::uint32_t character_id,
      const std::vector<inventory::InventoryItem>& items);
  bool ContainsCharacter(std::uint32_t character_id,
                         std::uint64_t excluded_session = 0) const;

 private:
  std::map<std::uint64_t, PlazaSnapshot> plazas_;
  std::map<std::uint64_t, SessionPingTracker> pings_;
  std::uint64_t next_instance_id_ = 1;
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_PLAZA_DIRECTORY_H_
