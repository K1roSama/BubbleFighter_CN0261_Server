#include "features/progression/progression_policy.h"

#include <algorithm>
#include <fstream>
#include <limits>
#include <nlohmann/json.hpp>
#include <regex>
#include <stdexcept>

namespace bubblefighter::progression {
namespace {
std::uint32_t Number(const nlohmann::json& value, std::uint32_t maximum) {
  if (!value.is_number_integer() || value < 0 || value > maximum) {
    throw std::invalid_argument(
        "Progression values must be nonnegative integers within the supported range");
  }
  return value.get<std::uint32_t>();
}
}  // namespace

ProgressionPolicy ProgressionPolicy::Load(const std::filesystem::path& path) {
  std::ifstream stream(path, std::ios::binary);
  if (!stream) {
    throw std::runtime_error("Could not open progression policy");
  }
  return Parse(std::string(std::istreambuf_iterator<char>(stream), {}));
}

ProgressionPolicy ProgressionPolicy::Parse(std::string_view text) {
  try {
    const auto root = nlohmann::json::parse(text);
    if (!root.is_object() || root.size() != 4 ||
        !root.at("version").is_string() ||
        !root.at("levelExperience").is_array() ||
        !root.at("rewards").is_object()) {
      throw std::invalid_argument("Invalid progression policy structure");
    }
    ProgressionPolicy policy;
    policy.version_ = root.at("version").get<std::string>();
    if (!std::regex_match(policy.version_,
                          std::regex("[a-z0-9][a-z0-9_-]{0,31}"))) {
      throw std::invalid_argument(
          "Progression policy version must use 1-32 lowercase letters, digits, _ or -");
    }
    for (const auto& value : root.at("levelExperience")) {
      const auto threshold =
          Number(value, std::numeric_limits<std::int32_t>::max());
      if ((!policy.thresholds_.empty() &&
           threshold <= policy.thresholds_.back()) ||
          (policy.thresholds_.empty() && threshold != 0)) {
        throw std::invalid_argument(
            "Level experience must start at zero and increase strictly");
      }
      policy.thresholds_.push_back(threshold);
    }
    if (policy.thresholds_.size() < 2 || policy.thresholds_.size() > 200) {
      throw std::invalid_argument(
          "Progression policy must contain 2-200 level thresholds");
    }
    const auto& rewards = root.at("rewards");
    if (rewards.size() != 3) {
      throw std::invalid_argument(
          "Progression policy requires win, draw and loss rewards");
    }
    std::size_t index = 0;
    for (const auto* name : {"win", "draw", "loss"}) {
      const auto& reward = rewards.at(name);
      if (!reward.is_object() || reward.size() != 2) {
        throw std::invalid_argument(
            "Game rewards require experience and lucci");
      }
      policy.rewards_[index++] = {Number(reward.at("experience"), 1000000),
                                  Number(reward.at("lucci"), 1000000)};
    }
    const auto& awards = root.at("resultAwards");
    if (!awards.is_object() || awards.size() != 3 ||
        !awards.at("scoreWeights").is_object() ||
        awards.at("scoreWeights").size() != 3 ||
        !awards.at("gradeMinimumScores").is_array() ||
        awards.at("gradeMinimumScores").size() != 9 ||
        !awards.at("medalMinimumCounts").is_object() ||
        awards.at("medalMinimumCounts").size() != 3) {
      throw std::invalid_argument(
          "resultAwards requires scoreWeights, nine gradeMinimumScores and medalMinimumCounts");
    }
    index = 0;
    for (const auto* name : {"bubble", "pang", "rescue"}) {
      policy.score_weights_[index] =
          Number(awards.at("scoreWeights").at(name), 1000);
      const auto& thresholds = awards.at("medalMinimumCounts").at(name);
      if (!thresholds.is_array() || thresholds.size() != 2) {
        throw std::invalid_argument(
            "medalMinimumCounts requires two increasing thresholds per action");
      }
      const auto first = Number(thresholds[0], 65535);
      const auto second = Number(thresholds[1], 65535);
      if (first == 0 || second <= first) {
        throw std::invalid_argument(
            "medalMinimumCounts must be positive and increase strictly");
      }
      policy.medal_thresholds_[index++] = {first, second};
    }
    if (policy.score_weights_ == std::array<std::uint32_t, 3>{}) {
      throw std::invalid_argument(
          "resultAwards scoreWeights cannot all be zero");
    }
    for (index = 0; index < policy.grade_thresholds_.size(); ++index) {
      const auto threshold =
          Number(awards.at("gradeMinimumScores")[index], 196605000);
      if ((index == 0 && threshold != 0) ||
          (index != 0 && threshold <= policy.grade_thresholds_[index - 1])) {
        throw std::invalid_argument(
            "gradeMinimumScores must start at zero and increase from F through SSS");
      }
      policy.grade_thresholds_[index] = threshold;
    }
    return policy;
  } catch (const nlohmann::json::exception&) {
    throw std::invalid_argument(
        "Progression policy contains invalid JSON or missing fields");
  }
}

CharacterProgress ProgressionPolicy::Describe(std::uint32_t experience) const {
  if (thresholds_.empty() || experience > thresholds_.back()) {
    throw std::invalid_argument(
        "Stored experience exceeds the configured progression range");
  }
  const auto next =
      std::upper_bound(thresholds_.begin(), thresholds_.end(), experience);
  const auto level = static_cast<std::uint32_t>(next - thresholds_.begin());
  return {experience, level, thresholds_[level - 1],
          next == thresholds_.end() ? thresholds_.back() : *next};
}

CharacterProgress ProgressionPolicy::Advance(std::uint32_t experience,
                                             std::uint32_t gain) const {
  static_cast<void>(Describe(experience));
  return Describe(static_cast<std::uint32_t>(
      std::min(static_cast<std::uint64_t>(experience) + gain,
               static_cast<std::uint64_t>(thresholds_.back()))));
}

GameReward ProgressionPolicy::Reward(std::uint8_t team,
                                     std::uint8_t winner) const {
  if (team < 1 || team > 2 || winner > 2) {
    throw std::invalid_argument(
        "Ordinary rewards require team 1 or 2 and winner 0-2");
  }
  return rewards_[winner == 0 ? 1 : (team == winner ? 0 : 2)];
}

const std::string& ProgressionPolicy::GetVersion() const { return version_; }

ResultAward ProgressionPolicy::EvaluateResult(std::uint16_t bubbles,
                                              std::uint16_t pangs,
                                              std::uint16_t rescues) const {
  const std::array<std::uint32_t, 3> counts{bubbles, pangs, rescues};
  ResultAward result;
  unsigned int best_tier = 0;
  std::uint32_t best_count = 0;
  for (std::size_t index = 0; index < counts.size(); ++index) {
    // Bounded counts and weights keep the sum below signed 32-bit protocol
    // limits.
    result.score += counts[index] * score_weights_[index];
    const auto& thresholds = medal_thresholds_[index];
    const unsigned int tier = counts[index] >= thresholds[1]
                                  ? 2
                                  : (counts[index] >= thresholds[0] ? 1 : 0);
    if (tier > best_tier ||
        (tier != 0 && tier == best_tier && counts[index] > best_count)) {
      best_tier = tier;
      best_count = counts[index];
      result.medal = static_cast<std::uint32_t>(index) + (tier == 2 ? 7 : 4);
    }
  }
  const auto next = std::upper_bound(grade_thresholds_.begin(),
                                     grade_thresholds_.end(), result.score);
  result.grade = static_cast<std::uint32_t>(grade_thresholds_.end() - next);
  return result;
}
}  // namespace bubblefighter::progression
