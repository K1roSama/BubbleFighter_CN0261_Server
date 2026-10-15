#include <spdlog/spdlog.h>

#include <algorithm>
#include <boost/asio/cancel_after.hpp>
#include <boost/asio/co_spawn.hpp>
#include <stdexcept>

#include "game/game_round_coordinator.h"
#include "protocol/game_end_messages.h"
#include "protocol/game_result_messages.h"
#include "protocol/health_report_messages.h"

namespace bubblefighter::game {
void GameRoundCoordinator::Finish(const std::shared_ptr<Round>& round,
                                  bool time_expired) {
  if (!IsCurrent(round) || round->phase != Phase::kPlaying) {
    return;
  }

  const auto room = rooms_.SetGameState(round->room.setting.room_id, 5);
  if (!room) {
    Cancel(round, 0, true);
    return;
  }

  round->room = *room;
  round->phase = Phase::kSettling;
  round->timer.cancel();
  for (auto& member : round->members) {
    if (member.respawn_timer) {
      member.respawn_timer->cancel();
    }
    if (member.character_reported &&
        member.health_trace.Request(HealthTraceReason::kEnd)) {
      // Defer behind an outstanding respawn snapshot. Missing diagnostics never
      // delay settlement.
      listener_.SendToOwner(member.identity.owner_session_id,
                            protocol::CreateHealthTraceRequest());
    }
  }

  auto& result = round->result;
  result.room_id = round->room.setting.room_id;
  result.tracking_id = round->tracking_id;
  result.mode = round->room.setting.game.mode;
  result.sub_mode = round->room.setting.game.sub_mode;
  result.reason = time_expired ? 2 : 1;
  result.team_scores = round->team_scores;
  for (const auto unit : round->room.setting.game.strings16[1]) {
    if (unit > 127) {
      throw std::logic_error("Configured map identifier must be ASCII");
    }
    result.map_name.push_back(static_cast<char>(unit));
  }
  for (const auto& member : round->members) {
    const auto slot = member.identity.slot;
    const auto& profile = round->room.members[slot].profile;
    result.players.push_back({slot, *profile.character, profile.team,
                              round->published_scores[slot]});
  }
  Settle(round);
}

void GameRoundCoordinator::Settle(const std::shared_ptr<Round>& round) {
  ++round->settlement_attempts;
  boost::asio::co_spawn(
      context_, settle_(round->result),
      boost::asio::cancel_after(
          std::chrono::seconds(15),
          [this, round](std::exception_ptr failure,
                        std::vector<SettledPlayer> settled) {
            if (!IsCurrent(round)) {
              return;
            }
            if (!failure) {
              try {
                PublishResult(round, settled);
                return;
              } catch (...) {
                failure = std::current_exception();
              }
            }
            spdlog::error("[Game] resultId={} settlement failed attempt={}",
                          round->result.id, round->settlement_attempts);
            if (round->settlement_attempts >= 3) {
              Cancel(round, 0, true);
              return;
            }
            round->timer.expires_after(std::chrono::milliseconds(200));
            round->timer.async_wait(
                [this, round](boost::system::error_code error) {
                  if (!error && IsCurrent(round)) {
                    Settle(round);
                  }
                });
          }));
}

void GameRoundCoordinator::PublishResult(
    const std::shared_ptr<Round>& round,
    const std::vector<SettledPlayer>& settled) {
  if (settled.size() != round->result.players.size()) {
    throw std::runtime_error("Settlement returned an incomplete roster");
  }
  std::vector<protocol::GameResultPlayer> players;
  for (const auto& participant : round->result.players) {
    const auto saved =
        std::find_if(settled.begin(), settled.end(),
                     [&participant](const SettledPlayer& value) {
                       return value.slot == participant.slot &&
                              value.character_id == participant.character.id;
                     });
    if (saved == settled.end()) {
      throw std::runtime_error(
          "Settlement returned an inconsistent player identity");
    }
    players.push_back({participant.slot, participant.character.account_id,
                       participant.character.nickname, participant.team,
                       participant.score, saved->stats.progress,
                       saved->experience_gain, saved->lucci_gain,
                       saved->level_up, saved->award});
  }
  const auto packet =
      protocol::CreateGameResultReply(round->team_scores, players);
  round->phase = Phase::kEnded;
  // S705 reads ScoreData to determine the winner. Every client must receive the
  // frozen snapshot first.
  Broadcast(*round, packet);
  Broadcast(*round, protocol::CreateGameEndReply(round->result.reason == 2));
  spdlog::info(
      "[Game] roomId={} ended trackingId={} reason={} firstTeamScore={} secondTeamScore={}",
      round->room.setting.room_id, round->tracking_id,
      round->result.reason == 2 ? "time" : "score", round->team_scores[0],
      round->team_scores[1]);
  ScheduleEndStage(round, timing_.end_announcement);
}

void GameRoundCoordinator::ScheduleEndStage(const std::shared_ptr<Round>& round,
                                            std::chrono::milliseconds delay) {
  round->timer.expires_after(delay);
  round->timer.async_wait([this, round](boost::system::error_code error) {
    if (!error && IsCurrent(round)) {
      AdvanceEndStage(round);
    }
  });
}

void GameRoundCoordinator::AdvanceEndStage(
    const std::shared_ptr<Round>& round) {
  if (round->phase == Phase::kEnded) {
    const auto room = rooms_.SetGameState(round->room.setting.room_id, 6);
    if (!room) {
      Cancel(round, 0, true);
      return;
    }

    round->room = *room;
    round->phase = Phase::kWinnerAnnouncement;
    Broadcast(*round, protocol::CreateGameWinnerReply(round->team_scores[0],
                                                      round->team_scores[1]));
    spdlog::info("[Game] roomId={} trackingId={} endStage=winner messageId=732",
                 round->room.setting.room_id, round->tracking_id);
    ScheduleEndStage(round, timing_.winner_announcement);
  } else if (round->phase == Phase::kWinnerAnnouncement) {
    round->phase = Phase::kResults;
    Broadcast(*round, {733, {}});  // 0x87EFE0 case 0x2DD calls 0x5ED270 without
                                   // deserializing a body.
    spdlog::info(
        "[Game] roomId={} trackingId={} endStage=results messageId=733",
        round->room.setting.room_id, round->tracking_id);
    ScheduleEndStage(round, timing_.result_display);
  } else if (round->phase == Phase::kResults) {
    // Returning to the waiting room resets readiness and revokes the old relay
    // keys.
    spdlog::info("[Game] roomId={} trackingId={} endStage=return messageId=736",
                 round->room.setting.room_id, round->tracking_id);
    Cancel(round, 0, true);
  }
}
}  // namespace bubblefighter::game
