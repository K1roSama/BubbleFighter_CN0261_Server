#include "game/game_defense_catalog.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace bubblefighter::game {
namespace {
using Json = nlohmann::json;

void CheckKeys(const Json& object,
               std::initializer_list<std::string_view> allowed) {
  if (!object.is_object() || object.size() != allowed.size()) {
    throw std::invalid_argument(
        "Game defense catalog has missing or unexpected fields");
  }
  for (const auto& entry : object.items()) {
    if (std::find(allowed.begin(), allowed.end(), entry.key()) ==
        allowed.end()) {
      throw std::invalid_argument("Game defense catalog has an unknown field");
    }
  }
}

std::uint32_t ItemId(const Json& value) {
  if (!value.is_number_integer() ||
      (!value.is_number_unsigned() && value.get<std::int64_t>() <= 0) ||
      value.get<std::uint64_t>() == 0 || value.get<std::uint64_t>() > 999999) {
    throw std::invalid_argument(
        "Game defense itemId must be an integer in 1..999999");
  }
  return value.get<std::uint32_t>();
}
}  // namespace

GameDefenseCatalog GameDefenseCatalog::Load(const std::filesystem::path& path) {
  std::ifstream input(path, std::ios::binary);
  if (!input) {
    throw std::runtime_error("Cannot open gameDefenseFile");
  }
  const std::string content{std::istreambuf_iterator<char>(input),
                            std::istreambuf_iterator<char>()};
  return Parse(content);
}

GameDefenseCatalog GameDefenseCatalog::Parse(std::string_view text) {
  try {
    const auto root = Json::parse(text);
    CheckKeys(root, {"items", "unsupportedItems"});
    const auto& items = root.at("items");
    const auto& unsupported = root.at("unsupportedItems");
    if (!items.is_array() || items.empty() || items.size() > 16384 ||
        !unsupported.is_array() || unsupported.size() > 16384) {
      throw std::invalid_argument(
          "Game defense catalog has invalid item collections");
    }
    GameDefenseCatalog catalog;
    for (const auto& entry : items) {
      CheckKeys(entry, {"itemId", "modifiers"});
      const auto id = ItemId(entry.at("itemId"));
      const auto& modifiers = entry.at("modifiers");
      if (!modifiers.is_array() || modifiers.empty() || modifiers.size() > 64 ||
          catalog.items_.contains(id)) {
        throw std::invalid_argument(
            "Game defense item has invalid modifiers or a duplicate identity");
      }
      auto& target = catalog.items_[id];
      for (const auto& modifier : modifiers) {
        CheckKeys(modifier, {"attribute", "ratio"});
        const auto& name = modifier.at("attribute");
        const auto& value = modifier.at("ratio");
        if (!name.is_string() ||
            (name != "minimum" && name != "maximum" &&
             name != "criticalCancelRatio" && name != "backReduction" &&
             name != "wallReduction") ||
            !value.is_number() || !std::isfinite(value.get<double>()) ||
            value.get<double>() < 0.0 || value.get<double>() > 1.0) {
          throw std::invalid_argument(
              "Game defense modifier requires a known attribute and ratio in 0..1");
        }
        const auto attribute =
            name == "minimum"         ? Attribute::kMinimum
            : name == "maximum"       ? Attribute::kMaximum
            : name == "backReduction" ? Attribute::kBackReduction
            : name == "wallReduction" ? Attribute::kWallReduction
                                      : Attribute::kCriticalCancelRatio;
        target.push_back({attribute, value.get<float>()});
      }
    }
    for (const auto& value : unsupported) {
      const auto id = ItemId(value);
      if (catalog.items_.contains(id) ||
          !catalog.unsupported_items_.insert(id).second) {
        throw std::invalid_argument(
            "Game defense unsupported item is duplicated or already defined");
      }
    }
    return catalog;
  } catch (const Json::exception&) {
    throw std::invalid_argument(
        "Game defense catalog is not valid JSON with the required fields");
  }
}

DefenseProfile GameDefenseCatalog::Evaluate(
    std::span<const inventory::InventoryItem> items) const {
  // 0xA32010 applies a sorted set of equipped IDs, once per item despite
  // overlapping occupied parts.
  std::set<std::uint32_t> equipped;
  for (const auto& item : items) {
    if (item.quantity != 0 && item.equipped_slot != 0) {
      equipped.insert(item.item_id);
    }
  }
  DefenseProfile profile;
  for (const auto id : equipped) {
    if (unsupported_items_.contains(id)) {
      throw std::invalid_argument(
          "Equipped item requires unsupported conditional defense attributes");
    }
    const auto found = items_.find(id);
    if (found == items_.end()) {
      continue;
    }
    for (const auto& modifier : found->second) {
      if (modifier.attribute == Attribute::kMinimum) {
        profile.minimum += modifier.ratio;
      } else if (modifier.attribute == Attribute::kMaximum) {
        profile.maximum += modifier.ratio;
      } else if (modifier.attribute == Attribute::kCriticalCancelRatio) {
        // 0xA41830 case 0x15 assigns; it does not use the additive 0xA32E80
        // path.
        profile.critical_cancel_ratio = modifier.ratio;
      } else if (modifier.attribute == Attribute::kBackReduction) {
        // 0xA41830 case 8 adds to CharacterSpec +300 through 0xA32E80.
        profile.back_reduction += modifier.ratio;
      } else {
        // Case 0x21 adds to +1068; this is separate from the sampled defense
        // range.
        profile.wall_reduction += modifier.ratio;
      }
    }
  }
  if (profile.minimum > profile.maximum || profile.maximum > 1.0f) {
    throw std::invalid_argument(
        "Equipped defense range is inconsistent or exceeds 100 percent");
  }
  if (profile.back_reduction + profile.maximum > 1.0f ||
      profile.wall_reduction > 1.0f) {
    throw std::invalid_argument(
        "Equipped directional defense exceeds the supported reduction range");
  }
  return profile;
}
}  // namespace bubblefighter::game
