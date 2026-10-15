#include <spdlog/spdlog.h>

#include <stdexcept>

#include "game/game_session.h"
#include "protocol/lobby_messages.h"
#include "protocol/room_messages.h"

namespace bubblefighter::game {
boost::asio::awaitable<void> GameSession::HandleRoomRequest(
    protocol::LambdaPacket packet) {
  if (packet.message_id == 505 || packet.message_id == 515) {
    std::optional<RoomSnapshot> room;
    if (packet.message_id == 505) {
      const auto status = protocol::ReadRoomReadyStatus(packet);
      room = rooms_.SetReadyStatus(session_id_, status.ready,
                                   status.activity_flags);
    } else {
      protocol::ValidateRoomTeamRequest(packet);
      room = rooms_.SwitchTeam(session_id_);
    }
    if (room) {
      for (std::size_t slot = 0; slot < room->members.size(); ++slot) {
        const auto& member = room->members[slot];
        if (member.session_id == session_id_) {
          const auto index = static_cast<std::uint32_t>(slot);
          if (packet.message_id == 515) {
            BroadcastRoom(*room, protocol::CreateRoomTeamReply(
                                     index, member.profile.team));
          }
          BroadcastRoom(*room,
                        protocol::CreateRoomSlotReply(index, member.profile));
          break;
        }
      }
    }
    co_return;
  }
  if (packet.message_id == 404) {
    protocol::ValidateLeaveRoomRequest(packet);
    co_await EnterLobby();
    co_return;
  }

  const auto creating = packet.message_id == 402;
  std::optional<protocol::CreateRoomRequest> create;
  std::optional<protocol::JoinRoomRequest> join;
  if (creating) {
    create = protocol::ReadCreateRoomRequest(packet);
  } else {
    join = protocol::ReadJoinRoomRequest(packet);
  }
  if (!in_lobby_ || plazas_.ContainsCharacter(character_->id, session_id_)) {
    connection_.Send(protocol::CreateRoomResult(creating, creating ? 1 : 5));
    co_return;
  }

  auto items = co_await ReadInventory(true);
  if (stopped_) {
    co_return;
  }
  // Another connection may have entered a plaza while this inventory query was
  // suspended.
  if (plazas_.ContainsCharacter(character_->id, session_id_)) {
    connection_.Send(protocol::CreateRoomResult(creating, creating ? 1 : 5));
    co_return;
  }
  protocol::SessionMember profile{character_, std::move(items), 1};
  const auto admission =
      creating ? rooms_.Create(session_id_, std::move(profile), channel_type_,
                               channel_index_, create->setting)
               : rooms_.Join(session_id_, std::move(profile), channel_type_,
                             channel_index_, join->room_id, join->password);
  if (admission.result != RoomAdmissionResult::kSuccess) {
    const auto result =
        creating                                                  ? 1u
        : admission.result == RoomAdmissionResult::kFull          ? 3u
        : admission.result == RoomAdmissionResult::kWrongPassword ? 4u
                                                                  : 5u;
    connection_.Send(protocol::CreateRoomResult(creating, result));
    co_return;
  }

