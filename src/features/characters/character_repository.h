#ifndef BUBBLEFIGHTER_SERVER_FEATURES_CHARACTERS_CHARACTER_REPOSITORY_H_
#define BUBBLEFIGHTER_SERVER_FEATURES_CHARACTERS_CHARACTER_REPOSITORY_H_

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include "features/progression/progression_policy.h"
#include "persistence/my_sql_database.h"

namespace bubblefighter::characters {
enum class CreationResult : std::uint32_t {
  kSuccess = 0,
  kNameTaken = 1,
  kStorageFailure = 2,
  kInvalidName = 3,
  kInvalidCharacter = 4,
  kInvalidLength = 6
};

struct CharacterProfile {
  std::uint32_t id;
  std::uint32_t account_id;
  std::u16string nickname;
  std::uint32_t character_type;
  std::uint32_t starter_selection;
  std::uint32_t level = 1;
};

struct CharacterStats {
  std::uint32_t lucci = 10000;
  std::uint32_t cash = 0;
  progression::CharacterProgress progress;
};

CreationResult CheckNickname(std::u16string_view nickname);
bool IsStarterSelection(std::uint32_t character_type,
                        std::uint32_t starter_selection);
std::string EncodeNickname(std::u16string_view nickname);
std::u16string DecodeNickname(std::string_view nickname);

class CharacterRepository {
 public:
  CharacterRepository(database::MySqlDatabase& database,
                      const progression::ProgressionPolicy& policy);
  boost::asio::awaitable<std::optional<CharacterProfile>> FindByAccount(
      std::uint32_t account_id);
  boost::asio::awaitable<CharacterStats> FindStats(std::uint32_t character_id);
  boost::asio::awaitable<CreationResult> CheckName(std::u16string nickname);
  boost::asio::awaitable<CreationResult> Create(
      std::uint32_t account_id, std::u16string nickname,
      std::uint32_t character_type, std::uint32_t starter_selection);

 private:
  database::MySqlDatabase& database_;
  const progression::ProgressionPolicy& policy_;
};
}  // namespace bubblefighter::characters

#endif  // BUBBLEFIGHTER_SERVER_FEATURES_CHARACTERS_CHARACTER_REPOSITORY_H_
