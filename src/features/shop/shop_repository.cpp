#include "features/shop/shop_repository.h"

#include <Windows.h>

#include <algorithm>
#include <boost/mysql/datetime.hpp>
#include <chrono>
#include <stdexcept>
#include <utility>

#include "features/inventory/inventory_repository.h"

namespace bubblefighter::shop {
namespace {
using boost::mysql::field;
using database::SqlCommand;

std::u16string DecodeName(boost::mysql::string_view value) {
  if (value.empty() || value.size() > 512) {
    throw std::runtime_error("Shop name is empty or too long");
  }

  const auto size = static_cast<int>(value.size());
  const auto count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                         value.data(), size, nullptr, 0);
  if (count == 0) {
    throw std::runtime_error("Shop name is not valid UTF-8");
  }

  std::wstring decoded(static_cast<std::size_t>(count), L'\0');
  if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.data(), size,
                          decoded.data(), count) != count) {
    throw std::runtime_error("Shop name conversion failed");
  }
  return std::u16string(decoded.begin(), decoded.end());
}

double ToDate(const boost::mysql::datetime& value) {
  return 25569.0 +
         std::chrono::duration<double>(value.as_time_point().time_since_epoch())
                 .count() /
             86400.0;
}
}  // namespace

ShopRepository::ShopRepository(database::MySqlDatabase& database)
    : database_(database) {}

boost::asio::awaitable<ShopCatalog> ShopRepository::ReadCatalog() {
  std::vector<SqlCommand> commands;
  commands.push_back(
      {"SELECT shop_items.item_id, item_definitions.name, shop_sub_categories.main_category_id, "
       "shop_items.sub_category_id, shop_items.display_order, shop_items.sale_starts_at, "
       "shop_items.sale_ends_at, shop_stocks.id, shop_stocks.duration_days, "
       "shop_stocks.show_price, shop_stocks.sell_price, shop_permanent_stocks.stock_id, shop_preview_items.item_id "
       "FROM shop_items "
       "JOIN item_definitions ON item_definitions.id = shop_items.item_id "
       "JOIN shop_sub_categories ON shop_sub_categories.id = shop_items.sub_category_id "
       "JOIN shop_stocks ON shop_stocks.item_id = shop_items.item_id "
       "LEFT JOIN shop_permanent_stocks ON shop_permanent_stocks.stock_id = shop_stocks.id "
       "LEFT JOIN shop_preview_items ON shop_preview_items.item_id = shop_items.item_id "
       "WHERE shop_items.is_on_sale AND shop_stocks.is_on_sale "
       "AND UTC_TIMESTAMP(6) >= shop_items.sale_starts_at "
       "AND UTC_TIMESTAMP(6) < shop_items.sale_ends_at "
       "ORDER BY shop_items.display_order, shop_items.item_id, shop_stocks.duration_days, shop_stocks.id "
       "LIMIT 2049",
       {}});
  commands.push_back(
      {"SELECT shop_main_categories.id, shop_main_categories.name, shop_main_categories.display_order, "
       "shop_sub_categories.id, shop_sub_categories.name, shop_sub_categories.display_order, "
       "shop_sub_categories.page_type FROM shop_main_categories JOIN shop_sub_categories "
       "ON shop_sub_categories.main_category_id = shop_main_categories.id "
       "WHERE shop_sub_categories.is_visible "
       "ORDER BY shop_main_categories.display_order, shop_main_categories.id, "
       "shop_sub_categories.display_order, shop_sub_categories.id LIMIT 129",
       {}});
  const auto results =
      co_await database_.ExecuteTransaction(std::move(commands));
  if (results[0].rows().size() > 2048 || results[1].rows().size() > 128) {
    throw std::runtime_error("Shop catalog exceeds supported capacity");
  }

  ShopCatalog catalog;
  for (const auto row : results[0].rows()) {
    const auto item_id = static_cast<std::uint32_t>(row[0].as_uint64());
    if (catalog.items.empty() || catalog.items.back().id != item_id) {
      catalog.items.push_back({item_id,
                               DecodeName(row[1].as_string()),
                               static_cast<std::uint32_t>(row[2].as_uint64()),
                               static_cast<std::uint32_t>(row[3].as_uint64()),
                               static_cast<std::uint32_t>(row[4].as_uint64()),
                               ToDate(row[5].as_datetime()),
                               ToDate(row[6].as_datetime()),
                               {},
                               !row[12].is_null()});
    }
    catalog.items.back().stocks.push_back(
        {static_cast<std::uint32_t>(row[7].as_uint64()), item_id,
         row[11].is_null() ? static_cast<std::uint32_t>(row[8].as_uint64())
                           : 0xFFFFFFFFu,
         static_cast<std::uint32_t>(row[9].as_uint64()),
         static_cast<std::uint32_t>(row[10].as_uint64()),
         static_cast<std::uint16_t>(row[11].is_null() ? 1 : 9)});
  }

  for (const auto row : results[1].rows()) {
    const auto category_id = static_cast<std::uint32_t>(row[0].as_uint64());
    if (catalog.categories.empty() ||
        catalog.categories.back().id != category_id) {
      catalog.categories.push_back(
          {category_id,
           DecodeName(row[1].as_string()),
           static_cast<std::uint32_t>(row[2].as_uint64()),
           {}});
    }
    catalog.categories.back().children.push_back(
        {static_cast<std::uint32_t>(row[3].as_uint64()),
         DecodeName(row[4].as_string()),
         static_cast<std::uint32_t>(row[5].as_uint64()),
         static_cast<std::uint32_t>(row[6].as_uint64())});
  }
  co_return catalog;
}

