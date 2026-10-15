#include "protocol/room_messages.h"

#include <algorithm>
#include <stdexcept>

#include "protocol/inventory_messages.h"

namespace bubblefighter::protocol {
CreateRoomRequest ReadCreateRoomRequest(const LambdaPacket& packet) {
  if (packet.message_id != 402) {
    throw std::invalid_argument("Expected ReqCreateSession");
  }

  PacketReader reader(packet.payload);
  CreateRoomRequest request;
  request.setting = ReadSessionSetting(reader);
  request.string600 = reader.ReadString(kSessionStringLimit);
  request.flag628 = reader.ReadBoolean();
  reader.RequireEnd();
  return request;
}

JoinRoomRequest ReadJoinRoomRequest(const LambdaPacket& packet) {
  if (packet.message_id != 403) {
    throw std::invalid_argument("Expected ReqJoinSession");
  }

  PacketReader reader(packet.payload);
  JoinRoomRequest request;
  request.room_id = reader.ReadUnsigned();
  request.value8 = reader.ReadUnsigned();
  request.value12 = reader.ReadUnsigned();
  request.password = reader.ReadString(kSessionStringLimit);
  request.value44 = reader.ReadUnsigned();
  reader.RequireEnd();
  return request;
}

void ValidateLeaveRoomRequest(const LambdaPacket& packet) {
  // Sender 0x705E60 writes C404 with the empty gcpReqLeaveSession serializer.
  if (packet.message_id != 404 || !packet.payload.empty()) {
    throw std::invalid_argument("Expected empty ReqLeaveSession");
  }
}

std::u16string ReadRoomKickRequest(const LambdaPacket& packet) {
  // 0x60C6B0 sends C510; 0x9CCE90 writes one UTF-16 nickname.
  if (packet.message_id != 510) {
    throw std::invalid_argument("Expected ReqKickFromSession");
  }

  PacketReader reader(packet.payload);
  auto nickname = reader.ReadString(kSessionStringLimit);
  reader.RequireEnd();
  if (characters::CheckNickname(nickname) !=
      characters::CreationResult::kSuccess) {
    throw std::invalid_argument(
        "Room kick target must be a valid character nickname");
  }
  return nickname;
}

LambdaPacket CreateRoomKickReply(std::u16string_view requester,
                                 std::u16string_view target,
                                 RoomKickResult result) {
  // 0x9CCFA0 / 0x88F4D0: requester, target, result. Room dispatcher 0x87EFE0
  // uses S513.
  PacketWriter writer;
  writer.WriteString(requester);
  writer.WriteString(target);
  writer.WriteUnsigned(static_cast<std::uint32_t>(result));
  return {513, writer.GetBytes()};
}

LambdaPacket CreateRoomResult(bool creating, std::uint32_t result) {
  PacketWriter writer;
  writer.WriteUnsigned(result);
  return {static_cast<std::uint16_t>(creating ? 102 : 103), writer.GetBytes()};
}

RoomReadyStatus ReadRoomReadyStatus(const LambdaPacket& packet) {
  if (packet.message_id != 505) {
    throw std::invalid_argument("Expected RoomReqReadyStatus");
  }
  PacketReader reader(packet.payload);
  RoomReadyStatus status;
  status.ready = reader.ReadBoolean();
  status.activity_flags[0] = reader.ReadBoolean();
  status.activity_flags[1] = reader.ReadBoolean();
  reader.RequireEnd();
  // Original 0x6099D0 emits one of four mutually exclusive activity states.
  if (static_cast<int>(status.ready) +
          static_cast<int>(status.activity_flags[0]) +
          static_cast<int>(status.activity_flags[1]) >
      1) {
    throw std::invalid_argument(
        "Room activity indicators are mutually exclusive");
  }
  return status;
}

void ValidateRoomTeamRequest(const LambdaPacket& packet) {
  // gcpRoomChangeTeam has an empty serializer; sender 0xA6FF10 writes C515.
  if (packet.message_id != 515 || !packet.payload.empty()) {
    throw std::invalid_argument("Expected empty RoomChangeTeam");
  }
}

RoomGameSettingRequest ReadRoomGameSettingRequest(const LambdaPacket& packet) {
  if (packet.message_id != 513) {
    throw std::invalid_argument("Expected RoomChangeGameSetting");
  }

  // C513 serializer 0x60CE70 is a request structure, not a full GameSetting.
  PacketReader reader(packet.payload);
  RoomGameSettingRequest request;
  for (auto& value : request.strings4) {
    value = reader.ReadString(kSessionStringLimit);
  }
  request.flag116 = reader.ReadBoolean();
  request.flag117 = reader.ReadBoolean();
  request.time_limit_milliseconds = reader.ReadUnsigned();
  request.score_limit = reader.ReadUnsigned();
  for (auto& value : request.values128) {
    value = reader.ReadUnsigned();
  }
  request.mode = reader.ReadUnsigned();
  request.sub_mode = reader.ReadUnsigned();
  request.flag160 = reader.ReadBoolean();
  reader.RequireEnd();
  return request;
}

LambdaPacket CreateRoomEntryReply(const SessionSetting& setting,
                                  std::uint32_t master_slot,
                                  std::uint32_t own_slot,
                                  std::span<const SessionMember> members) {
  if (members.empty() || members.size() > 32 || master_slot >= members.size() ||
      own_slot >= members.size() || !members[master_slot].character ||
      !members[own_slot].character) {
    throw std::invalid_argument(
        "Room entry requires valid master and own occupied slots");
  }

  PacketWriter writer;
  WriteSessionSetting(writer, setting);
  writer.WriteUnsigned(
      master_slot);  // Packet +600, passed to 0x6E49E0, the S502 master setter.
  writer.WriteUnsigned(
      own_slot);  // Packet +604, GameSession constructor parameter at 0x87E810.
  writer.WriteUnsigned(static_cast<std::uint32_t>(members.size()));
  for (std::size_t slot = 0; slot < members.size(); ++slot) {
    WriteSessionMember(writer, members[slot], static_cast<std::uint32_t>(slot));
  }
  writer.WriteUnsigned(static_cast<std::uint32_t>(std::count_if(
      members.begin(), members.end(), [](const SessionMember& member) {
        return member.character.has_value();
      })));
  for (const auto& member : members) {
    if (member.character) {
      writer.WriteBytes(CreateEquipmentReply(member.character->account_id,
                                             member.character->character_type,
                                             member.items)
                            .payload);
    }
  }
  writer.WriteUnsigned(0);  // Packet +640: preserve session title.
  writer.WriteUnsigned(0);  // Packet +644: session timer.
  return {110, writer.GetBytes()};
}

LambdaPacket CreateRoomSettingReply(const SessionSetting& setting) {
  PacketWriter writer;
  WriteSessionSetting(writer, setting);
  return {501, writer.GetBytes()};
}

LambdaPacket CreateRoomGameSettingReply(const GameSetting& setting) {
  // S505 serializes the complete GameSetting via vtable 0xE91E30 / 0x890F20.
  PacketWriter writer;
  WriteGameSetting(writer, setting);
  return {505, writer.GetBytes()};
}

LambdaPacket CreateRoomMasterReply(std::uint32_t slot) {
  if (slot >= 32) {
    throw std::invalid_argument(
        "Master slot exceeds the original UserSlot array");
  }
  PacketWriter writer;
  writer.WriteUnsigned(slot);
  return {502, writer.GetBytes()};
}

LambdaPacket CreateRoomSlotReply(std::uint32_t slot,
                                 const SessionMember& member) {
  if (slot >= 32 || !member.character) {
    throw std::invalid_argument(
        "RoomSetSlot requires a supported occupied slot");
  }
  PacketWriter writer;
  writer.WriteUnsigned(slot);
  WriteSessionMember(writer, member, slot);
  writer.WriteBytes(CreateEquipmentReply(member.character->account_id,
                                         member.character->character_type,
                                         member.items)
                        .payload);
  return {503, writer.GetBytes()};
}

LambdaPacket CreateRoomDepartureReply(std::uint32_t slot) {
  if (slot >= 32) {
    throw std::invalid_argument(
        "Departing slot exceeds the original UserSlot array");
  }
  PacketWriter writer;
  writer.WriteUnsigned(slot);
  WriteSessionMember(writer, {},
                     slot);  // S511 contains the new, empty UserSlot
                             // (0x881320), not an equipment list.
  return {511, writer.GetBytes()};
}

LambdaPacket CreateRoomTeamReply(std::uint32_t slot, std::uint8_t team) {
  if (slot >= 32 || (team != 1 && team != 2)) {
    throw std::invalid_argument(
        "RoomChangeTeam requires a valid slot and supported team");
  }
  PacketWriter writer;
  writer.WriteUnsigned(slot);
  writer.WriteUnsigned(team);
  return {504, writer.GetBytes()};
}
}  // namespace bubblefighter::protocol
