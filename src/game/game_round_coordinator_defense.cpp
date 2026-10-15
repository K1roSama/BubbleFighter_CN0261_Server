#include <algorithm>

#include "game/game_round_coordinator.h"
#include "protocol/defense_seed_messages.h"

namespace bubblefighter::game {
void GameRoundCoordinator::SynchronizeDefenseSeeds(Round& round) {
  if (round.defense_seeds_sent || round.phase != Phase::kCountdown ||
      !std::all_of(
          round.members.begin(), round.members.end(),
          [](const Member& member) { return member.character_reported; })) {
    return;
  }
  // Every loaded client now has all actors. Send the same tables through each
  // ordered relay stream.
  for (const auto& recipient : round.members) {
    for (const auto& member : round.members) {
      listener_.SendToOwner(recipient.identity.owner_session_id,
                            protocol::CreateDefenseSeedReply(
                                member.character_object, member.defense_seeds));
      listener_.SendToOwner(recipient.identity.owner_session_id,
                            protocol::CreateCancelSeedReply(
                                member.character_object, member.defense_seeds));
    }
    // 0x53B990 / 0x53BA90 export the recipient's installed tables without
    // consuming their indices.
    listener_.SendToOwner(recipient.identity.owner_session_id, {11, {}});
    listener_.SendToOwner(recipient.identity.owner_session_id, {12, {}});
  }
  round.defense_seeds_sent = true;
}
}  // namespace bubblefighter::game
