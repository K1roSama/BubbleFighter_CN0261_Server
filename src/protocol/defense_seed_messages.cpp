#include "protocol/defense_seed_messages.h"

#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
namespace {
void ValidateCharacter(std::uint32_t character_object) {
  if (character_object < 1000000 || character_object >= 1800000) {
    throw std::invalid_argument(
        "Defense seed owner is outside the supported player partitions");
  }
}
}  // namespace

LambdaPacket CreateDefenseSeedReply(std::uint32_t character_object,
                                    const game::DefenseSeedTables& tables) {
  ValidateCharacter(character_object);
  PacketWriter writer;
  writer.WriteUnsigned(character_object);
  writer.WriteUnsigned(static_cast<std::uint32_t>(tables.defense.size()));
  writer.WriteBytes(tables.defense);
  return {5, writer.GetBytes()};
}

LambdaPacket CreateCancelSeedReply(std::uint32_t character_object,
                                   const game::DefenseSeedTables& tables) {
  ValidateCharacter(character_object);
  PacketWriter writer;
  writer.WriteUnsigned(character_object);
  writer.WriteUnsigned(
      static_cast<std::uint32_t>(tables.critical_cancel.size()));
  for (const auto value : tables.critical_cancel) {
    writer.WriteUnsigned(value);
  }
  return {6, writer.GetBytes()};
}

void ValidateDefenseSeedReport(const LambdaPacket& packet,
                               const game::DefenseSeedTables& tables) {
  PacketReader reader(packet.payload);
  if (packet.message_id == 20) {
    if (reader.ReadUnsigned() != tables.defense.size()) {
      throw std::invalid_argument(
          "Defense seed report has an unexpected table length");
    }
    for (const auto expected : tables.defense) {
      if (reader.ReadByte() != expected) {
        throw std::invalid_argument(
            "Defense seed report does not match the installed table");
      }
    }
  } else if (packet.message_id == 21) {
    if (reader.ReadUnsigned() != tables.critical_cancel.size()) {
      throw std::invalid_argument(
          "Critical cancellation seed report has an unexpected table length");
    }
    for (const auto expected : tables.critical_cancel) {
      if (reader.ReadUnsigned() != expected) {
        throw std::invalid_argument(
            "Critical cancellation seed report does not match the installed table");
      }
    }
  } else {
    throw std::invalid_argument(
        "Expected defense or critical cancellation seed report");
  }
  reader.RequireEnd();
}
}  // namespace bubblefighter::protocol
