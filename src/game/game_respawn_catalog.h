#ifndef BUBBLEFIGHTER_SERVER_GAME_GAME_RESPAWN_CATALOG_H_
#define BUBBLEFIGHTER_SERVER_GAME_GAME_RESPAWN_CATALOG_H_

#include <chrono>
#include <filesystem>
#include <map>
#include <span>
#include <string_view>

#include "features/inventory/inventory_repository.h"

namespace bubblefighter::game {
// Ordinary mode timing, derived from FighterConfig and unconditional
// ShopItemSpec attributes.
class GameRespawnCatalog {
 public:
  static GameRespawnCatalog Load(const std::filesystem::path& path);
  static GameRespawnCatalog Parse(std::string_view text);
  std::chrono::milliseconds Evaluate(
      std::uint32_t character_type,
      std::span<const inventory::InventoryItem> items) const;

 private:
  GameRespawnCatalog() = default;
  std::map<std::uint32_t, std::int64_t> characters_;
  std::map<std::uint32_t, std::int64_t> reductions_;
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_GAME_RESPAWN_CATALOG_H_
