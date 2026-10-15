#include "game/game_session.h"
#include "protocol/session_ping_messages.h"

namespace bubblefighter::game {
void GameSession::HandleSessionPing(const protocol::LambdaPacket& packet) {
  if (packet.message_id == 517) {
    const auto slot = protocol::ReadSessionPingRequest(packet);
    if (const auto room = rooms_.RequestPing(session_id_, slot)) {
      BroadcastRoom(*room, protocol::CreateSessionPingRequest(slot));
    } else if (const auto plaza = plazas_.RequestPing(session_id_, slot)) {
      BroadcastPlaza(plaza->plaza, protocol::CreateSessionPingRequest(slot), 0,
                     true);
    }
    return;
  }

  const auto response = protocol::ReadSessionPingResponse(packet);
  auto requester = rooms_.CompletePing(session_id_, response.responder_slot,
                                       response.requester_slot);
  if (!requester) {
    requester = plazas_.CompletePing(session_id_, response.responder_slot,
                                     response.requester_slot);
  }
  if (requester) {
    notify_(*requester, protocol::CreateSessionPingReply(response));
  }
}
}  // namespace bubblefighter::game
