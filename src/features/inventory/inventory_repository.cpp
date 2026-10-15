#include "features/inventory/inventory_repository.h"

#include <boost/mysql/datetime.hpp>
#include <chrono>
#include <stdexcept>
#include <utility>

namespace bubblefighter::inventory {
namespace {
using boost::mysql::field;
using database::SqlCommand;

struct InventorySnapshotContext {
  std::uint32_t fallback_type;
  boost::mysql::datetime sampled_at;
};

boost::asio::awaitable<std::optional<InventorySnapshotContext>> LockSnapshot(
    database::SqlTransaction& transaction, std::uint32_t character_id) {
  SqlCommand query{
      "SELECT character_type FROM characters WHERE id = ? FOR UPDATE",
      {field(character_id)}};
  const auto character = co_await transaction.Execute(std::move(query));
  if (character.rows().empty()) {
    co_return std::nullopt;
  }
  const auto fallback_type =
      static_cast<std::uint32_t>(character.rows()[0][0].as_uint64());
  // Sample after acquiring the owner lock, then use one cutoff for every
  // eligibility decision in this transaction.
  const auto clock =
      co_await transaction.Execute({"SELECT UTC_TIMESTAMP(6)", {}, {}, 1});
  co_return InventorySnapshotContext{fallback_type,
                                     clock.rows()[0][0].as_datetime()};
}

boost::asio::awaitable<std::uint32_t> NormalizeAppearance(
    database::SqlTransaction& transaction, std::uint32_t character_id,
    const InventorySnapshotContext& snapshot) {
  SqlCommand query{
      "UPDATE character_items JOIN character_item_expirations "
      "ON character_item_expirations.character_item_id = character_items.id "
      "SET equipped_slot = NULL, updated_at = UTC_TIMESTAMP(6) "
      "WHERE character_id = ? AND equipped_slot IS NOT NULL AND expires_at <= ?",
      {field(character_id), field(snapshot.sampled_at)}};
  co_await transaction.Execute(std::move(query));
  query = {
      "SELECT character_appearances.character_type FROM character_items JOIN character_appearances "
      "ON character_appearances.item_id = character_items.item_id "
      "WHERE character_items.character_id = ? AND character_items.equipped_slot = 63",
      {field(character_id)}};
  const auto selected = co_await transaction.Execute(std::move(query));
  auto active_type = snapshot.fallback_type;
  if (selected.rows().empty()) {
    query = {
        "UPDATE character_items JOIN character_appearances "
        "ON character_appearances.item_id = character_items.item_id "
        "SET equipped_slot = 63, updated_at = UTC_TIMESTAMP(6) "
        "WHERE character_id = ? AND character_appearances.character_type = ?",
        {field(character_id), field(snapshot.fallback_type)},
        1};
    co_await transaction.Execute(std::move(query));
  } else {
    active_type = static_cast<std::uint32_t>(selected.rows()[0][0].as_uint64());
  }
  query = {
      "UPDATE character_items JOIN item_equipment_rules "
      "ON item_equipment_rules.item_id = character_items.item_id "
      "SET equipped_slot = NULL, updated_at = UTC_TIMESTAMP(6) "
      "WHERE character_id = ? AND equipped_slot IS NOT NULL AND equipped_slot <> 63 "
      "AND (item_equipment_rules.character_mask & (CAST(1 AS UNSIGNED) << (? - 1))) = 0",
      {field(character_id), field(active_type)}};
  co_await transaction.Execute(std::move(query));
  co_return active_type;
}

boost::asio::awaitable<WeaponPresets> NormalizeWeaponPresets(
    database::SqlTransaction& transaction, std::uint32_t character_id,
    const InventorySnapshotContext& snapshot) {
  SqlCommand query{
      "INSERT IGNORE INTO character_weapon_presets (character_id) VALUES (?)",
      {field(character_id)}};
  co_await transaction.Execute(std::move(query));
  const auto state = co_await transaction.Execute(
      {"SELECT active_index, slot_count FROM character_weapon_presets WHERE character_id = ?",
       {field(character_id)},
       {},
       1});
  WeaponPresets presets;
  presets.selected = static_cast<std::uint8_t>(state.rows()[0][0].as_uint64());
  presets.available_count =
      static_cast<std::uint8_t>(state.rows()[0][1].as_uint64());

  // Initialize each newly available setting from the current weapon; do not
  // grant another weapon.
  for (std::uint8_t index = 0; index < presets.available_count; ++index) {
    query = {
        "INSERT IGNORE INTO character_weapon_preset_items (character_id, preset_index, item_id) "
        "VALUES (?, ?, (SELECT item_id FROM character_items WHERE character_id = ? AND equipped_slot = 18))",
        {field(character_id), field(index), field(character_id)}};
    co_await transaction.Execute(std::move(query));
  }
  // The equipped inventory remains authoritative for the selected setting,
  // including unequip and online expiry.
  query = {
      "UPDATE character_weapon_preset_items SET item_id = "
      "(SELECT item_id FROM character_items WHERE character_id = ? AND equipped_slot = 18) "
      "WHERE character_id = ? AND preset_index = ?",
      {field(character_id), field(character_id), field(presets.selected)}};
  co_await transaction.Execute(std::move(query));
  query = {
      "UPDATE character_weapon_preset_items LEFT JOIN character_items "
      "ON character_items.character_id = character_weapon_preset_items.character_id "
      "AND character_items.item_id = character_weapon_preset_items.item_id "
      "LEFT JOIN character_item_expirations ON character_item_expirations.character_item_id = character_items.id "
      "LEFT JOIN item_definitions ON item_definitions.id = character_weapon_preset_items.item_id "
      "SET character_weapon_preset_items.item_id = NULL "
      "WHERE character_weapon_preset_items.character_id = ? AND (character_items.id IS NULL "
      "OR character_items.quantity = 0 OR item_definitions.equipment_slot <> 18 "
      "OR character_item_expirations.expires_at <= ?)",
      {field(character_id), field(snapshot.sampled_at)}};
  co_await transaction.Execute(std::move(query));
  const auto items = co_await transaction.Execute(
      {"SELECT preset_index, item_id FROM character_weapon_preset_items WHERE character_id = ?",
       {field(character_id)}});
  for (const auto row : items.rows()) {
    const auto index = row[0].as_uint64();
    if (index < presets.available_count && !row[1].is_null()) {
      presets.items[static_cast<std::size_t>(index)] =
          static_cast<std::uint32_t>(row[1].as_uint64());
    }
  }
  co_return presets;
}
}  // namespace

InventoryRepository::InventoryRepository(database::MySqlDatabase& database)
    : database_(database) {}

boost::asio::awaitable<WeaponPresets> InventoryRepository::ReadWeaponPresets(
    std::uint32_t character_id) {
  WeaponPresets presets;
  co_await database_.ExecuteTransaction(
      [character_id, &presets](database::SqlTransaction& transaction)
          -> boost::asio::awaitable<void> {
        const auto snapshot = co_await LockSnapshot(transaction, character_id);
        if (snapshot) {
          co_await NormalizeAppearance(transaction, character_id, *snapshot);
          presets = co_await NormalizeWeaponPresets(transaction, character_id,
                                                    *snapshot);
        }
      });
  co_return presets;
}

boost::asio::awaitable<bool> InventoryRepository::SelectWeaponPreset(
    std::uint32_t character_id, std::uint8_t index) {
  bool accepted = false;
  co_await database_.ExecuteTransaction([character_id, index,
                                         &accepted](database::SqlTransaction&
                                                        transaction)
                                            -> boost::asio::awaitable<void> {
    const auto snapshot = co_await LockSnapshot(transaction, character_id);
    if (!snapshot) {
      co_return;
    }
    const auto active_type =
        co_await NormalizeAppearance(transaction, character_id, *snapshot);
    const auto presets =
        co_await NormalizeWeaponPresets(transaction, character_id, *snapshot);
    if (index >= presets.available_count) {
      co_return;
    }
    const auto item_id = presets.items[index];
    if (item_id != 0) {
      SqlCommand query{
          "SELECT item_id FROM item_equipment_rules WHERE item_id = ? "
          "AND (character_mask & (CAST(1 AS UNSIGNED) << (? - 1))) <> 0",
          {field(item_id), field(active_type)}};
      if ((co_await transaction.Execute(std::move(query))).rows().empty()) {
        co_return;
      }
    }

    co_await transaction.Execute(
        {"UPDATE character_items SET equipped_slot = NULL, updated_at = UTC_TIMESTAMP(6) "
         "WHERE character_id = ? AND equipped_slot = 18",
         {field(character_id)}});
    if (item_id != 0) {
      co_await transaction.Execute(
          {"UPDATE character_items SET equipped_slot = 18, updated_at = UTC_TIMESTAMP(6) "
           "WHERE character_id = ? AND item_id = ?",
           {field(character_id), field(item_id)},
           1});
    }
    co_await transaction.Execute(
        {"UPDATE character_weapon_presets SET active_index = ? WHERE character_id = ?",
         {field(index), field(character_id)}});
    accepted = true;
  });
  co_return accepted;
}

boost::asio::awaitable<std::vector<InventoryItem>>
InventoryRepository::FindByCharacter(std::uint32_t character_id) {
  std::vector<InventoryItem> items;
  co_await database_.ExecuteTransaction([character_id,
                                         &items](database::SqlTransaction&
                                                     transaction)
                                            -> boost::asio::awaitable<void> {
    const auto snapshot = co_await LockSnapshot(transaction, character_id);
    if (!snapshot) {
      co_return;
    }
    co_await NormalizeAppearance(transaction, character_id, *snapshot);
    SqlCommand query{
        "SELECT item_id, quantity, equipped_slot, expires_at "
        "FROM character_items "
        "LEFT JOIN character_item_expirations "
        "ON character_item_expirations.character_item_id = character_items.id "
        "WHERE character_id = ? AND (expires_at IS NULL OR expires_at > ?) "
        "ORDER BY item_id LIMIT ?",
        {field(character_id), field(snapshot->sampled_at),
         field(static_cast<std::uint64_t>(kMaximumInventoryItems + 1))}};
    const auto result = co_await transaction.Execute(std::move(query));
    if (result.rows().size() > kMaximumInventoryItems) {
      throw std::runtime_error(
          "Character inventory exceeds the supported capacity");
    }
    for (const auto row : result.rows()) {
      InventoryItem item{static_cast<std::uint32_t>(row[0].as_uint64()),
                         static_cast<std::uint32_t>(row[1].as_uint64()),
                         row[2].is_null()
                             ? 0
                             : static_cast<std::uint32_t>(row[2].as_uint64())};
      if (!row[3].is_null()) {
        // Use the same database cutoff as normalization so the selected
        // appearance cannot disappear halfway.
        const auto remaining = row[3].as_datetime().as_time_point() -
                               snapshot->sampled_at.as_time_point();
        item.remaining_days =
            std::chrono::duration<double, std::ratio<86400>>(remaining).count();
      }

      items.push_back(item);
    }
  });
  co_return items;
}

boost::asio::awaitable<bool> InventoryRepository::SetEquipped(
    std::uint32_t character_id, std::uint32_t item_id, bool equipped) {
  bool accepted = false;
  co_await database_.ExecuteTransaction(
      [character_id, item_id, equipped, &accepted](database::SqlTransaction&
                                                       transaction) -> boost::
                                                                        asio::awaitable<void> {
                                                                          const auto snapshot =
                                                                              co_await LockSnapshot(
                                                                                  transaction,
                                                                                  character_id);
                                                                          if (!snapshot) {
                                                                            co_return;
                                                                          }
                                                                          const auto active_type =
                                                                              co_await NormalizeAppearance(
                                                                                  transaction,
                                                                                  character_id,
                                                                                  *snapshot);
                                                                          SqlCommand query{
                                                                              "SELECT item_definitions.equipment_slot, characters.character_type FROM character_items "
                                                                              "JOIN item_definitions ON item_definitions.id = character_items.item_id "
                                                                              "JOIN item_equipment_rules ON item_equipment_rules.item_id = character_items.item_id "
                                                                              "JOIN characters ON characters.id = character_items.character_id "
                                                                              "LEFT JOIN character_item_expirations "
                                                                              "ON character_item_expirations.character_item_id = character_items.id "
                                                                              "WHERE character_items.character_id = ? AND character_items.item_id = ? "
                                                                              "AND character_items.quantity > 0 AND item_definitions.equipment_slot > 0 "
                                                                              "AND (item_equipment_rules.character_mask & (CAST(1 AS UNSIGNED) << (? - 1))) <> 0 "
                                                                              "AND (character_item_expirations.expires_at IS NULL "
                                                                              "OR character_item_expirations.expires_at > ?) FOR UPDATE",
                                                                              {field(
                                                                                   character_id),
                                                                               field(
                                                                                   item_id),
                                                                               field(
                                                                                   active_type),
                                                                               field(
                                                                                   snapshot
                                                                                       ->sampled_at)}};
                                                                          const auto target =
                                                                              co_await transaction
                                                                                  .Execute(
                                                                                      std::move(
                                                                                          query));
                                                                          if (target
                                                                                  .rows()
                                                                                  .empty()) {
                                                                            co_return;
                                                                          }
                                                                          // A
                                                                          // character
                                                                          // must
                                                                          // always
                                                                          // have
                                                                          // an
                                                                          // appearance.
                                                                          // Unequipping
                                                                          // a
                                                                          // paid
                                                                          // character
                                                                          // falls
                                                                          // back
                                                                          // below.
                                                                          if (!equipped &&
                                                                              target.rows()
                                                                                      [0]
                                                                                      [0]
                                                                                          .as_uint64() ==
                                                                                  kCharacterEquipmentSlot &&
                                                                              item_id ==
                                                                                  300000 +
                                                                                      target
                                                                                          .rows()
                                                                                              [0]
                                                                                              [1]
                                                                                          .as_uint64()) {
                                                                            co_return;
                                                                          }
                                                                          if (equipped) {
                                                                            query = {
                                                                                "UPDATE character_items JOIN item_equipment_rules AS current_rules "
                                                                                "ON current_rules.item_id = character_items.item_id "
                                                                                "JOIN item_equipment_rules AS requested_rules ON requested_rules.item_id = ? "
                                                                                "SET equipped_slot = NULL, updated_at = UTC_TIMESTAMP(6) "
                                                                                "WHERE character_id = ? AND equipped_slot IS NOT NULL "
                                                                                "AND (current_rules.occupied_slots & requested_rules.occupied_slots) <> 0 "
                                                                                "AND character_items.item_id <> ?",
                                                                                {field(
                                                                                     item_id),
                                                                                 field(
                                                                                     character_id),
                                                                                 field(
                                                                                     item_id)}};
                                                                            co_await transaction
                                                                                .Execute(
                                                                                    std::move(
                                                                                        query));
                                                                            query = {
                                                                                "UPDATE character_items JOIN item_definitions "
                                                                                "ON item_definitions.id = character_items.item_id "
                                                                                "SET equipped_slot = item_definitions.equipment_slot, updated_at = UTC_TIMESTAMP(6) "
                                                                                "WHERE character_id = ? AND item_id = ?",
                                                                                {field(
                                                                                     character_id),
                                                                                 field(
                                                                                     item_id)}};
                                                                          } else {
                                                                            query = {
                                                                                "UPDATE character_items SET equipped_slot = NULL, updated_at = UTC_TIMESTAMP(6) "
                                                                                "WHERE character_id = ? AND item_id = ?",
                                                                                {field(
                                                                                     character_id),
                                                                                 field(
                                                                                     item_id)}};
                                                                          }
                                                                          co_await transaction
                                                                              .Execute(
                                                                                  std::move(
                                                                                      query));
                                                                          co_await NormalizeAppearance(
                                                                              transaction,
                                                                              character_id,
                                                                              *snapshot);
                                                                          co_await NormalizeWeaponPresets(
                                                                              transaction,
                                                                              character_id,
                                                                              *snapshot);
                                                                          accepted =
                                                                              true;
                                                                        });
  co_return accepted;
}
}  // namespace bubblefighter::inventory
