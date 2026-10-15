#include "game/game_map_catalog.h"

#include <algorithm>
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
        "Game map catalog has missing or unexpected fields");
  }

  for (const auto& entry : object.items()) {
    if (std::find(allowed.begin(), allowed.end(), entry.key()) ==
        allowed.end()) {
      throw std::invalid_argument("Game map catalog has an unknown field");
    }
  }
}

std::u16string ResourceName(const Json& object, const char* key) {
  if (!object.at(key).is_string()) {
    throw std::invalid_argument(std::string("Game map ") + key +
                                " must be a string");
  }

  const auto value = object.at(key).get<std::string>();
  if (value.empty() || value.size() > 96 ||
      !std::all_of(value.begin(), value.end(), [](char character) {
        return (character >= 'A' && character <= 'Z') ||
               (character >= 'a' && character <= 'z') ||
               (character >= '0' && character <= '9') || character == '_';
      })) {
    throw std::invalid_argument(std::string("Game map ") + key +
                                " must be an ASCII resource identifier");
  }
  return {value.begin(), value.end()};
}

std::uint32_t Number(const Json& object, const char* key, std::uint32_t minimum,
                     std::uint32_t maximum) {
  const auto& value = object.at(key);
  if (!value.is_number_integer() ||
      (value.is_number_integer() && !value.is_number_unsigned() &&
       value.get<std::int64_t>() < 0)) {
    throw std::invalid_argument(std::string("Game map ") + key +
                                " must be a nonnegative integer");
  }

  const auto number = value.get<std::uint64_t>();
  if (number < minimum || number > maximum) {
    throw std::invalid_argument(std::string("Game map ") + key +
                                " is outside the supported range");
  }
  return static_cast<std::uint32_t>(number);
}

bool Flag(const Json& object, const char* key) {
  if (!object.at(key).is_boolean()) {
    throw std::invalid_argument(std::string("Game map ") + key +
                                " must be a boolean");
  }
  return object.at(key).get<bool>();
}
}  // namespace

GameMapCatalog GameMapCatalog::Load(const std::filesystem::path& path) {
  std::ifstream input(path, std::ios::binary);
  if (!input) {
    throw std::runtime_error("Cannot open gameMapsFile");
  }

  const std::string content{std::istreambuf_iterator<char>(input),
                            std::istreambuf_iterator<char>()};
  return Parse(content);
}

GameMapCatalog GameMapCatalog::Parse(std::string_view text) {
  try {
    const auto root = Json::parse(text);
    CheckKeys(root, {"defaultRulesMap", "maps"});
    GameMapCatalog catalog;
    catalog.default_rules_map_ = ResourceName(root, "defaultRulesMap");
    const auto& maps = root.at("maps");
    if (!maps.is_array() || maps.empty() || maps.size() > 512) {
      throw std::invalid_argument(
          "Game map catalog must contain between 1 and 512 maps");
    }

    for (const auto& entry : maps) {
      CheckKeys(entry,
                {"resourceName", "theme", "difficulty", "timeLimitMilliseconds",
                 "scoreLimit", "normalMode", "itemMode", "teamSpawnCount"});
      GameMap map;
      map.resource_name = ResourceName(entry, "resourceName");
      map.theme = ResourceName(entry, "theme");
      map.difficulty = Number(entry, "difficulty", 0, 100);
      map.time_limit_milliseconds =
          Number(entry, "timeLimitMilliseconds", 60000, 1800000);
      map.score_limit = Number(entry, "scoreLimit", 1, 200);
      map.normal_mode = Flag(entry, "normalMode");
      map.item_mode = Flag(entry, "itemMode");
      map.team_spawn_count =
          static_cast<std::uint8_t>(Number(entry, "teamSpawnCount", 1, 8));
      if ((!map.normal_mode && !map.item_mode) ||
          map.time_limit_milliseconds % 1000 != 0) {
        throw std::invalid_argument(
            "Game map needs a supported mode and a whole-second time limit");
      }

      const auto key = map.resource_name;
      if (!catalog.maps_.emplace(key, std::move(map)).second) {
        throw std::invalid_argument("Game map resourceName is duplicated");
      }
    }
    if (!catalog.Find(catalog.default_rules_map_)) {
      throw std::invalid_argument(
          "Game map defaultRulesMap is absent from maps");
    }
    return catalog;
  } catch (const Json::exception&) {
    throw std::invalid_argument(
        "Game map catalog is not valid JSON with the required fields");
  }
}

const GameMap* GameMapCatalog::Find(std::u16string_view resource_name) const {
  const auto entry = maps_.find(resource_name);
  return entry == maps_.end() ? nullptr : &entry->second;
}

const GameMap& GameMapCatalog::GetDefaultRules() const {
  return maps_.at(default_rules_map_);
}

std::vector<const GameMap*> GameMapCatalog::FindCandidates(
    std::uint32_t mode, std::u16string_view theme) const {
  std::vector<const GameMap*> candidates;
  for (const auto& [name, map] : maps_) {
    static_cast<void>(name);
    if (((mode == 1 && map.normal_mode) || (mode == 3 && map.item_mode)) &&
        (theme == u"All" || theme == map.theme) && map.team_spawn_count != 0) {
      candidates.push_back(&map);
    }
  }
  return candidates;
}
}  // namespace bubblefighter::game
