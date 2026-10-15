#ifndef BUBBLEFIGHTER_SERVER_GAME_GAME_DEFENSE_CATALOG_H_
#define BUBBLEFIGHTER_SERVER_GAME_GAME_DEFENSE_CATALOG_H_

#include <cstdint>
#include <filesystem>
#include <map>
#include <set>
#include <span>
#include <string_view>
#include <vector>

#include "features/inventory/inventory_repository.h"

namespace bubblefighter::game {
struct DefenseProfile {
  float minimum = 0.0f;
  float maximum = 0.0f;
  float critical_cancel_ratio = 0.0f;
  float back_reduction = 0.0f;
  float wall_reduction = 0.0f;
};

class GameDefenseCatalog {
 public:
  static GameDefenseCatalog Load(const std::filesystem::path& path);
  static GameDefenseCatalog Parse(std::string_view text);
  DefenseProfile Evaluate(
      std::span<const inventory::InventoryItem> items) const;

 private:
  enum class Attribute {
    kMinimum,
    kMaximum,
    kCriticalCancelRatio,
    kBackReduction,
    kWallReduction
  };

  struct Modifier {
    Attribute attribute;
    float ratio;
  };

  GameDefenseCatalog() = default;
  std::map<std::uint32_t, std::vector<Modifier>> items_;
  std::set<std::uint32_t> unsupported_items_;
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_GAME_DEFENSE_CATALOG_H_
