#include "game/room_directory.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

#include "game/channel_directory.h"
#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::game {
std::optional<RoomSnapshot> RoomDirectory::UpdateLevel(
    std::uint32_t character_id, std::uint32_t level) {
  if (level < 1 || level > 200) {
    throw std::invalid_argument(
        "Character level is outside the supported progression range");
  }
  for (auto& [id, room] : rooms_) {
    static_cast<void>(id);
    for (auto& member : room.snapshot.members) {
      if (member.profile.character &&
          member.profile.character->id == character_id &&
          member.profile.character->level != level) {
        member.profile.character->level = level;
        return room.snapshot;
      }
    }
  }
  return std::nullopt;
}

namespace {
bool IsRoomText(std::u16string_view value, std::size_t maximum,
                bool allow_empty) {
  if ((!allow_empty && value.empty()) || value.size() > maximum) {
    return false;
  }
  for (std::size_t index = 0; index < value.size(); ++index) {
    const auto unit = value[index];
    if (unit < 0x20 || unit == 0x7F || (unit >= 0xDC00 && unit <= 0xDFFF)) {
      return false;
    }
    if (unit >= 0xD800 && unit <= 0xDBFF) {
      if (++index >= value.size() || value[index] < 0xDC00 ||
          value[index] > 0xDFFF) {
        return false;
      }
    }
  }
  return true;
}

void UpdateCounts(RoomSnapshot& room) {
  room.setting.capacity = static_cast<std::uint16_t>(room.members.size());
  room.setting.value42 = 0;  // Excluded from the listed member count; its
                             // participant kind is not confirmed.
  room.setting.member_count = static_cast<std::uint16_t>(std::count_if(
      room.members.begin(), room.members.end(),
      [](const RoomMember& member) { return member.session_id != 0; }));
}

std::uint8_t ChooseTeam(const RoomSnapshot& room) {
  std::size_t first = 0;
  std::size_t second = 0;
  for (const auto& member : room.members) {
    if (member.session_id != 0) {
      first += member.profile.team == 1 ? 1 : 0;
      second += member.profile.team == 2 ? 1 : 0;
    }
  }
  return first <= second ? 1 : 2;
}
}  // namespace

RoomDirectory::RoomDirectory(GameMapCatalog maps) : maps_(std::move(maps)) {}

bool RoomDirectory::ContainsCharacter(std::uint32_t character_id,
                                      std::uint64_t excluded_session) const {
  for (const auto& [id, entry] : rooms_) {
    static_cast<void>(id);
    for (const auto& member : entry.snapshot.members) {
      if (member.session_id != excluded_session && member.profile.character &&
          member.profile.character->id == character_id) {
        return true;
      }
    }
  }
  return false;
}

std::optional<protocol::GameSetting> RoomDirectory::NormalizeGameSetting(
    const protocol::GameSetting& request) const {
  if ((request.mode != 1 && request.mode != 3) ||
      (request.sub_mode != 0 && request.sub_mode != 1 &&
       request.sub_mode != 3 && request.sub_mode != 4) ||
      !IsRoomText(request.strings16[0], protocol::kSessionStringLimit, true)) {
    return {};
  }

  // 0x6077D0 keeps +100 == Random while resolving +44 for loading; 0x606160
  // clears +44 on return.
  const auto& resource = request.strings16[1].empty() ? request.strings16[3]
                                                      : request.strings16[1];
  const bool random = resource.empty() || request.strings16[3] == u"Random" ||
                      resource == u"Random";
  const auto theme = request.strings16[2].empty() ? std::u16string(u"All")
                                                  : request.strings16[2];
  const auto* map = random ? &maps_.GetDefaultRules() : maps_.Find(resource);
  if ((random && (maps_.FindCandidates(request.mode, theme).empty() ||
                  (!request.strings16[3].empty() &&
                   request.strings16[3] != u"Random"))) ||
      (!random &&
       (map == nullptr || (request.mode == 1 && !map->normal_mode) ||
        (request.mode == 3 && !map->item_mode) ||
        (!request.strings16[1].empty() && !request.strings16[3].empty() &&
         request.strings16[1] != request.strings16[3])))) {
    return {};
  }

  const auto time = request.values128[2] == 0 ? map->time_limit_milliseconds
                                              : request.values128[2];
  const auto score =
      request.values128[0] == 0 ? map->score_limit : request.values128[0];
  if (time < 60000 || time > 1800000 || time % 1000 != 0 || score > 200) {
    return {};
  }

  // Keep only confirmed ordinary-room inputs. Special-mode arrays and
  // client-supplied auxiliary values must not become authoritative rules or
  // appear in another member's resource lookups.
  protocol::GameSetting setting;
  setting.mode = request.mode;
  setting.sub_mode = request.sub_mode;
  setting.flag4 = request.flag4;
  setting.flag5 = request.flag5;
  setting.flag444 = request.flag444;
  if (random) {
    setting.strings16 = {u"Random", u"", theme, u"Random"};
  } else {
    setting.strings16 = {request.strings16[0].empty() ? map->resource_name
                                                      : request.strings16[0],
                         map->resource_name, map->theme, map->resource_name};
  }
  setting.values128[0] = score;
  setting.values128[2] = time;
  return setting;
}

