#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_INVENTORY_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_INVENTORY_MESSAGES_H_

#include <span>

#include "features/inventory/inventory_repository.h"
#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
std::uint32_t ReadEquipmentRequest(const LambdaPacket& packet);
std::uint8_t ReadWeaponPresetRequest(const LambdaPacket& packet);
LambdaPacket CreateWeaponPresetResult(std::uint8_t index, bool success);
LambdaPacket CreateWeaponPresetsReply(std::uint32_t identity,
                                      const inventory::WeaponPresets& presets);
LambdaPacket CreateEquipmentResult(bool equipped, bool success);
LambdaPacket CreateInventoryReply(
    std::span<const inventory::InventoryItem> items);
LambdaPacket CreateEquipmentReply(
    std::uint32_t identity, std::uint32_t character_type,
    std::span<const inventory::InventoryItem> items);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_INVENTORY_MESSAGES_H_
