#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_GAME_START_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_GAME_START_MESSAGES_H_

#include <array>
#include <string>

#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
struct GameStartRequest {
  bool flag4 = false;
  std::array<std::u16string, 3> strings8;
  std::uint32_t difficulty = 0;
  std::array<std::uint32_t, 13> values100{};
  std::array<bool, 5> event_flags{};
  std::uint32_t value156 = 0;
};

enum class GameStartResult : std::uint32_t {
  kSuccess = 0,
  kNotReady = 1,
  kUnequalTeams = 2,
  kUnavailable = 3,
  kAlone = 4
};

GameStartRequest ReadGameStartRequest(const LambdaPacket& packet);
void ValidateMapLoadedRequest(const LambdaPacket& packet);
bool ReadRelayConnectionReport(const LambdaPacket& packet);
void ValidatePeerTableReport(const LambdaPacket& packet);
LambdaPacket CreateGameEntryReply(GameStartResult result,
                                  std::u16string_view map, std::uint32_t state);
LambdaPacket CreateSessionSignatureReply(std::uint32_t mode);
LambdaPacket CreatePeerIdentityReply(std::uint32_t peer_id);
LambdaPacket CreatePeerTableReply(std::uint32_t slot, std::uint32_t peer_id);
LambdaPacket CreateMapLoadedReply(std::uint32_t slot);
LambdaPacket CreateEverybodyLoadedReply();
LambdaPacket CreateGameCountdownReply(std::uint32_t milliseconds);
LambdaPacket CreateGameStartReply(std::uint32_t duration_milliseconds,
                                  std::uint32_t tracking_id);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_GAME_START_MESSAGES_H_
