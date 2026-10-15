#ifndef BUBBLEFIGHTER_SERVER_GAME_GAME_ROUND_COORDINATOR_H_
#define BUBBLEFIGHTER_SERVER_GAME_GAME_ROUND_COORDINATOR_H_

#include <boost/asio/ip/tcp.hpp>
#include <functional>
#include <optional>
#include <set>

#include "features/progression/progression_policy.h"
#include "game/battle_life.h"
#include "game/battle_projectiles.h"
#include "game/damage_attributes.h"
#include "game/defense_seed_tables.h"
#include "game/game_character_catalog.h"
#include "game/game_respawn_catalog.h"
#include "game/game_settlement.h"
#include "game/game_weapon_catalog.h"
#include "game/health_trace_tracker.h"
#include "game/room_directory.h"
#include "relay/relay_listener.h"

namespace bubblefighter::game {
struct GameRoundTiming {
  std::chrono::milliseconds loading_timeout{90000};
  // The original HUD floors remaining milliseconds / 1000 and displays 3, 2, 1,
  // then its start cue at 0.
  std::chrono::milliseconds countdown{4000};
  std::optional<std::chrono::milliseconds>
      respawn_delay_override;  // Test clock override; production uses the
                               // catalog.
  std::chrono::milliseconds end_announcement{
      3000};  // MapMeta EndGameTimeOverTick.
  std::chrono::milliseconds winner_announcement{
      6000};  // MapMeta EndGameNotifyWinOrLoseTickTeam.
  std::chrono::milliseconds result_display{
      11000};  // MapMeta EndGameNotifyResultTick.
};

// Owns the frozen roster and timers. Transport registration alone cannot
// advance a game.
class GameRoundCoordinator {
 public:
  using NotifyHandler =
      std::function<void(std::uint64_t, const protocol::LambdaPacket&)>;
  using AddressHandler = std::function<std::string(std::uint64_t)>;
  using SettlementHandler =
      std::function<boost::asio::awaitable<std::vector<SettledPlayer>>(
          CompletedGame)>;

  GameRoundCoordinator(boost::asio::io_context& context, RoomDirectory& rooms,
                       relay::RelayDirectory& directory,
                       relay::RelayListener& listener,
                       GameWeaponCatalog weapons, GameDefenseCatalog defense,
                       GameCharacterCatalog characters,
                       GameRespawnCatalog respawns,
                       progression::ProgressionPolicy progression,
                       boost::asio::ip::tcp::endpoint endpoint,
                       NotifyHandler notify, AddressHandler address,
                       SettlementHandler settle, GameRoundTiming timing = {});
  void Begin(std::uint64_t session_id,
             const protocol::GameStartRequest& request);
  void HandleReport(std::uint64_t session_id,
                    const protocol::LambdaPacket& packet);
  void HandleRelay(const relay::RelayMember& member,
                   const protocol::LambdaPacket& packet);
  void CancelForSession(std::uint64_t session_id,
                        std::uint64_t excluded_session = 0);
  void Stop();

 private:
  enum class Phase {
    kLoading,
    kCountdown,
    kPlaying,
    kSettling,
    kEnded,
    kWinnerAnnouncement,
    kResults
  };

  struct Member {
    relay::RelayMember identity;
    protocol::RelaySessionKey key{};
    bool loaded = false;
    bool relay_reported = false;
    bool table_reported = false;
    bool character_reported = false;
    std::uint32_t character_object = 0;
    std::array<GameWeapon, 4> weapons;
    std::uint32_t grenades_remaining = 0;
    DamageAttributes damage_attributes;
    DefenseProfile defense;
    CharacterHealthProfile health;
    std::chrono::milliseconds respawn_delay{};
    DefenseSeedTables defense_seeds;
    bool defense_confirmed = false;
    bool cancel_confirmed = false;
    HealthTraceTracker health_trace;
    std::set<std::uint32_t> direct_peers;
    std::shared_ptr<boost::asio::steady_timer> respawn_timer;
    std::uint32_t respawn_count = 0;
  };

  struct Round {
    explicit Round(boost::asio::io_context& context) : timer(context) {}

    RoomSnapshot room;
    std::vector<Member> members;
    BattleProjectiles projectiles;
    BattleLife life;
    std::array<protocol::PlayerBattleScore, kRoomCapacity> published_scores{};
    std::array<std::uint16_t, 2> team_scores{};
    boost::asio::steady_timer timer;
    std::uint32_t tracking_id = 0;
    std::uint8_t team_spawn_count = 0;
    CompletedGame result;
    unsigned int settlement_attempts = 0;
    bool defense_seeds_sent = false;
    Phase phase = Phase::kLoading;
  };

  std::shared_ptr<Round> Find(std::uint64_t session_id) const;
  void TryCountdown(const std::shared_ptr<Round>& round);
  void SynchronizeDefenseSeeds(Round& round);
  void LogLoadingStatus(const Round& round) const;
  void HandleRespawnReport(std::uint64_t session_id,
                           const protocol::LambdaPacket& packet);
  bool HandleRespawnRelay(const std::shared_ptr<Round>& round,
                          const Member& source,
                          const protocol::LambdaPacket& packet);
  void TryRespawn(const std::shared_ptr<Round>& round, std::uint8_t slot);
  void BroadcastScores(Round& round);
  void Finish(const std::shared_ptr<Round>& round, bool time_expired);
  void Settle(const std::shared_ptr<Round>& round);
  void PublishResult(const std::shared_ptr<Round>& round,
                     const std::vector<SettledPlayer>& settled);
  void AdvanceEndStage(const std::shared_ptr<Round>& round);
  void ScheduleEndStage(const std::shared_ptr<Round>& round,
                        std::chrono::milliseconds delay);
  void Cancel(const std::shared_ptr<Round>& round,
              std::uint64_t excluded_session, bool notify);
  void Broadcast(const Round& round, const protocol::LambdaPacket& packet,
                 std::uint64_t excluded_session = 0);
  bool IsCurrent(const std::shared_ptr<Round>& round) const;

  boost::asio::io_context& context_;
  RoomDirectory& rooms_;
  relay::RelayDirectory& directory_;
  relay::RelayListener& listener_;
  GameWeaponCatalog weapons_;
  GameDefenseCatalog defense_;
  GameCharacterCatalog characters_;
  GameRespawnCatalog respawns_;
  progression::ProgressionPolicy progression_;
  boost::asio::ip::tcp::endpoint endpoint_;
  NotifyHandler notify_;
  AddressHandler address_;
  SettlementHandler settle_;
  GameRoundTiming timing_;
  std::map<std::uint16_t, std::shared_ptr<Round>> rounds_;
  std::uint32_t next_peer_id_ = 1;
  std::uint32_t next_tracking_id_ = 1;
  bool stopped_ = false;
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_GAME_ROUND_COORDINATOR_H_
