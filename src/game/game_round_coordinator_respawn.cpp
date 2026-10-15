#include <spdlog/spdlog.h>

#include <algorithm>

#include "game/game_round_coordinator.h"
#include "protocol/health_report_messages.h"
#include "protocol/respawn_messages.h"

namespace bubblefighter::game {
void GameRoundCoordinator::HandleRespawnReport(
    std::uint64_t session_id, const protocol::LambdaPacket& packet) {
  const auto request = protocol::ReadRespawnRequest(packet);
  const auto round = Find(session_id);
  if (!round || round->phase != Phase::kPlaying) {
    return;
  }

  for (const auto& member : round->members) {
    if (member.identity.owner_session_id != session_id) {
      continue;
    }

    const auto slot = member.identity.slot;
    if (packet.message_id == 713) {
      // Requested delay is telemetry. The server uses the character and
      // equipment frozen at round start.
      round->life.RequestRespawn(slot, request.object_id);
      TryRespawn(round, slot);
    } else if (round->life.ConfirmRespawn(slot, request.object_id,
                                          request.value)) {
      // Every observer clears this character's in-round items; only its owner
      // emits P32925.
      Broadcast(*round,
                protocol::CreateRespawnReply(request.object_id, request.value));
    }
    return;
  }
}

void GameRoundCoordinator::TryRespawn(const std::shared_ptr<Round>& round,
                                      std::uint8_t slot) {
  if (!IsCurrent(round) || round->phase != Phase::kPlaying ||
      !round->life.BeginRespawn(slot)) {
    return;
  }

  const auto member = std::find_if(
      round->members.begin(), round->members.end(),
      [slot](const Member& value) { return value.identity.slot == slot; });
  if (member == round->members.end()) {
    return;
  }

  // 0x59BE10 clears the client's history on respawn. Request it during the
  // existing wait, without delaying play.
  if (member->health_trace.Request(HealthTraceReason::kRespawn)) {
    listener_.SendToOwner(member->identity.owner_session_id,
                          protocol::CreateHealthTraceRequest());
  }
  member->respawn_timer = std::make_shared<boost::asio::steady_timer>(context_);
  spdlog::debug("[Game] roomId={} slot={} respawnDelayMs={}",
                round->room.setting.room_id, slot,
                member->respawn_delay.count());
  member->respawn_timer->expires_after(member->respawn_delay);
  member->respawn_timer->async_wait(
      [this, round, slot](boost::system::error_code error) {
        if (error || !IsCurrent(round) || round->phase != Phase::kPlaying) {
          return;
        }

        for (auto& player : round->members) {
          if (player.identity.slot != slot) {
            continue;
          }

          // Verified map markers are T11..T18 and T21..T28. Cycling is a server
          // selection policy.
          const auto team = round->room.members[slot].profile.team;
          const auto spawn_index =
              team * 10u +
              (slot / 2u + player.respawn_count) % round->team_spawn_count + 1u;
          if (const auto object = round->life.OfferRespawn(slot, spawn_index)) {
            // Clear before S708: the client's new protection reports may
            // precede P32925 on the relay.
            player.damage_attributes.ClearForRespawn();
            round->projectiles.DiscardHitReports(player.identity.slot);
            ++player.respawn_count;
            notify_(player.identity.owner_session_id,
                    protocol::CreateRespawnOffer(*object, spawn_index));
          }
          return;
        }
      });
}

bool GameRoundCoordinator::HandleRespawnRelay(
    const std::shared_ptr<Round>& round, const Member& source,
    const protocol::LambdaPacket& packet) {
  if (packet.message_id != 32904 && packet.message_id != 32925) {
    return false;
  }

  const auto request = protocol::ReadRespawnRequest(packet);
  if (round->phase != Phase::kPlaying) {
    return true;
  }

  round->life.ValidateReporter(source.identity.slot, request.object_id);
  // P32904 mirrors C713 but is not an authorization request or a handled
  // original peer state message.
  if (packet.message_id == 32904 ||
      !round->life.CompleteRespawn(source.identity.slot, request.object_id,
                                   request.value)) {
    return true;
  }

  for (const auto& member : round->members) {
    if (member.identity.owner_session_id != source.identity.owner_session_id &&
        !source.direct_peers.contains(member.identity.peer_id)) {
      listener_.SendToOwner(member.identity.owner_session_id, packet);
    }
  }
  return true;
}
}  // namespace bubblefighter::game