bool RoomDirectory::CanAdmit(std::uint64_t session_id,
                             const protocol::SessionMember& member) const {
  if (session_id == 0 || !member.character || member.character->id == 0 ||
      member.character->account_id == 0 ||
      (member.character->character_type != 1 &&
       member.character->character_type != 2) ||
      characters::CheckNickname(member.character->nickname) !=
          characters::CreationResult::kSuccess ||
      member.items.size() > inventory::kMaximumInventoryItems ||
      inventory_operations_.contains(member.character->id)) {
    return false;
  }
  for (const auto& [id, room] : rooms_) {
    static_cast<void>(id);
    for (const auto& existing : room.snapshot.members) {
      if (existing.session_id == session_id ||
          (existing.profile.character &&
           existing.profile.character->account_id ==
               member.character->account_id)) {
        return false;
      }
    }
  }
  return true;
}

std::uint16_t RoomDirectory::AllocateRoomId() {
  // The original room list sign-extends a 16-bit ID when composing C403.
  for (std::size_t count = 0; count < 32767; ++count) {
    const auto candidate = next_room_id_;
    next_room_id_ =
        candidate == 32767 ? 1 : static_cast<std::uint16_t>(candidate + 1);
    if (!rooms_.contains(candidate)) {
      return candidate;
    }
  }
  throw std::runtime_error("Room identifier capacity exhausted");
}

RoomAdmission RoomDirectory::Create(std::uint64_t session_id,
                                    protocol::SessionMember member,
                                    std::uint32_t channel_type,
                                    std::uint32_t channel_index,
                                    const protocol::SessionSetting& request) {
  if (!ChannelDirectory::Contains(channel_type, channel_index) ||
      !CanAdmit(session_id, member) || rooms_.size() >= 1024 ||
      !IsRoomText(request.title, 64, false) ||
      !IsRoomText(request.password, 16, !request.password_required)) {
    return {};
  }

  protocol::PacketWriter validation;
  protocol::WriteSessionSetting(validation, request);
  // Bound the requested card too, before discarding untrusted special-mode
  // data.
  if (validation.GetBytes().size() >
      (protocol::LambdaPacketCodec::kMaxPayloadSize - 12) / 6) {
    return {};
  }
  auto game = NormalizeGameSetting(request.game);
  if (!game) {
    return {};
  }

  // 0x4ABB70/0x4AB9F0 supply the channel default; 0x4ABC10 puts it in the C402
  // map fields. Creating an ordinary room always uses the server default.
  // Explicit map selection belongs to C513.
  game->strings16 = {u"Random", u"", u"All", u"Random"};
  game->values128[0] = request.game.values128[0] == 0
                           ? maps_.GetDefaultRules().score_limit
                           : request.game.values128[0];
  game->values128[2] = request.game.values128[2] == 0
                           ? maps_.GetDefaultRules().time_limit_milliseconds
                           : request.game.values128[2];

  Room room;
  room.snapshot.setting.state = 2;
  room.snapshot.setting.title = request.title;
  room.snapshot.setting.password_required = request.password_required;
  room.snapshot.setting.kind = 2;
  room.snapshot.setting.game = *game;
  room.snapshot.setting.room_id = AllocateRoomId();
  room.snapshot.channel_type = channel_type;
  room.snapshot.channel_index = channel_index;
  room.password = request.password_required ? request.password : u"";
  member.team = 1;
  member.ready = false;
  member.closed = false;
  member.activity_flags = {};
  room.snapshot.members[0] = {session_id, std::move(member)};
  UpdateCounts(room.snapshot);
  const auto snapshot = room.snapshot;
  rooms_.emplace(snapshot.setting.room_id, std::move(room));
  return {RoomAdmissionResult::kSuccess, 0, snapshot};
}

