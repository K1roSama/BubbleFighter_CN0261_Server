#ifndef BUBBLEFIGHTER_SERVER_GAME_ROOM_DIRECTORY_H_
#define BUBBLEFIGHTER_SERVER_GAME_ROOM_DIRECTORY_H_

#include <array>
#include <map>
#include <random>

#include "game/game_map_catalog.h"
#include "game/session_ping_tracker.h"
#include "protocol/game_start_messages.h"
#include "protocol/room_messages.h"
#include "protocol/session_member.h"
#include "protocol/session_setting.h"

namespace bubblefighter::game {
inline constexpr std::size_t kRoomCapacity = 8;

struct RoomMember {
  std::uint64_t session_id = 0;
  protocol::SessionMember profile;
  bool changing_inventory = false;
};

struct RoomSnapshot {
  protocol::SessionSetting setting;
  std::uint32_t channel_type = 0;
  std::uint32_t channel_index = 0;
  std::uint32_t master_slot = 0;
  std::array<RoomMember, kRoomCapacity> members;
};

enum class RoomAdmissionResult {
  kSuccess,
  kUnavailable,
  kFull,
  kWrongPassword
};

struct RoomAdmission {
  RoomAdmissionResult result = RoomAdmissionResult::kUnavailable;
  std::uint32_t slot = 0;
  std::optional<RoomSnapshot> room;
};

struct RoomDeparture {
  std::uint32_t slot = 0;
  bool master_changed = false;
  RoomSnapshot room;
};

struct RoomKick {
  protocol::RoomKickResult result = protocol::RoomKickResult::kUnavailable;
  std::uint64_t target_session_id = 0;
  std::optional<RoomDeparture> departure;
};

struct RoomPage {
  std::uint32_t page = 0;
  bool has_more = false;
  std::vector<protocol::SessionSetting> settings;
};

struct RoomGameStart {
  protocol::GameStartResult result = protocol::GameStartResult::kUnavailable;
  std::optional<RoomSnapshot> room;
  std::uint8_t team_spawn_count = 0;
};

// Room state is ephemeral and serialized by the application's single Asio event
// loop.
class RoomDirectory {
 public:
  std::optional<RoomSnapshot> UpdateLevel(std::uint32_t character_id,
                                          std::uint32_t level);
  explicit RoomDirectory(GameMapCatalog maps);
  RoomAdmission Create(std::uint64_t session_id, protocol::SessionMember member,
                       std::uint32_t channel_type, std::uint32_t channel_index,
                       const protocol::SessionSetting& request);
  RoomAdmission Join(std::uint64_t session_id, protocol::SessionMember member,
                     std::uint32_t channel_type, std::uint32_t channel_index,
                     std::uint32_t room_id, std::u16string_view password);
  std::optional<RoomDeparture> Leave(std::uint64_t session_id);
  RoomKick Kick(std::uint64_t requester_session_id,
                std::u16string_view target_nickname);
  std::optional<RoomSnapshot> UpdateInventory(
      std::uint64_t session_id, std::vector<inventory::InventoryItem> items,
      std::uint32_t character_id = 0, bool preserve_ready_if_unchanged = false);
  std::optional<RoomSnapshot> FindBySession(std::uint64_t session_id) const;
  std::optional<RoomSnapshot> RequestPing(
      std::uint64_t session_id, std::uint32_t slot,
      SessionPingTracker::Clock::time_point now =
          SessionPingTracker::Clock::now());
  std::optional<std::uint64_t> CompletePing(
      std::uint64_t session_id, std::uint32_t responder_slot,
      std::uint32_t requester_slot,
      SessionPingTracker::Clock::time_point now =
          SessionPingTracker::Clock::now());
  bool ContainsCharacter(std::uint32_t character_id,
                         std::uint64_t excluded_session = 0) const;
  std::optional<RoomSnapshot> SetReadyStatus(
      std::uint64_t session_id, bool ready,
      const std::array<bool, 2>& activity_flags);
  std::optional<RoomSnapshot> SwitchTeam(std::uint64_t session_id);
  std::optional<RoomSnapshot> ChangeGameSetting(
      std::uint64_t session_id,
      const protocol::RoomGameSettingRequest& request);
  RoomGameStart BeginGame(std::uint64_t session_id,
                          const protocol::GameStartRequest& request);
  std::optional<RoomSnapshot> SetGameState(std::uint16_t room_id,
                                           std::uint32_t state);
  bool SetInventoryBusy(std::uint64_t session_id, bool busy,
                        std::uint32_t character_id = 0);
  RoomPage List(std::uint32_t channel_type, std::uint32_t channel_index,
                std::uint16_t page, std::uint16_t count) const;

 private:
  struct Room {
    RoomSnapshot snapshot;
    std::u16string password;
    SessionPingTracker pings;
  };

  bool CanAdmit(std::uint64_t session_id,
                const protocol::SessionMember& member) const;
  std::optional<protocol::GameSetting> NormalizeGameSetting(
      const protocol::GameSetting& request) const;
  std::uint16_t AllocateRoomId();
  GameMapCatalog maps_;
  std::mt19937 random_{std::random_device{}()};
  std::map<std::uint16_t, Room> rooms_;
  std::map<std::uint32_t, std::uint64_t> inventory_operations_;
  std::uint16_t next_room_id_ = 1;
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_ROOM_DIRECTORY_H_