  LeavePlaza();
  in_lobby_ = false;
  const auto& room = *admission.room;
  std::array<protocol::SessionMember, kRoomCapacity> members;
  for (std::size_t index = 0; index < members.size(); ++index) {
    members[index] = room.members[index].profile;
  }
  connection_.Send(protocol::CreateRoomResult(creating, 0));
  connection_.Send(protocol::CreateRoomEntryReply(
      room.setting, room.master_slot, admission.slot, members));
  if (!creating) {
    BroadcastRoom(
        room,
        protocol::CreateRoomSlotReply(admission.slot, members[admission.slot]),
        session_id_);
    BroadcastRoom(room, protocol::CreateRoomSettingReply(room.setting),
                  session_id_);
  }
  spdlog::info(
      "[Room] session={} roomId={} slot={} action={} random={} clientMapProvided={}",
      session_id_, room.setting.room_id, admission.slot,
      creating ? "created" : "joined",
      room.setting.game.strings16[3] == u"Random",
      create && (!create->setting.game.strings16[1].empty() ||
                 !create->setting.game.strings16[3].empty()));
}

void GameSession::SendRoomList(const protocol::LambdaPacket& packet) {
  const auto request = protocol::ReadSessionListRequest(packet);
  if (!in_lobby_) {
    throw std::invalid_argument("ReqListSession requires lobby membership");
  }
  const auto page =
      rooms_.List(channel_type_, channel_index_, request.page, request.count);
  std::vector<std::vector<std::uint8_t>> settings;
  for (const auto& setting : page.settings) {
    protocol::PacketWriter writer;
    protocol::WriteSessionSetting(writer, setting);
    settings.push_back(writer.GetBytes());
  }
  connection_.Send(
      protocol::CreateSessionListReply(page.page, page.has_more, settings));
}

void GameSession::HandleRoomGameSetting(const protocol::LambdaPacket& packet) {
  const auto request = protocol::ReadRoomGameSettingRequest(packet);
  const auto previous = rooms_.FindBySession(session_id_);
  if (!previous) {
    return;
  }

  const auto room = rooms_.ChangeGameSetting(session_id_, request);
  if (!room) {
    // No separate failure result is confirmed for C513; restore the caller's
    // authoritative settings.
    connection_.Send(
        protocol::CreateRoomGameSettingReply(previous->setting.game));
    return;
  }

  BroadcastRoom(*room,
                protocol::CreateRoomGameSettingReply(room->setting.game));
  for (std::size_t slot = 0; slot < room->members.size(); ++slot) {
    if (previous->members[slot].profile.ready !=
        room->members[slot].profile.ready) {
      BroadcastRoom(
          *room, protocol::CreateRoomSlotReply(static_cast<std::uint32_t>(slot),
                                               room->members[slot].profile));
    }
  }
}

void GameSession::BroadcastRoom(const RoomSnapshot& room,
                                const protocol::LambdaPacket& packet,
                                std::uint64_t excluded_session) {
  for (const auto& member : room.members) {
    if (member.session_id != 0 && member.session_id != excluded_session) {
      notify_(member.session_id, packet);
    }
  }
}

void GameSession::HandleRoomKick(const protocol::LambdaPacket& packet) {
  const auto target = protocol::ReadRoomKickRequest(packet);
  const auto kick = rooms_.Kick(session_id_, target);
  const auto reply =
      protocol::CreateRoomKickReply(character_->nickname, target, kick.result);
  if (!kick.departure) {
    connection_.Send(reply);
    return;
  }

  // S513 displays the result while the original RoomStage is still active; it
  // does not move the target itself.
  const auto& departure = *kick.departure;
  notify_(kick.target_session_id, reply);
  notify_(session_id_, reply);
  BroadcastRoom(departure.room,
                protocol::CreateRoomDepartureReply(departure.slot));
  BroadcastRoom(departure.room,
                protocol::CreateRoomSettingReply(departure.room.setting));
  return_to_lobby_(kick.target_session_id);
  spdlog::info("[Room] session={} roomId={} targetSession={} kicked",
               session_id_, departure.room.setting.room_id,
               kick.target_session_id);
}

void GameSession::LeaveRoom() {
  rounds_.CancelForSession(session_id_, session_id_);
  const auto departure = rooms_.Leave(session_id_);
  if (!departure) {
    return;
  }
  const auto& room = departure->room;
  BroadcastRoom(room, protocol::CreateRoomDepartureReply(departure->slot));
  if (departure->master_changed && room.setting.member_count != 0) {
    BroadcastRoom(room, protocol::CreateRoomMasterReply(room.master_slot));
  }
  BroadcastRoom(room, protocol::CreateRoomSettingReply(room.setting));
  spdlog::info("[Room] session={} roomId={} slot={} left remaining={}",
               session_id_, room.setting.room_id, departure->slot,
               room.setting.member_count);
}

void GameSession::RefreshRoomInventory(
    const std::vector<inventory::InventoryItem>& items, bool notify_owner) {
  if (const auto plaza = plazas_.UpdateInventory(character_->id, items)) {
    BroadcastPlaza(plaza->plaza,
                   protocol::CreateRoomSlotReply(
                       plaza->slot, plaza->plaza.members[plaza->slot].profile),
                   session_id_);
  }
  // Another session's pre-start check may already have published the same
  // expiry change.
  const auto room =
      rooms_.UpdateInventory(session_id_, items, character_->id, notify_owner);
  if (!room) {
    return;
  }
  for (std::size_t slot = 0; slot < room->members.size(); ++slot) {
    if (room->members[slot].profile.character &&
        room->members[slot].profile.character->id == character_->id) {
      BroadcastRoom(
          *room,
          protocol::CreateRoomSlotReply(static_cast<std::uint32_t>(slot),
                                        room->members[slot].profile),
          notify_owner ? 0 : session_id_);
      return;
    }
  }
}
}  // namespace bubblefighter::game