RoomAdmission RoomDirectory::Join(std::uint64_t session_id,
                                  protocol::SessionMember member,
                                  std::uint32_t channel_type,
                                  std::uint32_t channel_index,
                                  std::uint32_t room_id,
                                  std::u16string_view password) {
  if (room_id == 0 || room_id > 32767 || !CanAdmit(session_id, member)) {
    return {};
  }
  const auto entry = rooms_.find(static_cast<std::uint16_t>(room_id));
  if (entry == rooms_.end() ||
      entry->second.snapshot.channel_type != channel_type ||
      entry->second.snapshot.channel_index != channel_index ||
      entry->second.snapshot.setting.state != 2) {
    return {};
  }
  auto& room = entry->second;
  if (room.snapshot.setting.password_required && room.password != password) {
    return {RoomAdmissionResult::kWrongPassword, 0, {}};
  }
  for (std::size_t slot = 0; slot < room.snapshot.members.size(); ++slot) {
    if (room.snapshot.members[slot].session_id == 0) {
      member.team = ChooseTeam(room.snapshot);
      member.ready = false;
      member.closed = false;
      member.activity_flags = {};
      room.snapshot.members[slot] = {session_id, std::move(member)};
      UpdateCounts(room.snapshot);
      return {RoomAdmissionResult::kSuccess, static_cast<std::uint32_t>(slot),
              room.snapshot};
    }
  }
  return {RoomAdmissionResult::kFull, 0, {}};
}

std::optional<RoomDeparture> RoomDirectory::Leave(std::uint64_t session_id) {
  for (auto entry = rooms_.begin(); entry != rooms_.end(); ++entry) {
    auto& room = entry->second.snapshot;
    for (std::size_t slot = 0; slot < room.members.size(); ++slot) {
      if (session_id != 0 && room.members[slot].session_id == session_id) {
        entry->second.pings.Remove(session_id);
        room.members[slot] = {};
        UpdateCounts(room);
        const auto changed = room.master_slot == slot;
        if (changed && room.setting.member_count != 0) {
          for (std::size_t remaining = 0; remaining < room.members.size();
               ++remaining) {
            if (room.members[remaining].session_id != 0) {
              room.master_slot = static_cast<std::uint32_t>(remaining);
              room.members[remaining].profile.ready = false;
              break;
            }
          }
        }
        RoomDeparture departure{static_cast<std::uint32_t>(slot), changed,
                                room};
        if (room.setting.member_count == 0) {
          rooms_.erase(entry);
        }
        return departure;
      }
    }
  }
  return {};
}

