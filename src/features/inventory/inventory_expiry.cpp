#include "features/inventory/inventory_expiry.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace bubblefighter::inventory {
bool InventoryExpiry::Observe(std::span<const InventoryItem> items,
                              Clock::time_point query_started) {
  std::optional<Clock::time_point> next;
  for (const auto& item : items) {
    if (!item.remaining_days) {
      continue;
    }
    if (!std::isfinite(*item.remaining_days) || *item.remaining_days < 0) {
      throw std::invalid_argument(
          "Inventory remaining duration must be finite and nonnegative");
    }
    // Recheck the database clock at least hourly. These are scheduling bounds,
    // not item durations.
    const auto seconds =
        std::clamp(*item.remaining_days, 1.0 / 86400.0, 1.0 / 24.0) * 86400.0;
    const auto deadline =
        query_started + std::chrono::ceil<Clock::duration>(
                            std::chrono::duration<double>(seconds));
    if (!next || deadline < *next) {
      next = deadline;
    }
  }

  const auto changed =
      !std::equal(items.begin(), items.end(), items_.begin(), items_.end(),
                  [](const auto& left, const auto& right) {
                    return left.item_id == right.item_id &&
                           left.quantity == right.quantity &&
                           left.equipped_slot == right.equipped_slot;
                  });
  items_.assign(items.begin(), items.end());
  next_check_ = next;
  return changed;
}

std::optional<InventoryExpiry::Clock::time_point> InventoryExpiry::NextCheck()
    const {
  return next_check_;
}
}  // namespace bubblefighter::inventory
