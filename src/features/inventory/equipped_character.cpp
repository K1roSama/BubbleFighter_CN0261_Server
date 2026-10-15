#include "features/inventory/equipped_character.h"

#include <stdexcept>

namespace bubblefighter::inventory {
std::uint32_t ResolveEquippedCharacter(std::uint32_t initial_character_type,
                                       std::span<const InventoryItem> items) {
  if (items.size() > kMaximumInventoryItems ||
      (initial_character_type != 1 && initial_character_type != 2)) {
    throw std::invalid_argument("Unsupported equipment profile");
  }

  auto character_type = initial_character_type;
  bool selected = false;
  for (const auto& item : items) {
    if (item.equipped_slot == kCharacterEquipmentSlot) {
      if (selected || item.quantity == 0 || item.item_id <= 300000 ||
          item.item_id > 301999) {
        throw std::invalid_argument("Invalid selected character appearance");
      }
      character_type = item.item_id - 300000;
      selected = true;
    }
  }
  return character_type;
}
}  // namespace bubblefighter::inventory
