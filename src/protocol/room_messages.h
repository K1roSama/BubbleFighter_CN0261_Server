#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_ROOM_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_ROOM_MESSAGES_H_

#include "protocol/lambda_packet_codec.h"
#include "protocol/session_member.h"
#include "protocol/session_setting.h"

namespace bubblefighter::protocol {
struct CreateRoomRequest {
  SessionSetting setting;
  std::u16string string600;
  bool flag628 = false;
};

struct JoinRoomRequest {
  std::uint32_t room_id;
  std::uint32_t value8;
  std::uint32_t value12;
  std::u16string password;
  std::uint32_t value44;
};

struct RoomReadyStatus {
  bool ready = false;
  std::array<bool, 2> activity_flags{};
};

enum class RoomKickResult : std::uint32_t {
  kSuccess = 0,
  kSelf = 2,
  kNotInRoom = 3,
  kUnavailable = 5
};

struct RoomGameSettingRequest {
  std::array<std::u16string, 4> strings4;
  bool flag116 = true;
  bool flag117 = true;
  std::uint32_t time_limit_milliseconds = 0;
  std::uint32_t score_limit = 0;
  std::array<std::uint32_t, 6> values128{};
  std::uint32_t mode = 1;
  std::uint32_t sub_mode = 0;
  bool flag160 = false;
};

CreateRoomRequest ReadCreateRoomRequest(const LambdaPacket& packet);
JoinRoomRequest ReadJoinRoomRequest(const LambdaPacket& packet);
std::u16string ReadRoomKickRequest(const LambdaPacket& packet);
LambdaPacket CreateRoomKickReply(std::u16string_view requester,
                                 std::u16string_view target,
                                 RoomKickResult result);
void ValidateLeaveRoomRequest(const LambdaPacket& packet);
RoomReadyStatus ReadRoomReadyStatus(const LambdaPacket& packet);
RoomGameSettingRequest ReadRoomGameSettingRequest(const LambdaPacket& packet);
void ValidateRoomTeamRequest(const LambdaPacket& packet);
LambdaPacket CreateRoomResult(bool creating, std::uint32_t result);
LambdaPacket CreateRoomEntryReply(const SessionSetting& setting,
                                  std::uint32_t master_slot,
                                  std::uint32_t own_slot,
                                  std::span<const SessionMember> members);
LambdaPacket CreateRoomSettingReply(const SessionSetting& setting);
LambdaPacket CreateRoomGameSettingReply(const GameSetting& setting);
LambdaPacket CreateRoomMasterReply(std::uint32_t slot);
LambdaPacket CreateRoomSlotReply(std::uint32_t slot,
                                 const SessionMember& member);
LambdaPacket CreateRoomDepartureReply(std::uint32_t slot);
LambdaPacket CreateRoomTeamReply(std::uint32_t slot, std::uint8_t team);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_ROOM_MESSAGES_H_
