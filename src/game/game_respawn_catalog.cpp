#include "game/game_respawn_catalog.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <set>
#include <stdexcept>

namespace bubblefighter::game {
namespace {
using Json = nlohmann::json;

void CheckKeys(const Json& object,
               std::initializer_list<std::string_view> allowed) {
  if (!object.is_object() || object.size() != allowed.size()) {
    throw std::invalid_argument(
        "Game respawn catalog has missing or unexpected fields");
  }
  for (const auto& entry : object.items()) {
    if (std::find(allowed.begin(), allowed.end(), entry.key()) ==
        allowed.end()) {
      throw std::invalid_argument("Game respawn catalog has an unknown field");
    }
  }
}

std::uint32_t Integer(const Json& value, std::uint32_t maximum) {
  if (!value.is_number_integer() ||
      (!value.is_number_unsigned() && value.get<std::int64_t>() < 0) ||
      value.get<std::uint64_t>() > maximum) {
    throw std::invalid_argument(
        "Game respawn catalog integer is outside its supported range");
  }
  return value.get<std::uint32_t>();
}

std::map<std::uint32_t, std::int64_t> ReadEntries(const Json& entries,
                                                  const char* duration_key,
                                                  std::uint32_t maximum_id) {
  if (!entries.is_array() || entries.empty() || entries.size() > 4096) {
    throw std::invalid_argument(
        "Game respawn catalog has an invalid collection");
  }
  std::map<std::uint32_t, std::int64_t> result;
  for (const auto& entry : entries) {
    CheckKeys(entry, {"id", duration_key});
    const auto id = Integer(entry.at("id"), maximum_id);
    const auto duration = Integer(entry.at(duration_key), 60000);
    if (id == 0 || !result.emplace(id, duration).second) {
      throw std::invalid_argument(
          "Game respawn catalog contains an invalid or duplicate identity");
    }
  }
  return result;
}
}  // namespace

GameRespawnCatalog GameRespawnCatalog::Load(const std::filesystem::path& path) {
  std::ifstream input(path, std::ios::binary);
  if (!input) {
    throw std::runtime_error("Cannot open gameRespawnFile");
  }
  const std::string content{std::istreambuf_iterator<char>(input),
                            std::istreambuf_iterator<char>()};
  return Parse(content);
}

GameRespawnCatalog GameRespawnCatalog::Parse(std::string_view text) {
  try {
    const auto root = Json::parse(text);
    CheckKeys(root, {"characters", "items"});
    GameRespawnCatalog catalog;
    catalog.characters_ =
        ReadEntries(root.at("characters"), "durationMilliseconds", 32);
    catalog.reductions_ =
        ReadEntries(root.at("items"), "reductionMilliseconds", 999999);
    if (!catalog.characters_.contains(1) || !catalog.characters_.contains(2)) {
      throw std::invalid_argument(
          "Game respawn catalog is missing initial character profiles");
    }
    return catalog;
  } catch (const Json::exception&) {
    throw std::invalid_argument(
        "Game respawn catalog is not valid JSON with the required fields");
  }
}

std::chrono::milliseconds GameRespawnCatalog::Evaluate(
    std::uint32_t character_type,
    std::span<const inventory::InventoryItem> items) const {
  const auto character = characters_.find(character_type);
  if (character == characters_.end() ||
      items.size() > inventory::kMaximumInventoryItems) {
    throw std::invalid_argument(
        "Selected character or inventory has no supported respawn profile");
  }
  auto duration = character->second;
  std::set<std::uint32_t> equipped;
  for (const auto& item : items) {
    if (item.equipped_slot == 0 || item.quantity == 0) {
      continue;
    }
    if (!equipped.insert(item.item_id).second) {
      throw std::invalid_argument(
          "Equipped inventory contains a duplicate respawn item");
    }
    if (const auto reduction = reductions_.find(item.item_id);
        reduction != reductions_.end()) {
      // Original property 23 adds signed integers at Spec +780; 0x5BEF00
      // subtracts the sum from the base.
      duration -= reduction->second;
    }
  }
  if (duration < 0) {
    throw std::invalid_argument(
        "Equipped respawn reduction exceeds the supported base duration");
  }
  return std::chrono::milliseconds(duration);
}
}  // namespace bubblefighter::game
