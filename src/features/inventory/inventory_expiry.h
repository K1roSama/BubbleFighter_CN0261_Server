#ifndef BUBBLEFIGHTER_SERVER_FEATURES_INVENTORY_INVENTORY_EXPIRY_H_
#define BUBBLEFIGHTER_SERVER_FEATURES_INVENTORY_INVENTORY_EXPIRY_H_

#include <chrono>
#include <span>

#include "features/inventory/inventory_repository.h"

namespace bubblefighter::inventory {
class InventoryExpiry {
 public:
  using Clock = std::chrono::steady_clock;

  bool Observe(std::span<const InventoryItem> items,
               Clock::time_point query_started);
  std::optional<Clock::time_point> NextCheck() const;

 private:
  std::vector<InventoryItem> items_;
  std::optional<Clock::time_point> next_check_;
};
}  // namespace bubblefighter::inventory

#endif  // BUBBLEFIGHTER_SERVER_FEATURES_INVENTORY_INVENTORY_EXPIRY_H_
