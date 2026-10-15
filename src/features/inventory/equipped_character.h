#ifndef BUBBLEFIGHTER_SERVER_FEATURES_INVENTORY_EQUIPPED_CHARACTER_H_
#define BUBBLEFIGHTER_SERVER_FEATURES_INVENTORY_EQUIPPED_CHARACTER_H_

#include <span>

#include "features/inventory/inventory_repository.h"

namespace bubblefighter::inventory {
std::uint32_t ResolveEquippedCharacter(std::uint32_t initial_character_type,
                                       std::span<const InventoryItem> items);
}  // namespace bubblefighter::inventory

#endif  // BUBBLEFIGHTER_SERVER_FEATURES_INVENTORY_EQUIPPED_CHARACTER_H_