std::optional<RoomSnapshot> RoomDirectory::UpdateInventory(
    std::uint64_t session_id, std::vector<inventory::InventoryItem> items,
    std::uint32_t character_id, bool preserve_ready_if_unchanged) {
  if (items.size() > inventory::kMaximumInventoryItems) {
    throw std::invalid_argument("Room member inventory exceeds capacity");
  }
  for (auto& [id, room] : rooms_) {
    static_cast<void>(id);
    for (auto& member : room.snapshot.members) {
      if (session_id != 0 && member.session_id != 0 &&
          (member.session_id == session_id ||
           (character_id != 0 && member.profile.character &&
            member.profile.character->id == character_id))) {
        if (room.snapshot.setting.state != 2) {
          return {};
        }
        const auto unchanged =
            std::equal(items.begin(), items.end(), member.profile.items.begin(),
                       member.profile.items.end(),
                       [](const auto& left, const auto& right) {
                         return left.item_id == right.item_id &&
                                left.quantity == right.quantity &&
                                left.equipped_slot == right.equipped_slot;
                       });
        member.profile.items = std::move(items);
        if (!preserve_ready_if_unchanged || !unchanged) {
          member.profile.ready = false;
        }
        return room.snapshot;
      }
    }
  }
  return {};
}

RoomKick RoomDirectory::Kick(std::uint64_t requester_session_id,
                             std::u16string_view target_nickname) {
  const auto room = FindBySession(requester_session_id);
  if (!room || room->setting.state != 2 ||
      room->members[room->master_slot].session_id != requester_session_id) {
    return {};
  }

  for (const auto& member : room->members) {
    if (member.session_id != 0 && member.profile.character &&
        member.profile.character->nickname == target_nickname) {
      if (member.session_id == requester_session_id) {
        return {protocol::RoomKickResult::kSelf, 0, {}};
      }

      auto departure = Leave(member.session_id);
      return {protocol::RoomKickResult::kSuccess, member.session_id,
              std::move(departure)};
    }
  }
  return {protocol::RoomKickResult::kNotInRoom, 0, {}};
}

std::optional<RoomSnapshot> RoomDirectory::FindBySession(
    std::uint64_t session_id) const {
  for (const auto& [id, room] : rooms_) {
    static_cast<void>(id);
    for (const auto& member : room.snapshot.members) {
      if (session_id != 0 && member.session_id == session_id) {
        return room.snapshot;
      }
    }
  }
  return {};
}

std::optional<RoomSnapshot> RoomDirectory::SetReadyStatus(
    std::uint64_t session_id, bool ready,
    const std::array<bool, 2>& activity_flags) {
  if (static_cast<int>(ready) + static_cast<int>(activity_flags[0]) +
          static_cast<int>(activity_flags[1]) >
      1) {
    throw std::invalid_argument(
        "Room activity indicators are mutually exclusive");
  }
  for (auto& [id, entry] : rooms_) {
    static_cast<void>(id);
    auto& room = entry.snapshot;
    if (room.setting.state != 2) {
      continue;
    }
    for (std::size_t slot = 0; slot < room.members.size(); ++slot) {
      auto& member = room.members[slot];
      if (session_id != 0 && member.session_id == session_id) {
        member.profile.ready = ready && slot != room.master_slot;
        member.profile.activity_flags = activity_flags;
        return room;
      }
    }
  }
  return {};
}

std::optional<RoomSnapshot> RoomDirectory::SwitchTeam(
    std::uint64_t session_id) {
  for (auto& [id, entry] : rooms_) {
    static_cast<void>(id);
    auto& room = entry.snapshot;
    if (room.setting.state != 2) {
      continue;
    }
    for (auto& member : room.members) {
      if (session_id == 0 || member.session_id != session_id) {
        continue;
      }
      const std::uint8_t target = member.profile.team == 1 ? 2 : 1;
      const auto count =
          std::count_if(room.members.begin(), room.members.end(),
                        [target](const RoomMember& candidate) {
                          return candidate.session_id != 0 &&
                                 candidate.profile.team == target;
                        });
      if (count < static_cast<std::ptrdiff_t>(kRoomCapacity / 2)) {
        member.profile.team = target;
        member.profile.ready = false;
      }
      return room;
    }
  }
  return {};
}

