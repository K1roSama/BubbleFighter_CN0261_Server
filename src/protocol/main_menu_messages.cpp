#include "protocol/main_menu_messages.h"

#include <array>

#include "protocol/inventory_messages.h"
#include "protocol/room_messages.h"

namespace bubblefighter::protocol {
LambdaPacket CreateSoloPartyReply(
    const characters::CharacterProfile& character,
    std::span<const inventory::InventoryItem> items) {
  PacketWriter writer;
  // S201 / 0x9CD0C0. The plaza gate at 0x8980D0 requires state 2 and the local
  // account as leader.
  writer.WriteUnsigned(2);
  writer.WriteUnsigned(1);
  writer.WriteUnsigned(character.account_id);
  writer.WriteString(character.nickname);
  // Preserve the PartyMember constructor defaults for the three unconfirmed
  // metadata fields.
  writer.WriteUnsigned(0xFFFFFFFFu);
  writer.WriteUnsigned(0xFFFFFFFFu);
  writer.WriteUnsigned(0);
  writer.WriteUnsigned(1);
  writer.WriteBytes(CreateEquipmentReply(character.account_id,
                                         character.character_type, items)
                        .payload);
  return {201, writer.GetBytes()};
}

LambdaPacket CreateMainMenuReply(
    const characters::CharacterProfile& character,
    std::span<const inventory::InventoryItem> items) {
  SessionSetting setting;
  setting.kind = 1;  // S110 handler 0x8A6D50 selects the main menu stage.
  const std::array members{
      SessionMember{character, {items.begin(), items.end()}, 1}};
  return CreateRoomEntryReply(setting, 0, 0, members);
}
}  // namespace bubblefighter::protocol
