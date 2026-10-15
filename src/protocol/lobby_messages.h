#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_LOBBY_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_LOBBY_MESSAGES_H_

#include <array>
#include <cstdint>
#include <span>
#include <vector>

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
struct ChannelRequest {
  std::uint32_t channel_type = 0;
  std::uint32_t channel_index = 0;
};

struct MoveIntoRequest {
  std::uint32_t account_id = 0;
  bool enter_lobby = false;
};

struct SessionListRequest {
  std::uint16_t page = 0;
  std::uint16_t count = 0;
};

ChannelRequest ReadChannelRequest(const LambdaPacket& packet);
MoveIntoRequest ReadMoveIntoRequest(const LambdaPacket& packet);
SessionListRequest ReadSessionListRequest(const LambdaPacket& packet);
LambdaPacket CreateChannelReply(std::uint32_t result,
                                const std::array<std::uint8_t, 4>& address,
                                std::uint16_t port);
LambdaPacket CreateMoveIntoReply(bool success, std::uint32_t channel_type,
                                 std::uint32_t channel_index);
LambdaPacket CreateGotoLobbyReply();
LambdaPacket CreateSessionListReply(
    std::uint32_t page, bool has_more,
    std::span<const std::vector<std::uint8_t>> settings);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_LOBBY_MESSAGES_H_
