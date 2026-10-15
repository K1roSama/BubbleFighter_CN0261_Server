#ifndef BUBBLEFIGHTER_SERVER_FEATURES_INVENTORY_INVENTORY_REPOSITORY_H_
#define BUBBLEFIGHTER_SERVER_FEATURES_INVENTORY_INVENTORY_REPOSITORY_H_

#include <array>
#include <cstdint>
#include <optional>
#include <vector>

#include "persistence/my_sql_database.h"

namespace bubblefighter::inventory {
// Internal database slot; S805 places this resource in its independent
// character field.
inline constexpr std::uint32_t kCharacterEquipmentSlot = 63;
// Server capacity policy. With empty attributes, 512 original 44-byte slots fit
// in one Lambda packet.
inline constexpr std::size_t kMaximumInventoryItems = 512;
inline constexpr std::uint32_t kWeaponPresetExpansionItem = 500001;

struct WeaponPresets {
  std::uint8_t selected = 0;
  std::uint8_t available_count = 2;
  std::array<std::uint32_t, 5> items{};
};

struct InventoryItem {
  std::uint32_t item_id = 0;
  std::uint32_t quantity = 0;
  std::uint32_t equipped_slot = 0;
  std::optional<double> remaining_days = std::nullopt;
};

class InventoryRepository {
 public:
  explicit InventoryRepository(database::MySqlDatabase& database);
  boost::asio::awaitable<std::vector<InventoryItem>> FindByCharacter(
      std::uint32_t character_id);
  boost::asio::awaitable<bool> SetEquipped(std::uint32_t character_id,
                                           std::uint32_t item_id,
                                           bool equipped);
  boost::asio::awaitable<WeaponPresets> ReadWeaponPresets(
      std::uint32_t character_id);
  boost::asio::awaitable<bool> SelectWeaponPreset(std::uint32_t character_id,
                                                  std::uint8_t index);

 private:
  database::MySqlDatabase& database_;
};
}  // namespace bubblefighter::inventory

#endif  // BUBBLEFIGHTER_SERVER_FEATURES_INVENTORY_INVENTORY_REPOSITORY_H_