RoomPage RoomDirectory::List(std::uint32_t channel_type,
                             std::uint32_t channel_index, std::uint16_t page,
                             std::uint16_t count) const {
  if (count == 0 || count > 6) {
    throw std::invalid_argument(
        "Room list page must request between one and six entries");
  }
  RoomPage result{page, false, {}};
  std::size_t matched = 0;
  const auto first = static_cast<std::size_t>(page) * count;
  for (const auto& [id, room] : rooms_) {
    static_cast<void>(id);
    if (room.snapshot.channel_type != channel_type ||
        room.snapshot.channel_index != channel_index) {
      continue;
    }
    if (matched++ < first) {
      continue;
    }
    if (result.settings.size() == count) {
      result.has_more = true;
      break;
    }
    result.settings.push_back(room.snapshot.setting);
  }
  return result;
}

std::optional<RoomSnapshot> RoomDirectory::ChangeGameSetting(
    std::uint64_t session_id, const protocol::RoomGameSettingRequest& request) {
  for (auto& [id, entry] : rooms_) {
    static_cast<void>(id);
    auto& room = entry.snapshot;
    if (session_id == 0 ||
        room.members[room.master_slot].session_id != session_id ||
        room.setting.state != 2) {
      continue;
    }

    protocol::GameSetting candidate;
    candidate.strings16 = request.strings4;
    candidate.mode = request.mode;
    candidate.sub_mode = request.sub_mode;
    candidate.flag4 = request.flag116;
    candidate.flag5 = request.flag117;
    candidate.flag444 = request.flag160;
    candidate.values128[0] = request.score_limit;
    candidate.values128[2] = request.time_limit_milliseconds;
    const auto setting = NormalizeGameSetting(candidate);
    if (!setting) {
      return {};
    }
    if (room.setting.game != *setting) {
      room.setting.game = *setting;
      for (auto& member : room.members) {
        member.profile.ready = false;
      }
    }
    return room;
  }
  return {};
}

RoomGameStart RoomDirectory::BeginGame(
    std::uint64_t session_id, const protocol::GameStartRequest& request) {
  for (auto& [id, entry] : rooms_) {
    static_cast<void>(id);
    auto& room = entry.snapshot;
    if (session_id == 0 ||
        room.members[room.master_slot].session_id != session_id) {
      continue;
    }
    const auto& resource = room.setting.game.strings16[1];
    const bool random = room.setting.game.strings16[3] == u"Random";
    const auto* map = maps_.Find(resource);
    const auto candidates =
        random ? maps_.FindCandidates(room.setting.game.mode,
                                      room.setting.game.strings16[2])
               : std::vector<const GameMap*>{};
    if (room.setting.state != 2 || request.flag4 ||
        (random &&
         (candidates.empty() || (!request.strings8[2].empty() &&
                                 request.strings8[2] != u"Random"))) ||
        (!random &&
         (map == nullptr || map->team_spawn_count == 0 ||
          (!request.strings8[0].empty() && request.strings8[0] != resource) ||
          (!request.strings8[2].empty() && request.strings8[2] != resource))) ||
        std::any_of(request.event_flags.begin(), request.event_flags.end(),
                    [](bool value) { return value; })) {
      return {protocol::GameStartResult::kUnavailable, room};
    }
    std::array<unsigned int, 2> teams{};
    bool ready = true;
    for (std::size_t slot = 0; slot < room.members.size(); ++slot) {
      const auto& member = room.members[slot];
      if (member.session_id == 0) {
        continue;
      }
      if (member.profile.team < 1 || member.profile.team > 2) {
        return {protocol::GameStartResult::kUnavailable, room};
      }
      ++teams[member.profile.team - 1];
      ready = ready && (slot == room.master_slot || member.profile.ready) &&
              !member.changing_inventory && !member.profile.activity_flags[0] &&
              !member.profile.activity_flags[1];
    }
    if (teams[0] + teams[1] < 2) {
      return {protocol::GameStartResult::kAlone, room};
    }
    if (!ready) {
      return {protocol::GameStartResult::kNotReady, room};
    }
    if (teams[0] != teams[1]) {
      return {protocol::GameStartResult::kUnequalTeams, room};
    }
    // The server owns the map and rules. C503 auxiliary values never replace
    // C513-validated settings.
    if (random) {
      std::uniform_int_distribution<std::size_t> choice(0,
                                                        candidates.size() - 1);
      map = candidates[choice(random_)];
      room.setting.game.strings16[1] = map->resource_name;
    }
    entry.pings.Clear();
    room.setting.state = 3;
    for (auto& member : room.members) {
      member.profile.intrusion_state = 0;
    }
    return {protocol::GameStartResult::kSuccess, room, map->team_spawn_count};
  }
  return {protocol::GameStartResult::kUnavailable, FindBySession(session_id)};
}

