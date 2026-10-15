#include "game/game_round_coordinator.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <stdexcept>
#include <utility>

#include "features/inventory/equipped_character.h"
#include "game/impact_damage.h"
#include "game/player_damage.h"
#include "protocol/damage_calculation_messages.h"
#include "protocol/defense_seed_messages.h"
#include "protocol/health_report_messages.h"
#include "protocol/peer_damage_messages.h"
#include "protocol/peer_fire_messages.h"
#include "protocol/peer_kick_messages.h"
#include "protocol/peer_life_messages.h"
#include "protocol/peer_messages.h"
#include "protocol/peer_near_messages.h"

namespace bubblefighter::game {
namespace {
// 0x5517C6 passes zero through 0x683300 to GoDamager +988; it is not the P32773
// attribute byte.
constexpr std::uint8_t kNearAttackFlags = 0;

std::array<std::uint32_t, 4> GetWeapons(const protocol::SessionMember& member) {
  // 0x530440/0x5306D0 supply defaults; 0x5BC9F0 converts ShopItem IDs to
  // WeaponSpec IDs.
  std::array<std::uint32_t, 4> weapons{2004, 1004, 7001, 0};
  for (const auto& item : member.items) {
    if (item.equipped_slot >= 18 && item.equipped_slot <= 21 &&
        item.quantity != 0 && item.item_id > 200000 && item.item_id < 300000) {
      weapons[item.equipped_slot - 18] = item.item_id - 200000;
    }
  }
  return weapons;
}

std::array<std::uint32_t, 4> GetWeapons(
    const std::array<GameWeapon, 4>& weapons) {
  return {weapons[0].definition_id, weapons[1].definition_id,
          weapons[2].definition_id, weapons[3].definition_id};
}
}  // namespace

GameRoundCoordinator::GameRoundCoordinator(
    boost::asio::io_context& context, RoomDirectory& rooms,
    relay::RelayDirectory& directory, relay::RelayListener& listener,
    GameWeaponCatalog weapons, GameDefenseCatalog defense,
    GameCharacterCatalog characters, GameRespawnCatalog respawns,
    progression::ProgressionPolicy progression,
    boost::asio::ip::tcp::endpoint endpoint, NotifyHandler notify,
    AddressHandler address, SettlementHandler settle, GameRoundTiming timing)
    : context_(context),
      rooms_(rooms),
      directory_(directory),
      listener_(listener),
      weapons_(std::move(weapons)),
      defense_(std::move(defense)),
      characters_(std::move(characters)),
      respawns_(std::move(respawns)),
      progression_(std::move(progression)),
      endpoint_(endpoint),
      notify_(std::move(notify)),
      address_(std::move(address)),
      settle_(std::move(settle)),
      timing_(timing) {
  if (!settle_ || timing_.loading_timeout.count() <= 0 ||
      timing_.loading_timeout > std::chrono::minutes(5) ||
      timing_.countdown.count() <= 0 || timing_.countdown.count() > 60000 ||
      (timing_.respawn_delay_override &&
       (timing_.respawn_delay_override->count() <= 0 ||
        timing_.respawn_delay_override->count() > 60000)) ||
      timing_.end_announcement.count() <= 0 ||
      timing_.end_announcement.count() > 60000 ||
      timing_.winner_announcement.count() <= 0 ||
      timing_.winner_announcement.count() > 60000 ||
      timing_.result_display.count() <= 0 ||
      timing_.result_display.count() > 60000) {
    throw std::invalid_argument("Game round timing is invalid");
  }
}

void GameRoundCoordinator::Begin(std::uint64_t session_id,
                                 const protocol::GameStartRequest& request) {
  const auto previous = rooms_.FindBySession(session_id);
  if (stopped_ || !previous) {
    return;
  }
  auto start =
      endpoint_.port() != 0 && endpoint_.address().is_v4()
          ? rooms_.BeginGame(session_id, request)
          : RoomGameStart{protocol::GameStartResult::kUnavailable, previous};
  if (start.result != protocol::GameStartResult::kSuccess) {
    notify_(session_id, protocol::CreateGameEntryReply(
                            start.result, previous->setting.game.strings16[1],
                            previous->setting.state));
    return;
  }

  auto round = std::make_shared<Round>(context_);
  round->room = *start.room;
  round->team_spawn_count = start.team_spawn_count;
  round->tracking_id = next_tracking_id_++;
  if (next_tracking_id_ == 0) {
    next_tracking_id_ = 1;
  }
  const auto loading_started = relay::RelayDirectory::Clock::now();
  const auto loading_deadline = loading_started + timing_.loading_timeout;
  try {
    round->result.id = CreateGameResultId();
    for (std::size_t slot = 0; slot < round->room.members.size(); ++slot) {
      const auto owner = round->room.members[slot].session_id;
      if (owner == 0) {
        continue;
      }
      if (next_peer_id_ == 0 || next_peer_id_ == 0xFFFFFFFFu) {
        next_peer_id_ = 1;
      }
      Member member;
      const auto& profile = round->room.members[slot].profile;
      if (!profile.character) {
        throw std::invalid_argument(
            "Occupied room member has no character profile");
      }
      const auto character_type = inventory::ResolveEquippedCharacter(
          profile.character->character_type, profile.items);
      member.health =
          characters_.Evaluate(character_type, round->room.channel_type,
                               round->room.setting.game.mode);
      member.respawn_delay = timing_.respawn_delay_override.value_or(
          respawns_.Evaluate(character_type, profile.items));
      const auto definitions = GetWeapons(round->room.members[slot].profile);
      for (std::size_t weapon_slot = 0; weapon_slot < definitions.size();
           ++weapon_slot) {
        if (weapon_slot == 3 && definitions[weapon_slot] == 0) {
          continue;
        }
        const auto* weapon = weapons_.Find(definitions[weapon_slot]);
        if (!weapon ||
            weapon->weapon_slot != static_cast<std::uint8_t>(weapon_slot)) {
          throw std::invalid_argument(
              "Equipped weapon has no supported definition in its original slot");
        }

        member.weapons[weapon_slot] = *weapon;
      }
      for (const auto& item : profile.items) {
        if (item.equipped_slot == 21 && item.item_id == 205003) {
          member.grenades_remaining = item.quantity;
        }
      }

      member.defense =
          defense_.Evaluate(round->room.members[slot].profile.items);
      member.defense_seeds = CreateDefenseSeedTables(member.defense);
      member.identity = {owner, round->room.setting.room_id,
                         static_cast<std::uint8_t>(slot), next_peer_id_++,
                         address_(owner)};
      const auto key = directory_.Reserve(member.identity, loading_started,
                                          timing_.loading_timeout);
      if (!key) {
        throw std::runtime_error("Could not reserve a relay identity");
      }
      member.key = *key;
      round->members.push_back(std::move(member));
    }
  } catch (const std::exception& error) {
    spdlog::warn("[Game] roomId={} start rejected: {}",
                 round->room.setting.room_id, error.what());
    for (const auto& member : round->members) {
      listener_.Revoke(member.identity.owner_session_id);
    }
    rooms_.SetGameState(round->room.setting.room_id, 2);
    notify_(session_id, protocol::CreateGameEntryReply(
                            protocol::GameStartResult::kUnavailable,
                            previous->setting.game.strings16[1], 2));
    return;
  }
  rounds_.emplace(round->room.setting.room_id, round);
  Broadcast(*round, protocol::CreateRoomSettingReply(round->room.setting));
  // Publish normal-entry slots before S510 creates all characters with the game
  // world.
  for (const auto& member : round->members) {
    const auto slot = member.identity.slot;
    Broadcast(*round, protocol::CreateRoomSlotReply(
                          slot, round->room.members[slot].profile));
  }
  Broadcast(*round, protocol::CreateSessionSignatureReply(
                        round->room.setting.game.mode));
  Broadcast(*round, protocol::CreateGameEntryReply(
                        protocol::GameStartResult::kSuccess,
                        round->room.setting.game.strings16[1], 3));
  for (const auto& member : round->members) {
    const auto owner = member.identity.owner_session_id;
    notify_(owner, protocol::CreatePeerIdentityReply(member.identity.peer_id));
    for (const auto& peer : round->members) {
      notify_(owner, protocol::CreatePeerTableReply(peer.identity.slot,
                                                    peer.identity.peer_id));
    }
    notify_(owner, {611, {}});
    notify_(owner, protocol::CreateRelayEndpointReply(
                       endpoint_.address().to_v4().to_bytes(), endpoint_.port(),
                       member.key));
  }
  round->timer.expires_at(loading_deadline);
  round->timer.async_wait([this, round](boost::system::error_code error) {
    if (!error && IsCurrent(round)) {
      spdlog::info("[Game] roomId={} loading timed out",
                   round->room.setting.room_id);
      LogLoadingStatus(*round);
      Cancel(round, 0, true);
    }
  });
  std::string map_name;
  for (const auto unit : round->room.setting.game.strings16[1]) {
    map_name.push_back(static_cast<char>(
        unit));  // Catalog identifiers are restricted to ASCII.
  }
  spdlog::info(
      "[Game] roomId={} loading members={} trackingId={} map={} random={}",
      round->room.setting.room_id, round->members.size(), round->tracking_id,
      map_name, round->room.setting.game.strings16[3] == u"Random");
}

std::shared_ptr<GameRoundCoordinator::Round> GameRoundCoordinator::Find(
    std::uint64_t session_id) const {
  for (const auto& [id, round] : rounds_) {
    static_cast<void>(id);
    for (const auto& member : round->members) {
      if (session_id != 0 && member.identity.owner_session_id == session_id) {
        return round;
      }
    }
  }
  return {};
}

bool GameRoundCoordinator::IsCurrent(
    const std::shared_ptr<Round>& round) const {
  const auto found = rounds_.find(round->room.setting.room_id);
  return !stopped_ && found != rounds_.end() && found->second == round;
}

void GameRoundCoordinator::HandleReport(std::uint64_t session_id,
                                        const protocol::LambdaPacket& packet) {
  if (packet.message_id == 713 || packet.message_id == 704) {
    HandleRespawnReport(session_id, packet);
    return;
  }
  if (packet.message_id == 2015 || packet.message_id == 2017) {
    // C2015/C2017 belong to intrusion synchronization. Normal initial worlds
    // already contain every slot. Do not relay unsolicited state snapshots or
    // wait for reports this path never generates.
    return;
  }
  bool relay_connected = false;
  if (packet.message_id == 701) {
    protocol::ValidateMapLoadedRequest(packet);
  } else if (packet.message_id == 601) {
    relay_connected = protocol::ReadRelayConnectionReport(packet);
  } else {
    protocol::ValidatePeerTableReport(packet);
  }
  const auto round = Find(session_id);
  if (!round || round->phase != Phase::kLoading) {
    return;
  }
  for (auto& member : round->members) {
    if (member.identity.owner_session_id != session_id) {
      continue;
    }
    if (packet.message_id == 701 && !member.loaded) {
      member.loaded = true;
      notify_(session_id, protocol::CreateMapLoadedReply(member.identity.slot));
      for (const auto& peer : round->members) {
        if (peer.loaded && peer.identity.owner_session_id != session_id) {
          notify_(peer.identity.owner_session_id,
                  protocol::CreateMapLoadedReply(member.identity.slot));
          notify_(session_id,
                  protocol::CreateMapLoadedReply(peer.identity.slot));
        }
      }
    } else if (packet.message_id == 601) {
      if (!relay_connected || !directory_.FindOwnerConnection(session_id)) {
        Cancel(round, 0, true);
        return;
      }
      member.relay_reported = true;
    } else if (packet.message_id == 607 && !member.table_reported) {
      member.table_reported = true;
      notify_(session_id, {608, {}});
    }
    spdlog::debug(
        "[Game] roomId={} session={} slot={} loadingReport={} mapLoaded={} relayReported={} tableReported={}",
        round->room.setting.room_id, session_id, member.identity.slot,
        packet.message_id, member.loaded, member.relay_reported,
        member.table_reported);
    break;
  }
  TryCountdown(round);
}

void GameRoundCoordinator::LogLoadingStatus(const Round& round) const {
  for (const auto& member : round.members) {
    const auto registered =
        directory_.FindOwnerConnection(member.identity.owner_session_id)
            .has_value();
    if (!member.loaded || !member.relay_reported || !registered ||
        !member.table_reported) {
      spdlog::info(
          "[Game] roomId={} session={} slot={} loadingIncomplete mapLoaded={} relayReported={} "
          "relayRegistered={} tableReported={}",
          round.room.setting.room_id, member.identity.owner_session_id,
          member.identity.slot, member.loaded, member.relay_reported,
          registered, member.table_reported);
    }
  }
}

void GameRoundCoordinator::TryCountdown(const std::shared_ptr<Round>& round) {
  if (round->phase != Phase::kLoading ||
      !std::all_of(round->members.begin(), round->members.end(),
                   [this](const Member& member) {
                     return member.loaded && member.relay_reported &&
                            member.table_reported &&
                            directory_
                                .FindOwnerConnection(
                                    member.identity.owner_session_id)
                                .has_value();
                   })) {
    return;
  }
  round->phase = Phase::kCountdown;
  spdlog::info("[Game] roomId={} normalStart countdownMs={} intrusionStates=0",
               round->room.setting.room_id, timing_.countdown.count());
  Broadcast(*round, protocol::CreateEverybodyLoadedReply());
  Broadcast(*round, protocol::CreateGameCountdownReply(
                        static_cast<std::uint32_t>(timing_.countdown.count())));
  round->timer.expires_after(timing_.countdown);
  round->timer.async_wait([this, round](boost::system::error_code error) {
    if (error || !IsCurrent(round)) {
      return;
    }
    if (!std::all_of(round->members.begin(), round->members.end(),
                     [](const Member& member) {
                       return member.character_reported &&
                              member.defense_confirmed &&
                              member.cancel_confirmed;
                     })) {
      spdlog::info(
          "[Game] roomId={} countdown ended without every character and defense seed confirmation",
          round->room.setting.room_id);
      Cancel(round, 0, true);
      return;
    }
    const auto room = rooms_.SetGameState(round->room.setting.room_id, 4);
    if (!room) {
      Cancel(round, 0, true);
      return;
    }
    round->room = *room;
    round->phase = Phase::kPlaying;
    for (auto& member : round->members) {
      member.damage_attributes.BeginPlay();
    }
    const auto duration = round->room.setting.game.values128[2];
    Broadcast(*round,
              protocol::CreateGameStartReply(duration, round->tracking_id));
    spdlog::info("[Game] roomId={} started trackingId={}",
                 round->room.setting.room_id, round->tracking_id);
    round->timer.expires_after(std::chrono::milliseconds(duration));
    round->timer.async_wait(
        [this, round](boost::system::error_code finish_error) {
          if (!finish_error && IsCurrent(round)) {
            Finish(round, true);
          }
        });
  });
}

void GameRoundCoordinator::HandleRelay(const relay::RelayMember& identity,
                                       const protocol::LambdaPacket& packet) {
  const auto round = Find(identity.owner_session_id);
  if (!round || round->room.setting.room_id != identity.room_id) {
    throw std::invalid_argument("Relay member has no active game");
  }
  const auto source = std::find_if(
      round->members.begin(), round->members.end(),
      [&identity](const Member& member) {
        return member.identity.owner_session_id == identity.owner_session_id &&
               member.identity.peer_id == identity.peer_id;
      });
  if (source == round->members.end() ||
      source->identity.slot != identity.slot) {
    throw std::invalid_argument(
        "Relay identity does not match the frozen roster");
  }
  if (packet.message_id == 32840) {
    // GameWorld 0x559B00 consumes only its own slot's echoed tick. Never
    // broadcast this probe.
    const auto reply = protocol::CreatePeerPingReply(packet, identity.slot);
    if (listener_.SendToOwner(identity.owner_session_id, reply)) {
      spdlog::debug("[Relay] roomId={} slot={} peerPing echoed",
                    identity.room_id, identity.slot);
    }
    return;
  }
  if (packet.message_id == 18) {
    // This reply can arrive during settlement/results. It never changes the
    // already frozen outcome.
    if (!source->health_trace.HasPending()) {
      return;
    }

    const auto entries = protocol::ReadHealthTraceReport(packet);
    const auto receipt = source->health_trace.Complete();
    spdlog::debug(
        "[Game] roomId={} slot={} character={} healthTrace received count={} sequence={} reason={} "
        "requestedReset={} receivedReset={} crossedReset={}",
        identity.room_id, identity.slot, source->character_object,
        entries.size(), receipt.sequence,
        receipt.reason == HealthTraceReason::kRespawn ? "respawn" : "end",
        receipt.requested_reset, receipt.received_reset,
        receipt.requested_reset != receipt.received_reset);
    for (std::size_t index = 0; index < entries.size(); ++index) {
      const auto& entry = entries[index];
      spdlog::debug(
          "[Game] roomId={} slot={} healthTrace index={} kind={} source={} amount={} reportedHealth={}",
          identity.room_id, identity.slot, index, entry.kind, entry.source,
          entry.amount, entry.resulting_health);
    }
    if (source->health_trace.RequestQueuedEnd()) {
      listener_.SendToOwner(identity.owner_session_id,
                            protocol::CreateHealthTraceRequest());
    }
    return;
  }
  if (packet.message_id == 14) {
    const auto report = protocol::ReadHealthResetReport(packet, identity.slot);
    // The original actor can reset before C3. C3 must subsequently confirm the
    // same complete object.
    if (source->health_trace.RecordReset(report.character_object,
                                         report.tick)) {
      spdlog::debug(
          "[Game] roomId={} slot={} character={} healthReset reported tick={} reportedReset={} "
          "confirmed={} offeredRespawns={}",
          identity.room_id, identity.slot, report.character_object, report.tick,
          source->health_trace.GetReportedReset(), source->character_reported,
          source->respawn_count);
    }
    return;
  }
  if (round->phase == Phase::kSettling || round->phase == Phase::kEnded ||
      round->phase == Phase::kWinnerAnnouncement ||
      round->phase == Phase::kResults) {
    // In-flight peer traffic cannot change a frozen result or abort its
    // presentation.
    return;
  }
  if (packet.message_id == 3) {
    if (round->phase != Phase::kCountdown && round->phase != Phase::kPlaying) {
      throw std::invalid_argument(
          "Relay character report requires the game countdown");
    }
    // 0x8866C0 sends ChrPlayer +404 after S703; C2015 only carries an
    // allocation counter.
    const auto character_object = protocol::ReadRelayUseReport(
        packet, identity.slot, round->room.setting.game.strings16[1]);
    auto health_trace = source->health_trace;
    health_trace.Bind(character_object);
    source->damage_attributes.Bind(character_object);
    round->life.Bind(identity.slot, character_object,
                     round->room.members[identity.slot].profile.team);
    source->health_trace = health_trace;
    source->character_object = character_object;
    source->character_reported = true;
    SynchronizeDefenseSeeds(*round);
    return;
  }
  if (packet.message_id == 11) {
    const auto report =
        protocol::ReadDamageAttributeReport(packet, identity.slot);
    // The actor already exists before S703/C3. Stage early reports until C3
    // confirms that exact object.
    if (source->damage_attributes.Apply(report)) {
      spdlog::debug(
          "[Game] roomId={} slot={} character={} damageAttribute kind={} value={} tick={} confirmed={} "
          "reportedFlags={}",
          identity.room_id, identity.slot, report.character_object, report.kind,
          report.value, report.tick, source->character_reported,
          source->character_reported ? source->damage_attributes.GetFlags()
                                     : 0);
    }
    return;
  }
  if (packet.message_id == 13) {
    const auto report =
        protocol::ReadDamageCalculationReport(packet, identity.slot);
    if (round->phase != Phase::kPlaying) {
      return;
    }
    round->life.ValidateReporter(identity.slot, report.target_object);
    const auto attack =
        round->projectiles.ConfirmHit(identity.slot, report.projectile_id);
    if (attack) {
      const auto defense = source->defense_seeds.defense[report.defense_index];
      const auto cancelled =
          (source->defense_seeds.critical_cancel[report.cancel_index / 32] >>
           (report.cancel_index % 32)) &
          1u;
      std::optional<ImpactDamage> impact;
      std::optional<bool> reported_protection;
      const auto attacker = std::find_if(
          round->members.begin(), round->members.end(),
          [&report](const Member& member) {
            return member.identity.slot == report.projectile_id / 100000;
          });
      if (attacker != round->members.end()) {
        const auto weapon = std::find_if(
            attacker->weapons.begin(), attacker->weapons.end(),
            [&attack](const GameWeapon& value) {
              return value.definition_id == attack->weapon_definition;
            });
        if (weapon != attacker->weapons.end() && !weapon->grenade) {
          ImpactDamageContext context;
          context.room_mode = round->room.setting.game.mode;
          context.attack_flags = attack->flags;
          context.target_flags = report.target_flags;
          context.defense_percent = defense;
          context.critical_cancelled = cancelled != 0;
          context.back_reduction = source->defense.back_reduction;
          context.wall_reduction = source->defense.wall_reduction;
          // The verified ShopItemSpec has no backDamageIncRatio rows; its reset
          // value is zero. Shield activity remains unknown until its runtime
          // state is established.
          impact = CalculateImpactDamage(weapon->damage, context);
          reported_protection = HasPlayerDamageProtection(
              report.target_flags, weapon->damage.can_damage_stunned);
        }
      }
      spdlog::debug(
          "[Game] roomId={} victim={} projectile={} damageReport confirmed weapon={} attackFlags={} "
          "targetFlags={} tick={} defenseIndex={} defenseValue={} cancelIndex={} cancelValue={} "
          "backReduction={} wallReduction={} characterType={} baseHealth={} impactAvailable={} "
          "impactDamage={} impactCritical={} reportedProtection={}",
          identity.room_id, report.target_object, report.projectile_id,
          attack->weapon_definition,
          attack->flags ? static_cast<int>(*attack->flags) : -1,
          report.target_flags, report.tick, report.defense_index, defense,
          report.cancel_index, cancelled, source->defense.back_reduction,
          source->defense.wall_reduction, source->health.character_type,
          source->health.base_health, impact.has_value(),
          impact ? impact->health_damage : -1.0f,
          impact ? static_cast<int>(impact->critical_applied) : -1,
          reported_protection ? static_cast<int>(*reported_protection) : -1);
    }
    return;
  }
  if (packet.message_id == 16) {
    const auto report =
        protocol::ReadHealthRecoveryReport(packet, identity.slot);
    if (round->phase == Phase::kPlaying) {
      round->life.ValidateReporter(identity.slot, report.character_object);
      spdlog::debug(
          "[Game] roomId={} slot={} character={} healthRecovery reported kind={} tick={}",
          identity.room_id, identity.slot, report.character_object, report.kind,
          report.tick);
    }
    return;
  }
  if (packet.message_id == 20 || packet.message_id == 21) {
    if (!round->defense_seeds_sent || !source->character_reported) {
      throw std::invalid_argument(
          "Defense seed report was not requested for this game");
    }
    protocol::ValidateDefenseSeedReport(packet, source->defense_seeds);
    if (packet.message_id == 20) {
      source->defense_confirmed = true;
    } else {
      source->cancel_confirmed = true;
    }
    return;
  }
  if (packet.message_id == 17) {
    if (!packet.payload.empty() || identity.slot != round->room.master_slot ||
        round->phase != Phase::kPlaying) {
      throw std::invalid_argument("Unexpected relay game start report");
    }
    return;
  }
  if (packet.message_id == 8 || packet.message_id == 9) {
    const auto peer = protocol::ReadRelayPeerRequest(packet);
    if (peer == identity.peer_id ||
        !std::any_of(round->members.begin(), round->members.end(),
                     [peer](const Member& member) {
                       return member.identity.peer_id == peer;
                     })) {
      throw std::invalid_argument("Relay peer is outside the current game");
    }
    if (packet.message_id == 8) {
      source->direct_peers.erase(peer);
    } else {
      source->direct_peers.insert(peer);
    }
    return;
  }
  const auto action = protocol::IsPeerAction(packet.message_id);
  if (HandleRespawnRelay(round, *source, packet)) {
    return;
  }
  const auto fire = protocol::IsPeerFire(packet.message_id);
  const auto near_attack_message = packet.message_id == 32773;
  const auto hit = packet.message_id == 32777;
  const auto explosion_message = protocol::IsPeerExplosion(packet.message_id);
  const auto removal = packet.message_id == 32906;
  const auto detonation_message = packet.message_id == 32900;
  const auto life = protocol::IsPeerLife(packet.message_id);
  const auto life_point = packet.message_id == 32920;
  const auto kick = protocol::IsPeerKick(packet.message_id);
  if (!action && !fire && !near_attack_message && !hit && !explosion_message &&
      !removal && !detonation_message && !life && !life_point && !kick &&
      packet.message_id != 32884 && packet.message_id != 32885 &&
      packet.message_id != 32922) {
    spdlog::debug("[Relay] roomId={} messageId={} awaiting peer handler",
                  identity.room_id, packet.message_id);
    return;
  }
  std::optional<std::uint32_t> destination;
  std::optional<std::uint32_t> source_object;
  std::optional<std::uint32_t> watching_target;
  std::optional<protocol::PeerFire> shot;
  std::optional<protocol::PeerNearAttack> near_attack;
  std::uint8_t projectile_count = 0;
  std::optional<protocol::PeerHit> collision;
  std::optional<protocol::PeerExplosion> explosion;
  std::optional<protocol::PeerDetonation> detonation;
  std::optional<std::uint32_t> removed_id;
  std::optional<protocol::PeerLifeEvent> life_event;
  std::optional<protocol::PeerLifePoint> life_point_report;
  std::optional<protocol::PeerKickEvent> kick_event;
  if (action || fire) {
    const auto weapons = GetWeapons(source->weapons);
    if (fire) {
      shot = protocol::ReadPeerFire(packet, identity.slot, weapons);
      source_object = shot->source_object;
      const auto target_slot = shot->target_slot;
      if (target_slot &&
          !std::any_of(round->members.begin(), round->members.end(),
                       [target_slot](const Member& member) {
                         return member.identity.slot == *target_slot;
                       })) {
        throw std::invalid_argument("Fire target is not a member of this game");
      }
      const auto& weapon = source->weapons[shot->weapon_slot];
      if (weapon.projectile_count == 0) {
        throw std::invalid_argument(
            "Fire requires a supported gun or grenade in its original weapon slot");
      }
      projectile_count = weapon.projectile_count;
    } else {
      source_object =
          protocol::ReadPeerActionObject(packet, identity.slot, weapons);
    }
  } else if (near_attack_message) {
    const auto weapons = GetWeapons(source->weapons);
    near_attack =
        protocol::ReadPeerNearAttack(packet, identity.slot, weapons[2]);
    const auto& weapon = source->weapons[2];
    if (weapon.projectile_count != 0) {
      throw std::invalid_argument(
          "Near attack requires the frozen ordinary near weapon");
    }
  } else if (hit) {
    collision = protocol::ReadPeerHit(packet);
    if (!std::any_of(round->members.begin(), round->members.end(),
                     [&collision](const Member& member) {
                       return member.identity.slot == collision->target_slot;
                     })) {
      throw std::invalid_argument("Hit target is not a member of this game");
    }
    if (collision->projectile_id / 100000 != identity.slot) {
      throw std::invalid_argument(
          "Hit projectile does not belong to the authenticated room slot");
    }
  } else if (explosion_message) {
    explosion = protocol::ReadPeerExplosion(packet);
    if (explosion->projectile_id / 100000 != identity.slot) {
      throw std::invalid_argument(
          "Exploded projectile does not belong to the authenticated room slot");
    }
    if (explosion->target_slot &&
        !std::any_of(round->members.begin(), round->members.end(),
                     [&explosion](const Member& member) {
                       return member.identity.slot == *explosion->target_slot;
                     })) {
      throw std::invalid_argument(
          "Explosion target is not a member of this game");
    }
  } else if (detonation_message) {
    detonation = protocol::ReadPeerDetonation(packet);
    if (detonation->projectile_id / 100000 != identity.slot) {
      throw std::invalid_argument(
          "Detonated grenade does not belong to the authenticated room slot");
    }
  } else if (removal) {
    removed_id = protocol::ReadPeerProjectileRemoval(packet);
    if (*removed_id / 100000 != identity.slot) {
      throw std::invalid_argument(
          "Removed projectile does not belong to the authenticated room slot");
    }
  } else if (kick) {
    kick_event = protocol::ReadPeerKick(packet);
  } else if (life_point) {
    life_point_report = protocol::ReadPeerLifePoint(packet, identity.slot);
    source_object = life_point_report->character_object;
  } else if (life) {
    life_event = protocol::ReadPeerLife(packet);
    if (life_event->kind == protocol::PeerLifeKind::kKnocked ||
        life_event->kind == protocol::PeerLifeKind::kStunned) {
      const auto attacker_slot = (life_event->actor_object - 1000000) / 100000;
      const auto attacker =
          std::find_if(round->members.begin(), round->members.end(),
                       [attacker_slot](const Member& member) {
                         return member.identity.slot == attacker_slot;
                       });
      if (attacker == round->members.end() ||
          std::none_of(attacker->weapons.begin(), attacker->weapons.end(),
                       [&life_event](const GameWeapon& weapon) {
                         return weapon.definition_id != 0 &&
                                weapon.definition_id ==
                                    life_event->weapon_definition;
                       })) {
        throw std::invalid_argument(
            "Knockdown requires a supported weapon in the attacker's frozen inventory");
      }
      if (life_event->kind == protocol::PeerLifeKind::kStunned &&
          !attacker->weapons[3].grenade) {
        throw std::invalid_argument(
            "Stun requires the verified equipped grenade");
      }
      if (life_event->kind == protocol::PeerLifeKind::kStunned &&
          life_event->weapon_definition != 5003) {
        throw std::invalid_argument(
            "Only the verified E02 stun flow is supported");
      }
    }
  } else {
    const auto movement = protocol::ReadPeerMovement(packet, identity.slot);
    source_object = movement.source_object;
    watching_target = movement.target_object;
    destination = movement.destination;
  }
  if (destination && !std::any_of(round->members.begin(), round->members.end(),
                                  [destination](const Member& member) {
                                    return member.identity.peer_id ==
                                           *destination;
                                  })) {
    throw std::invalid_argument(
        "Movement destination is outside the current game");
  }
  if (round->phase != Phase::kPlaying &&
      !(round->phase == Phase::kCountdown && packet.message_id == 32810)) {
    return;
  }
  if (near_attack) {
    round->life.ValidateReporter(identity.slot, near_attack->source_object);
  }
  if (source_object) {
    round->life.ValidateReporter(identity.slot, *source_object);
  }
  if (shot && shot->target_object) {
    round->life.ValidateReporter(*shot->target_slot, *shot->target_object);
  }
  if (collision) {
    // A partition identifies a seat, not the character currently bound to that
    // seat.
    round->life.ValidateReporter(collision->target_slot,
                                 collision->target_object);
  }
  if (explosion && explosion->target_object) {
    round->life.ValidateReporter(*explosion->target_slot,
                                 *explosion->target_object);
  }
  if (watching_target) {
    const auto target_slot =
        static_cast<std::uint8_t>((*watching_target - 1000000) / 100000);
    round->life.ValidateReporter(target_slot, *watching_target);
    if (!std::any_of(round->members.begin(), round->members.end(),
                     [target_slot, destination](const Member& member) {
                       return member.identity.slot == target_slot &&
                              member.identity.peer_id == *destination;
                     })) {
      throw std::invalid_argument(
          "Watching target does not match its destination peer");
    }
  }
  const auto grenade_shot = shot && source->weapons[shot->weapon_slot].grenade;
  if (grenade_shot && source->grenades_remaining == 0) {
    return;
  }
  const auto self_grenade = life_event &&
                            life_event->weapon_definition == 5003 &&
                            source->weapons[3].grenade;
  if ((life_event &&
       !round->life.Apply(identity.slot, *life_event, self_grenade)) ||
      (kick_event && !round->life.ApplyKick(identity.slot, *kick_event)) ||
      ((shot || near_attack) && !round->life.CanFire(identity.slot)) ||
      (packet.message_id == 32813 &&
       !round->life.ReturnToNormal(identity.slot))) {
    return;
  }
  if ((shot &&
       !round->projectiles.Register(
           identity.slot, shot->first_projectile_id, projectile_count, false,
           {shot->weapon_definition, shot->attack_flags,
            grenade_shot ? ProjectileKind::kGrenade
                         : ProjectileKind::kOrdinary})) ||
      (near_attack &&
       !round->projectiles.Register(
           identity.slot, near_attack->damager_id, 1, true,
           {near_attack->weapon_definition, kNearAttackFlags})) ||
      (collision &&
       !round->projectiles.Hit(identity.slot, collision->projectile_id,
                               collision->target_slot,
                               !round->life.AreOpponents(
                                   identity.slot, collision->target_object))) ||
      (explosion && !round->projectiles.Explode(
                        identity.slot, explosion->projectile_id,
                        explosion->target_slot, explosion->apply_damage,
                        explosion->target_object &&
                            !round->life.AreOpponents(
                                identity.slot, *explosion->target_object))) ||
      (detonation &&
       !round->projectiles.Detonate(identity.slot, detonation->projectile_id,
                                    detonation->splash_id)) ||
      (removed_id && !round->projectiles.Remove(identity.slot, *removed_id))) {
    return;
  }
  if (grenade_shot) {
    --source->grenades_remaining;
    spdlog::debug(
        "[Game] roomId={} slot={} grenadeThrown projectile={} remainingRoundQuantity={}",
        identity.room_id, identity.slot, shot->first_projectile_id,
        source->grenades_remaining);
  }
  if (detonation) {
    spdlog::debug(
        "[Game] roomId={} slot={} grenadeDetonated projectile={} splash={} shellRemoved=true",
        identity.room_id, identity.slot, detonation->projectile_id,
        detonation->splash_id);
  }
  if (removed_id) {
    spdlog::debug("[Game] roomId={} slot={} damagerRemoved id={}",
                  identity.room_id, identity.slot, *removed_id);
  }
  if (explosion) {
    spdlog::debug(
        "[Game] roomId={} slot={} bulletExplosion projectile={} target={} applyDamage={}",
        identity.room_id, identity.slot, explosion->projectile_id,
        explosion->target_object.value_or(0xFFFFFFFFu),
        explosion->apply_damage);
  }
  if (life_point_report) {
    // Original ordinary rooms use victim authority. This is peer presentation,
    // not server HP or score input.
    spdlog::debug(
        "[Game] roomId={} slot={} character={} lifePoint reported remaining={} effect={}",
        identity.room_id, identity.slot, life_point_report->character_object,
        life_point_report->remaining_points,
        static_cast<std::uint32_t>(life_point_report->effect));
  }
  if (packet.message_id == 32813) {
    spdlog::debug(
        "[Game] roomId={} slot={} returnToNormal character={} accepted",
        identity.room_id, identity.slot, *source_object);
  }
  if (life_event || kick_event) {
    spdlog::debug("[Game] roomId={} slot={} lifeEvent messageId={} accepted",
                  identity.room_id, identity.slot, packet.message_id);
  }
  for (const auto& member : round->members) {
    if (member.identity.owner_session_id != identity.owner_session_id &&
        !source->direct_peers.contains(member.identity.peer_id) &&
        (!destination || member.identity.peer_id == *destination)) {
      listener_.SendToOwner(member.identity.owner_session_id, packet);
    }
  }
  if (life_event || kick_event) {
    BroadcastScores(*round);
    const auto limit = round->room.setting.game.values128[0];
    if (round->team_scores[0] >= limit || round->team_scores[1] >= limit) {
      Finish(round, false);
    } else {
      TryRespawn(round, identity.slot);
    }
  }
}

void GameRoundCoordinator::BroadcastScores(Round& round) {
  std::array<std::uint32_t, 2> team_scores{};
  for (const auto& member : round.members) {
    const auto slot = member.identity.slot;
    const auto& score = round.life.GetScore(slot);
    // Current ordinary scoring policy: one confirmed enemy Pang contributes one
    // team point.
    team_scores[round.room.members[slot].profile.team - 1] += score.pang_count;
    if (score != round.published_scores[slot]) {
      const auto award = progression_.EvaluateResult(
          score.bubble_count, score.pang_count, score.rescue_count);
      Broadcast(round,
                protocol::CreatePlayerScoreReply(slot, score, award.score));
      round.published_scores[slot] = score;
    }
  }
  for (std::size_t team = 0; team < team_scores.size(); ++team) {
    const auto score =
        static_cast<std::uint16_t>(std::min(team_scores[team], 200u));
    if (score != round.team_scores[team]) {
      round.team_scores[team] = score;
      Broadcast(round, protocol::CreateTeamScoreReply(
                           static_cast<std::uint8_t>(team + 1), score));
    }
  }
}

void GameRoundCoordinator::Broadcast(const Round& round,
                                     const protocol::LambdaPacket& packet,
                                     std::uint64_t excluded_session) {
  for (const auto& member : round.members) {
    if (member.identity.owner_session_id != excluded_session) {
      notify_(member.identity.owner_session_id, packet);
    }
  }
}

void GameRoundCoordinator::Cancel(const std::shared_ptr<Round>& round,
                                  std::uint64_t excluded_session, bool notify) {
  rounds_.erase(round->room.setting.room_id);
  round->timer.cancel();
  for (const auto& member : round->members) {
    if (member.respawn_timer) {
      member.respawn_timer->cancel();
    }
    listener_.Revoke(member.identity.owner_session_id);
  }
  const auto room = rooms_.SetGameState(round->room.setting.room_id, 2);
  if (notify && room) {
    Broadcast(*round, {736, {}}, excluded_session);
    Broadcast(*round, protocol::CreateRoomSettingReply(room->setting),
              excluded_session);
    for (const auto& member : round->members) {
      const auto slot = member.identity.slot;
      Broadcast(
          *round,
          protocol::CreateRoomSlotReply(slot, room->members[slot].profile),
          excluded_session);
    }
  }
}

void GameRoundCoordinator::CancelForSession(std::uint64_t session_id,
                                            std::uint64_t excluded_session) {
  if (const auto round = Find(session_id)) {
    Cancel(round, excluded_session, true);
  }
}

void GameRoundCoordinator::Stop() {
  stopped_ = true;
  while (!rounds_.empty()) {
    // Retain ownership across erase; never pass a reference to the map's
    // shared_ptr.
    const auto round = rounds_.begin()->second;
    Cancel(round, 0, false);
  }
}
}  // namespace bubblefighter::game
