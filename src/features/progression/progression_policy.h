#ifndef BUBBLEFIGHTER_SERVER_FEATURES_PROGRESSION_PROGRESSION_POLICY_H_
#define BUBBLEFIGHTER_SERVER_FEATURES_PROGRESSION_PROGRESSION_POLICY_H_

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace bubblefighter::progression {
struct CharacterProgress {
  std::uint32_t experience = 0;
  std::uint32_t level = 1;
  std::uint32_t level_start = 0;
  std::uint32_t next_level = 100;

  bool operator==(const CharacterProgress&) const = default;
};

struct GameReward {
  std::uint32_t experience = 0;
  std::uint32_t lucci = 0;
};

struct ResultAward {
  std::uint32_t score = 0;
  std::uint32_t grade = 0xFFFFFFFF;  // Original atlas: 0 = SSS, ..., 8 = F;
                                     // unset for historical results.
  std::uint32_t medal =
      0;  // Ordinary award type, not the mode-prefixed medal identifier.

  bool operator==(const ResultAward&) const = default;
};

// Configurable private-server policy; client protocol evidence does not
// establish operator reward rates.
class ProgressionPolicy {
 public:
  static ProgressionPolicy Load(const std::filesystem::path& path);
  static ProgressionPolicy Parse(std::string_view text);
  CharacterProgress Describe(std::uint32_t experience) const;
  CharacterProgress Advance(std::uint32_t experience, std::uint32_t gain) const;
  GameReward Reward(std::uint8_t team, std::uint8_t winner) const;
  ResultAward EvaluateResult(std::uint16_t bubbles, std::uint16_t pangs,
                             std::uint16_t rescues) const;
  const std::string& GetVersion() const;

 private:
  std::string version_;
  std::vector<std::uint32_t> thresholds_;
  std::array<GameReward, 3> rewards_;                // Win, draw, loss.
  std::array<std::uint32_t, 3> score_weights_{};     // Bubble, pang, rescue.
  std::array<std::uint32_t, 9> grade_thresholds_{};  // Ascending F through SSS.
  std::array<std::array<std::uint32_t, 2>, 3>
      medal_thresholds_{};  // Expert, master per action.
};
}  // namespace bubblefighter::progression

#endif  // BUBBLEFIGHTER_SERVER_FEATURES_PROGRESSION_PROGRESSION_POLICY_H_
