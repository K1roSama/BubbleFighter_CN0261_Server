#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_MAIN_MENU_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_MAIN_MENU_MESSAGES_H_

#include <span>

#include "features/characters/character_repository.h"
#include "features/inventory/inventory_repository.h"
#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
LambdaPacket CreateSoloPartyReply(
    const characters::CharacterProfile& character,
    std::span<const inventory::InventoryItem> items);
LambdaPacket CreateMainMenuReply(
    const characters::CharacterProfile& character,
    std::span<const inventory::InventoryItem> items);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_MAIN_MENU_MESSAGES_H_