boost::asio::awaitable<PurchaseResult> ShopRepository::Purchase(
    std::uint32_t character_id, std::uint32_t stock_id) {
  PurchaseResult outcome = PurchaseResult::kUnavailable;
  co_await database_
      .ExecuteTransaction([character_id, stock_id, &outcome](
                              database::SqlTransaction& transaction) -> boost::
                                                                         asio::
                                                                             awaitable<void> {
                                                                               // Use the same
                                                                               // parent lock as
                                                                               // equipment
                                                                               // changes and
                                                                               // other purchases
                                                                               // for this
                                                                               // character.
                                                                               SqlCommand query{
                                                                                   "SELECT id FROM characters WHERE id = ? FOR UPDATE",
                                                                                   {field(
                                                                                       character_id)}};
                                                                               if ((co_await transaction
                                                                                        .Execute(
                                                                                            std::move(
                                                                                                query)))
                                                                                       .rows()
                                                                                       .empty()) {
                                                                                 co_return;
                                                                               }

                                                                               query = {
                                                                                   "SELECT shop_stocks.item_id, shop_stocks.duration_days, shop_stocks.sell_price, "
                                                                                   "shop_items.max_duration_days, shop_items.currency, shop_permanent_stocks.stock_id "
                                                                                   "FROM shop_stocks JOIN shop_items "
                                                                                   "ON shop_items.item_id = shop_stocks.item_id "
                                                                                   "LEFT JOIN shop_permanent_stocks ON shop_permanent_stocks.stock_id = shop_stocks.id "
                                                                                   "WHERE shop_stocks.id = ? AND shop_stocks.is_on_sale AND shop_items.is_on_sale "
                                                                                   "AND UTC_TIMESTAMP(6) >= shop_items.sale_starts_at "
                                                                                   "AND UTC_TIMESTAMP(6) < shop_items.sale_ends_at "
                                                                                   "AND NOT EXISTS (SELECT 1 FROM shop_preview_items "
                                                                                   "WHERE shop_preview_items.item_id = shop_items.item_id) FOR SHARE",
                                                                                   {field(
                                                                                       stock_id)}};
                                                                               const auto stock =
                                                                                   co_await transaction
                                                                                       .Execute(
                                                                                           std::move(
                                                                                               query));
                                                                               if (stock
                                                                                       .rows()
                                                                                       .empty()) {
                                                                                 co_return;
                                                                               }

                                                                               const auto item_id = static_cast<
                                                                                   std::
                                                                                       uint32_t>(
                                                                                   stock
                                                                                       .rows()
                                                                                           [0]
                                                                                           [0]
                                                                                       .as_uint64());
                                                                               const auto duration = std::chrono::
                                                                                   days(static_cast<
                                                                                        std::
                                                                                            int64_t>(
                                                                                       stock
                                                                                           .rows()
                                                                                               [0]
                                                                                               [1]
                                                                                           .as_uint64()));
                                                                               const auto price =
                                                                                   stock
                                                                                       .rows()
                                                                                           [0]
                                                                                           [2]
                                                                                       .as_uint64();
                                                                               const auto maximum = std::chrono::
                                                                                   days(static_cast<
                                                                                        std::
                                                                                            int64_t>(
                                                                                       stock
                                                                                           .rows()
                                                                                               [0]
                                                                                               [3]
                                                                                           .as_uint64()));
                                                                               const auto currency =
                                                                                   stock
                                                                                       .rows()
                                                                                           [0]
                                                                                           [4]
                                                                                       .as_string();
                                                                               const bool permanent =
                                                                                   !stock
                                                                                        .rows()
                                                                                            [0]
                                                                                            [5]
                                                                                        .is_null();
                                                                               if (permanent &&
                                                                                   item_id !=
                                                                                       inventory::
                                                                                           kWeaponPresetExpansionItem) {
                                                                                 co_return;  // Each permanent functional item needs its own implemented effect.
                                                                               }
                                                                               if (currency !=
                                                                                       "lucci" &&
                                                                                   currency !=
                                                                                       "cash_cn") {
                                                                                 co_return;
                                                                               }
                                                                               const bool cash =
                                                                                   currency ==
                                                                                   "cash_cn";
                                                                               query = {
                                                                                   "SELECT lucci, cash, UTC_TIMESTAMP(6) FROM character_stats WHERE character_id = ? FOR UPDATE",
                                                                                   {field(
                                                                                       character_id)},
                                                                                   {},
                                                                                   1};
                                                                               const auto wallet =
                                                                                   co_await transaction
                                                                                       .Execute(
                                                                                           std::move(
                                                                                               query));
                                                                               const auto now =
                                                                                   wallet
                                                                                       .rows()
                                                                                           [0]
                                                                                           [2]
                                                                                       .as_datetime()
                                                                                       .as_time_point();
                                                                               if (wallet
                                                                                       .rows()
                                                                                           [0]
                                                                                           [cash
                                                                                                ? 1
                                                                                                : 0]
                                                                                       .as_uint64() <
                                                                                   price) {
                                                                                 outcome =
                                                                                     cash
                                                                                         ? PurchaseResult::
                                                                                               kInsufficientCash
                                                                                         : PurchaseResult::
                                                                                               kInsufficientLucci;
                                                                                 co_return;
                                                                               }

                                                                               query = {
                                                                                   "SELECT character_items.id, character_item_expirations.expires_at FROM character_items "
                                                                                   "LEFT JOIN character_item_expirations "
                                                                                   "ON character_item_expirations.character_item_id = character_items.id "
                                                                                   "WHERE character_items.character_id = ? AND character_items.item_id = ? FOR UPDATE",
                                                                                   {field(
                                                                                        character_id),
                                                                                    field(
                                                                                        item_id)}};
                                                                               const auto owned =
                                                                                   co_await transaction
                                                                                       .Execute(
                                                                                           std::move(
                                                                                               query));
                                                                               auto expires_at =
                                                                                   now +
                                                                                   duration;
                                                                               std::uint64_t
                                                                                   ownership_id =
                                                                                       0;
                                                                               bool expired =
                                                                                   false;
                                                                               if (!owned
                                                                                        .rows()
                                                                                        .empty()) {
                                                                                 ownership_id =
                                                                                     owned
                                                                                         .rows()
                                                                                             [0]
                                                                                             [0]
                                                                                         .as_uint64();
                                                                                 if (owned
                                                                                         .rows()
                                                                                             [0]
                                                                                             [1]
                                                                                         .is_null()) {
                                                                                   outcome =
                                                                                       PurchaseResult::
                                                                                           kAlreadyPermanent;
                                                                                   co_return;
                                                                                 }
                                                                                 const auto previous =
                                                                                     owned
                                                                                         .rows()
                                                                                             [0]
                                                                                             [1]
                                                                                         .as_datetime()
                                                                                         .as_time_point();
                                                                                 expired =
                                                                                     previous <=
                                                                                     now;
                                                                                 expires_at =
                                                                                     std::max(
                                                                                         previous,
                                                                                         now) +
                                                                                     duration;
                                                                               }
                                                                               if (!permanent &&
                                                                                   expires_at >
                                                                                       now +
                                                                                           maximum) {
                                                                                 outcome =
                                                                                     PurchaseResult::
                                                                                         kDurationLimit;
                                                                                 co_return;
                                                                               }

                                                                               if (ownership_id ==
                                                                                       0 ||
                                                                                   expired) {
                                                                                 query = {
                                                                                     "SELECT COUNT(*) FROM character_items LEFT JOIN character_item_expirations "
                                                                                     "ON character_item_expirations.character_item_id = character_items.id "
                                                                                     "WHERE character_items.character_id = ? AND (character_item_expirations.expires_at IS NULL "
                                                                                     "OR character_item_expirations.expires_at > ?)",
                                                                                     {field(
                                                                                          character_id),
                                                                                      field(boost::mysql::datetime(
                                                                                          now))}};
                                                                                 if ((co_await transaction
                                                                                          .Execute(
                                                                                              std::move(
                                                                                                  query)))
                                                                                         .rows()
                                                                                             [0]
                                                                                             [0]
                                                                                         .as_int64() >=
                                                                                     static_cast<
                                                                                         std::
                                                                                             int64_t>(
                                                                                         inventory::
                                                                                             kMaximumInventoryItems)) {
                                                                                   outcome =
                                                                                       PurchaseResult::
                                                                                           kCapacityLimit;
                                                                                   co_return;
                                                                                 }
                                                                               }

                                                                               if (permanent) {
                                                                                 // Duplicate
                                                                                 // purchases are
                                                                                 // rejected by
                                                                                 // permanent
                                                                                 // ownership
                                                                                 // above, under
                                                                                 // the character
                                                                                 // lock.
                                                                                 query = {
                                                                                     "INSERT INTO character_weapon_presets (character_id, slot_count) VALUES (?, 3) "
                                                                                     "ON DUPLICATE KEY UPDATE slot_count = 3",
                                                                                     {field(
                                                                                         character_id)}};
                                                                                 co_await transaction
                                                                                     .Execute(
                                                                                         std::move(
                                                                                             query));
                                                                               }
                                                                               query = {
                                                                                   cash
                                                                                       ? "UPDATE character_stats SET cash = cash - ?, updated_at = UTC_TIMESTAMP(6) "
                                                                                         "WHERE character_id = ? AND cash >= ?"
                                                                                       : "UPDATE character_stats SET lucci = lucci - ?, updated_at = UTC_TIMESTAMP(6) "
                                                                                         "WHERE character_id = ? AND lucci >= ?",
                                                                                   {field(
                                                                                        price),
                                                                                    field(
                                                                                        character_id),
                                                                                    field(
                                                                                        price)},
                                                                                   1};
                                                                               co_await transaction
                                                                                   .Execute(
                                                                                       std::move(
                                                                                           query));
                                                                               if (ownership_id ==
                                                                                   0) {
                                                                                 query = {
                                                                                     "INSERT INTO character_items "
                                                                                     "(character_id, item_id, quantity, equipped_slot, created_at, updated_at) "
                                                                                     "VALUES (?, ?, 1, NULL, UTC_TIMESTAMP(6), UTC_TIMESTAMP(6))",
                                                                                     {field(
                                                                                          character_id),
                                                                                      field(
                                                                                          item_id)},
                                                                                     1};
                                                                                 ownership_id =
                                                                                     (co_await transaction
                                                                                          .Execute(
                                                                                              std::move(
                                                                                                  query)))
                                                                                         .last_insert_id();
                                                                                 query = {
                                                                                     "INSERT INTO character_item_expirations (character_item_id, expires_at) VALUES (?, ?)",
                                                                                     {field(
                                                                                          ownership_id),
                                                                                      field(boost::mysql::datetime(
                                                                                          expires_at))},
                                                                                     1};
                                                                               } else {
                                                                                 query = {
                                                                                     "UPDATE character_items SET equipped_slot = CASE WHEN ? THEN NULL ELSE equipped_slot END, "
                                                                                     "updated_at = UTC_TIMESTAMP(6) WHERE id = ?",
                                                                                     {field(
                                                                                          expired
                                                                                              ? 1
                                                                                              : 0),
                                                                                      field(
                                                                                          ownership_id)}};
                                                                                 co_await transaction
                                                                                     .Execute(
                                                                                         std::move(
                                                                                             query));
                                                                                 query = {
                                                                                     "UPDATE character_item_expirations SET expires_at = ? WHERE character_item_id = ?",
                                                                                     {field(boost::mysql::datetime(
                                                                                          expires_at)),
                                                                                      field(
                                                                                          ownership_id)},
                                                                                     1};
                                                                               }
                                                                               if (permanent) {
                                                                                 // Permanent
                                                                                 // items have no
                                                                                 // expiration
                                                                                 // row; preserve
                                                                                 // this
                                                                                 // distinction
                                                                                 // in the
                                                                                 // purchase
                                                                                 // audit too.
                                                                                 co_await transaction
                                                                                     .Execute(
                                                                                         {"DELETE FROM character_item_expirations WHERE character_item_id = ?",
                                                                                          {field(
                                                                                              ownership_id)}});
                                                                               } else {
                                                                                 co_await transaction
                                                                                     .Execute(
                                                                                         std::move(
                                                                                             query));
                                                                               }
                                                                               query = {
                                                                                   "INSERT INTO shop_purchases "
                                                                                   "(character_id, stock_id, lucci_spent, cash_spent, expires_at, created_at) "
                                                                                   "VALUES (?, ?, ?, ?, ?, UTC_TIMESTAMP(6))",
                                                                                   {field(
                                                                                        character_id),
                                                                                    field(
                                                                                        stock_id),
                                                                                    field(
                                                                                        cash
                                                                                            ? 0u
                                                                                            : price),
                                                                                    field(
                                                                                        cash
                                                                                            ? price
                                                                                            : 0u),
                                                                                    permanent
                                                                                        ? field(
                                                                                              nullptr)
                                                                                        : field(boost::mysql::datetime(
                                                                                              expires_at))},
                                                                                   1};
                                                                               co_await transaction
                                                                                   .Execute(
                                                                                       std::move(
                                                                                           query));
                                                                               outcome =
                                                                                   PurchaseResult::
                                                                                       kSuccess;
                                                                             });
  co_return outcome;
}
}  // namespace bubblefighter::shop
