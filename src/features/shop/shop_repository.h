#ifndef BUBBLEFIGHTER_SERVER_FEATURES_SHOP_SHOP_REPOSITORY_H_
#define BUBBLEFIGHTER_SERVER_FEATURES_SHOP_SHOP_REPOSITORY_H_

#include <cstdint>
#include <string>
#include <vector>

#include "persistence/my_sql_database.h"

namespace bubblefighter::shop {
struct ShopStock {
  std::uint32_t id = 0;
  std::uint32_t item_id = 0;
  std::uint32_t duration_days = 0;
  std::uint32_t show_price = 0;
  std::uint32_t sell_price = 0;
  std::uint16_t type = 1;
};

struct ShopItem {
  std::uint32_t id = 0;
  std::u16string name;
  std::uint32_t main_category_id = 0;
  std::uint32_t sub_category_id = 0;
  std::uint32_t display_order = 0;
  double sale_starts_at = 0;
  double sale_ends_at = 0;
  std::vector<ShopStock> stocks;
  bool preview_only = false;
};

struct ShopSubCategory {
  std::uint32_t id = 0;
  std::u16string name;
  std::uint32_t display_order = 0;
  std::uint32_t page_type = 0;
};

struct ShopMainCategory {
  std::uint32_t id = 0;
  std::u16string name;
  std::uint32_t display_order = 0;
  std::vector<ShopSubCategory> children;
};

struct ShopCatalog {
  std::vector<ShopItem> items;
  std::vector<ShopMainCategory> categories;
};

enum class PurchaseResult : std::uint32_t {
  kUnavailable = 0,
  kSuccess = 1,
  kAlreadyPermanent = 11,
  kDurationLimit = 17,
  kCapacityLimit = 22,
  kInsufficientCash = 38,
  kInsufficientLucci = 0xFFFFFFF3
};

class ShopRepository {
 public:
  explicit ShopRepository(database::MySqlDatabase& database);
  boost::asio::awaitable<ShopCatalog> ReadCatalog();
  boost::asio::awaitable<PurchaseResult> Purchase(std::uint32_t character_id,
                                                  std::uint32_t stock_id);

 private:
  database::MySqlDatabase& database_;
};
}  // namespace bubblefighter::shop

#endif  // BUBBLEFIGHTER_SERVER_FEATURES_SHOP_SHOP_REPOSITORY_H_