std::optional<RoomSnapshot> RoomDirectory::SetGameState(std::uint16_t room_id,
                                                        std::uint32_t state) {
  if (state != 2 && state != 4 && state != 5 && state != 6) {
    throw std::invalid_argument("Unsupported authoritative game state");
  }
  const auto entry = rooms_.find(room_id);
  if (entry == rooms_.end() ||
      (state != 2 && entry->second.snapshot.setting.state != state - 1)) {
    return {};
  }
  auto& room = entry->second.snapshot;
  entry->second.pings.Clear();
  room.setting.state = state;
  if (state == 2) {
    if (room.setting.game.strings16[3] == u"Random") {
      room.setting.game.strings16[1].clear();
    }
    for (auto& member : room.members) {
      member.profile.ready = false;
      member.profile.activity_flags = {};
      member.profile.intrusion_state = 0;
    }
  }
  return room;
}

std::optional<RoomSnapshot> RoomDirectory::RequestPing(
    std::uint64_t session_id, std::uint32_t slot,
    SessionPingTracker::Clock::time_point now) {
  const auto room = FindBySession(session_id);
  if (!room) {
    return {};
  }

  std::array<std::uint64_t, kRoomCapacity> members{};
  for (std::size_t index = 0; index < members.size(); ++index) {
    members[index] = room->members[index].session_id;
  }
  if (!rooms_.at(room->setting.room_id)
           .pings.Begin(session_id, slot, members, now)) {
    return {};
  }
  return room;
}

std::optional<std::uint64_t> RoomDirectory::CompletePing(
    std::uint64_t session_id, std::uint32_t responder_slot,
    std::uint32_t requester_slot, SessionPingTracker::Clock::time_point now) {
  const auto room = FindBySession(session_id);
  if (!room) {
    return {};
  }

  std::array<std::uint64_t, kRoomCapacity> members{};
  for (std::size_t index = 0; index < members.size(); ++index) {
    members[index] = room->members[index].session_id;
  }
  return rooms_.at(room->setting.room_id)
      .pings.Complete(session_id, responder_slot, requester_slot, members, now);
}

bool RoomDirectory::SetInventoryBusy(std::uint64_t session_id, bool busy,
                                     std::uint32_t character_id) {
  if (session_id == 0) {
    return false;
  }
  if (character_id == 0) {
    if (const auto room = FindBySession(session_id)) {
      for (const auto& member : room->members) {
        if (member.session_id == session_id && member.profile.character) {
          character_id = member.profile.character->id;
        }
      }
    }
  }
  if (character_id == 0) {
    return true;
  }
  if (!busy) {
    const auto operation = inventory_operations_.find(character_id);
    if (operation == inventory_operations_.end() ||
        operation->second != session_id) {
      return false;
    }
    inventory_operations_.erase(operation);
  } else {
    for (const auto& [id, entry] : rooms_) {
      static_cast<void>(id);
      for (const auto& member : entry.snapshot.members) {
        if (member.profile.character &&
            member.profile.character->id == character_id &&
            entry.snapshot.setting.state != 2) {
          return false;
        }
      }
    }
    if (!inventory_operations_.emplace(character_id, session_id).second) {
      return false;
    }
  }
  for (auto& [id, entry] : rooms_) {
    static_cast<void>(id);
    for (auto& member : entry.snapshot.members) {
      if (member.profile.character &&
          member.profile.character->id == character_id) {
        member.changing_inventory = busy;
      }
    }
  }
  return true;
}
}  // namespace bubblefighter::game
