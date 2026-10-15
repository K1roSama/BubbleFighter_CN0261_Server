#ifndef BUBBLEFIGHTER_SERVER_GAME_GAME_MAP_CATALOG_H_
#define BUBBLEFIGHTER_SERVER_GAME_GAME_MAP_CATALOG_H_

#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace bubblefighter::game {
struct GameMap {
  std::u16string resource_name;
  std::u16string theme;
  std::uint32_t difficulty = 0;
  std::uint32_t time_limit_milliseconds = 0;
  std::uint32_t score_limit = 0;
  bool normal_mode = false;
  bool item_mode = false;
  std::uint8_t team_spawn_count = 0;
};

class GameMapCatalog {
 public:
  static GameMapCatalog Load(const std::filesystem::path& path);
  static GameMapCatalog Parse(std::string_view text);
  const GameMap* Find(std::u16string_view resource_name) const;
  const GameMap& GetDefaultRules() const;
  std::vector<const GameMap*> FindCandidates(std::uint32_t mode,
                                             std::u16string_view theme) const;

 private:
  GameMapCatalog() = default;
  std::u16string default_rules_map_;
  std::map<std::u16string, GameMap, std::less<>> maps_;
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_GAME_MAP_CATALOG_H_
