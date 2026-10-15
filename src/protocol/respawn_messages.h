#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_RESPAWN_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_RESPAWN_MESSAGES_H_

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
struct RespawnRequest {
  std::uint32_t object_id = 0;
  std::uint32_t value =
      0;  // C713/P32904: requested delay; C704/P32925: spawn index.
};

RespawnRequest ReadRespawnRequest(const LambdaPacket& packet);
LambdaPacket CreateRespawnOffer(std::uint32_t object_id,
                                std::uint32_t spawn_index);
LambdaPacket CreateRespawnReply(std::uint32_t object_id,
                                std::uint32_t spawn_index);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_RESPAWN_MESSAGES_H_
