#include "game/plaza_directory.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

#include "game/channel_directory.h"

namespace bubblefighter::game {
std::optional<PlazaChange> PlazaDirectory::UpdateLevel(
    std::uint32_t character_id, std::uint32_t level) {
  if (level < 1 || level > 200) {
    throw std::invalid_argument(
        "Character level is outside the supported progression range");
  }
  for (auto& [id, plaza] : plazas_) {
    static_cast<void>(id);
    for (std::size_t slot = 0; slot < plaza.members.size(); ++slot) {
      auto& member = plaza.members[slot];
      if (member.profile.character &&
          member.profile.character->id == character_id &&
          member.profile.character->level != level) {
        member.profile.character->level = level;
        return PlazaChange{plaza, static_cast<std::uint32_t>(slot), true};
      }
    }
  }
  return std::nullopt;
}

protocol::SessionSetting PlazaSnapshot::GetSetting() const {
  protocol::SessionSetting setting;
  setting.kind =
      3;  // 0x8A6D50 selects LobbyStage, which uses Scenes/stage_Plaza/track.x.
  setting.state = 2;
  setting.title = u"Plaza";
  setting.capacity = static_cast<std::uint16_t>(members.size());
  setting.member_count = static_cast<std::uint16_t>(
      std::count_if(members.begin(), members.end(),
                    [](const auto& member) { return member.session_id != 0; }));
  // Room number zero: plazas are assigned by channel and are not joinable
  // battle room cards.
  return setting;
}

PlazaChange PlazaDirectory::Enter(std::uint64_t session_id,
                                  protocol::SessionMember profile,
                                  std::uint32_t channel_type,
                                  std::uint32_t channel_index) {
  if (session_id == 0 || !profile.character || profile.character->id == 0 ||
      profile.character->account_id == 0 ||
      profile.items.size() > inventory::kMaximumInventoryItems ||
      !ChannelDirectory::Contains(channel_type, channel_index) ||
      ContainsCharacter(profile.character->id, session_id)) {
    throw std::invalid_argument(
        "Plaza admission requires an available authenticated character and channel");
  }
  if (const auto existing = Find(session_id)) {
    if (existing->plaza.channel_type != channel_type ||
        existing->plaza.channel_index != channel_index ||
        existing->plaza.members[existing->slot].profile.character->id !=
            profile.character->id) {
      throw std::invalid_argument(
          "Leave the previous plaza before changing identity or channel");
    }
    return *existing;
  }
  auto found = std::find_if(
      plazas_.begin(), plazas_.end(),
      [channel_type, channel_index](const auto& entry) {
        return entry.second.channel_type == channel_type &&
               entry.second.channel_index == channel_index &&
               entry.second.GetSetting().member_count < kPlazaCapacity;
      });
  if (found == plazas_.end()) {
    PlazaSnapshot plaza;
    plaza.instance_id = next_instance_id_++;
    plaza.channel_type = channel_type;
    plaza.channel_index = channel_index;
    found = plazas_.emplace(plaza.instance_id, std::move(plaza)).first;
  }
  auto& plaza = found->second;
  for (std::size_t slot = 0; slot < plaza.members.size(); ++slot) {
    if (plaza.members[slot].session_id == 0) {
      profile.team = 1;
      profile.closed = false;
      profile.ready = false;
      profile.activity_flags = {};
      plaza.members[slot] = {session_id, std::move(profile), false, {}};
      return {plaza, static_cast<std::uint32_t>(slot), true};
    }
  }
  throw std::logic_error("Plaza admission selected a full instance");
}

std::optional<PlazaChange> PlazaDirectory::Find(
    std::uint64_t session_id) const {
  if (session_id != 0) {
    for (const auto& [id, plaza] : plazas_) {
      static_cast<void>(id);
      for (std::size_t slot = 0; slot < plaza.members.size(); ++slot) {
        if (plaza.members[slot].session_id == session_id) {
          return PlazaChange{plaza, static_cast<std::uint32_t>(slot), false};
        }
      }
    }
  }
  return {};
}

std::optional<PlazaChange> PlazaDirectory::Leave(std::uint64_t session_id) {
  const auto current = Find(session_id);
  if (!current) {
    return {};
  }
  auto& plaza = plazas_.at(current->plaza.instance_id);
  if (const auto tracker = pings_.find(plaza.instance_id);
      tracker != pings_.end()) {
    tracker->second.Remove(session_id);
  }
  plaza.members[current->slot] = {};
  if (plaza.master_slot == current->slot) {
    for (std::size_t slot = 0; slot < plaza.members.size(); ++slot) {
      if (plaza.members[slot].session_id != 0) {
        plaza.master_slot = static_cast<std::uint32_t>(slot);
        break;
      }
    }
  }
  PlazaChange departure{plaza, current->slot,
                        plaza.master_slot != current->plaza.master_slot};
  if (plaza.GetSetting().member_count == 0) {
    pings_.erase(current->plaza.instance_id);
    plazas_.erase(current->plaza.instance_id);
  }
  return departure;
}

std::optional<PlazaChange> PlazaDirectory::RequestPing(
    std::uint64_t session_id, std::uint32_t slot,
    SessionPingTracker::Clock::time_point now) {
  const auto current = Find(session_id);
  if (!current || !current->plaza.members[current->slot].initialized) {
    return {};
  }

  std::array<std::uint64_t, kPlazaCapacity> members{};
  for (std::size_t index = 0; index < members.size(); ++index) {
    const auto& member = current->plaza.members[index];
    members[index] = member.initialized ? member.session_id : 0;
  }
  if (!pings_[current->plaza.instance_id].Begin(session_id, slot, members,
                                                now)) {
    return {};
  }
  return current;
}

std::optional<std::uint64_t> PlazaDirectory::CompletePing(
    std::uint64_t session_id, std::uint32_t responder_slot,
    std::uint32_t requester_slot, SessionPingTracker::Clock::time_point now) {
  const auto current = Find(session_id);
  if (!current) {
    return {};
  }
  const auto tracker = pings_.find(current->plaza.instance_id);
  if (tracker == pings_.end()) {
    return {};
  }

  std::array<std::uint64_t, kPlazaCapacity> members{};
  for (std::size_t index = 0; index < members.size(); ++index) {
    const auto& member = current->plaza.members[index];
    members[index] = member.initialized ? member.session_id : 0;
  }
  return tracker->second.Complete(session_id, responder_slot, requester_slot,
                                  members, now);
}

std::optional<PlazaChange> PlazaDirectory::Initialize(
    std::uint64_t session_id) {
  const auto current = Find(session_id);
  if (!current) {
    return {};
  }
  auto& plaza = plazas_.at(current->plaza.instance_id);
  auto& member = plaza.members[current->slot];
  const auto changed = !member.initialized;
  member.initialized = true;
  return PlazaChange{plaza, current->slot, changed};
}

std::optional<PlazaChange> PlazaDirectory::Move(
    std::uint64_t session_id, const protocol::PlazaPosition& position) {
  const auto current = Find(session_id);
  if (!current) {
    return {};
  }
  auto& plaza = plazas_.at(current->plaza.instance_id);
  plaza.members[current->slot].position = position;
  return PlazaChange{plaza, current->slot, true};
}

std::optional<PlazaChange> PlazaDirectory::UpdateInventory(
    std::uint32_t character_id,
    const std::vector<inventory::InventoryItem>& items) {
  if (items.size() > inventory::kMaximumInventoryItems) {
    throw std::invalid_argument("Plaza member inventory exceeds capacity");
  }
  for (auto& [id, plaza] : plazas_) {
    static_cast<void>(id);
    for (std::size_t slot = 0; slot < plaza.members.size(); ++slot) {
      auto& member = plaza.members[slot];
      if (member.profile.character &&
          member.profile.character->id == character_id) {
        member.profile.items = items;
        return PlazaChange{plaza, static_cast<std::uint32_t>(slot), true};
      }
    }
  }
  return {};
}

bool PlazaDirectory::ContainsCharacter(std::uint32_t character_id,
                                       std::uint64_t excluded_session) const {
  for (const auto& [id, plaza] : plazas_) {
    static_cast<void>(id);
    for (const auto& member : plaza.members) {
      if (member.session_id != excluded_session && member.profile.character &&
          member.profile.character->id == character_id) {
        return true;
      }
    }
  }
  return false;
}
}  // namespace bubblefighter::game
