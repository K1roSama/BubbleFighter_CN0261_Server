#include "game/game_character_catalog.h"

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
        "Game character catalog has missing or unexpected fields");
  }
  for (const auto& entry : object.items()) {
    if (std::find(allowed.begin(), allowed.end(), entry.key()) ==
        allowed.end()) {
      throw std::invalid_argument(
          "Game character catalog has an unknown field");
    }
  }
}

float HealthValue(const Json& value) {
  if (!value.is_number() || !std::isfinite(value.get<double>()) ||
      value.get<double>() < -10000.0 || value.get<double>() > 10000.0) {
    throw std::invalid_argument(
        "Game character health must be a finite number in -10000..10000");
  }
  return value.get<float>();
}

std::uint32_t Identity(const Json& value, std::uint32_t maximum) {
  if (!value.is_number_integer() ||
      (!value.is_number_unsigned() && value.get<std::int64_t>() <= 0) ||
      value.get<std::uint64_t>() == 0 || value.get<std::uint64_t>() > maximum) {
    throw std::invalid_argument(
        "Game character catalog identity is outside its supported range");
  }
  return value.get<std::uint32_t>();
}

std::map<std::uint32_t, float> ReadEntries(const Json& entries,
                                           std::uint32_t maximum,
                                           bool modes = false) {
  if (!entries.is_array() || entries.empty() || entries.size() > 128) {
    throw std::invalid_argument(
        "Game character catalog has an invalid collection");
  }
  std::map<std::uint32_t, float> result;
  for (const auto& entry : entries) {
    CheckKeys(
        entry,
        modes ? std::initializer_list<std::string_view>{"id", "resourceId",
                                                        "healthDelta"}
              : std::initializer_list<std::string_view>{"id", "healthDelta"});
    const auto id = Identity(entry.at("id"), maximum);
    if (modes &&
        ((id != 1 && id != 3) || Identity(entry.at("resourceId"), 2) != 2)) {
      throw std::invalid_argument(
          "Supported room modes 1 and 3 must use FighterConfigMode 2");
    }
    if (!result.emplace(id, HealthValue(entry.at("healthDelta"))).second) {
      throw std::invalid_argument(
          "Game character catalog contains a duplicate identity");
    }
  }
  return result;
}
}  // namespace

GameCharacterCatalog GameCharacterCatalog::Load(
    const std::filesystem::path& path) {
  std::ifstream input(path, std::ios::binary);
  if (!input) {
    throw std::runtime_error("Cannot open gameCharactersFile");
  }
  const std::string content{std::istreambuf_iterator<char>(input),
                            std::istreambuf_iterator<char>()};
  return Parse(content);
}

GameCharacterCatalog GameCharacterCatalog::Parse(std::string_view text) {
  try {
    const auto root = Json::parse(text);
    CheckKeys(root, {"commonHealth", "characters", "channels", "modes"});
    GameCharacterCatalog catalog;
    catalog.common_health_ = HealthValue(root.at("commonHealth"));
    catalog.characters_ = ReadEntries(root.at("characters"), 32);
    catalog.channels_ = ReadEntries(root.at("channels"), 4000);
    catalog.modes_ = ReadEntries(root.at("modes"), 3, true);
    if (catalog.common_health_ <= 0.0f || !catalog.characters_.contains(1) ||
        !catalog.characters_.contains(2) || !catalog.channels_.contains(10) ||
        !catalog.channels_.contains(60) || !catalog.modes_.contains(1) ||
        !catalog.modes_.contains(3)) {
      throw std::invalid_argument(
          "Game character catalog is missing required base profiles");
    }
    // Reject invalid combinations on load, before a user can select them for a
    // round.
    for (const auto& [character, characterDelta] : catalog.characters_) {
      static_cast<void>(characterDelta);
      for (const auto& [channel, channelDelta] : catalog.channels_) {
        static_cast<void>(channelDelta);
        if (channel == 99 || channel == 100) {
          throw std::invalid_argument(
              "Fallback channel aliases must use the channel 10 profile");
        }
        for (const auto mode : {1u, 3u}) {
          static_cast<void>(catalog.Evaluate(character, channel, mode));
        }
      }
    }
    return catalog;
  } catch (const Json::exception&) {
    throw std::invalid_argument(
        "Game character catalog is not valid JSON with the required fields");
  }
}

CharacterHealthProfile GameCharacterCatalog::Evaluate(
    std::uint32_t character_type, std::uint32_t channel_type,
    std::uint32_t room_mode) const {
  // 0x5C3BF0 maps unset and nonstandard channel IDs to the beginner resource
  // column.
  if (channel_type == 0 || channel_type == 99 || channel_type == 100) {
    channel_type = 10;
  }
  const auto character = characters_.find(character_type);
  const auto channel = channels_.find(channel_type);
  const auto mode = modes_.find(room_mode);
  if (character == characters_.end() || channel == channels_.end() ||
      mode == modes_.end()) {
    throw std::invalid_argument(
        "Selected character, channel or room mode has no supported health profile");
  }
  // 0x589F10 composes common, channel, mode, then character; 0x5EB2D0 stores
  // each sum as float32.
  float health = common_health_;
  health += channel->second;
  health += mode->second;
  health += character->second;
  if (!std::isfinite(health) || health <= 0.0f || health > 10000.0f) {
    throw std::invalid_argument(
        "Combined character base health must be in (0, 10000]");
  }
  return {character_type, health};
}
}  // namespace bubblefighter::game
