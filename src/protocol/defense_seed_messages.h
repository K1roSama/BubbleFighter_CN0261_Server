#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_DEFENSE_SEED_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_DEFENSE_SEED_MESSAGES_H_

#include "game/defense_seed_tables.h"
#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
LambdaPacket CreateDefenseSeedReply(std::uint32_t character_object,
                                    const game::DefenseSeedTables& tables);
LambdaPacket CreateCancelSeedReply(std::uint32_t character_object,
                                   const game::DefenseSeedTables& tables);
void ValidateDefenseSeedReport(const LambdaPacket& packet,
                               const game::DefenseSeedTables& tables);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_DEFENSE_SEED_MESSAGES_H_
