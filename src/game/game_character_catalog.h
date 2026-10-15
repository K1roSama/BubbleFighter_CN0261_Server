#ifndef BUBBLEFIGHTER_SERVER_GAME_GAME_CHARACTER_CATALOG_H_
#define BUBBLEFIGHTER_SERVER_GAME_GAME_CHARACTER_CATALOG_H_

#include <cstdint>
#include <filesystem>
#include <map>
#include <string_view>

namespace bubblefighter::game {
struct CharacterHealthProfile {
  std::uint32_t character_type = 0;
  float base_health = 0.0f;
};

class GameCharacterCatalog {
 public:
  static GameCharacterCatalog Load(const std::filesystem::path& path);
  static GameCharacterCatalog Parse(std::string_view text);
  CharacterHealthProfile Evaluate(std::uint32_t character_type,
                                  std::uint32_t channel_type,
                                  std::uint32_t room_mode) const;

 private:
  GameCharacterCatalog() = default;
  float common_health_ = 0.0f;
  std::map<std::uint32_t, float> characters_;
  std::map<std::uint32_t, float> channels_;
  std::map<std::uint32_t, float> modes_;
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_GAME_CHARACTER_CATALOG_H_
