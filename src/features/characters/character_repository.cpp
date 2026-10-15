#include "features/characters/character_repository.h"

#include <boost/mysql/common_server_errc.hpp>
#include <stdexcept>

namespace bubblefighter::characters {
CreationResult CheckNickname(std::u16string_view nickname) {
  std::size_t width = 0;
  for (const auto unit : nickname) {
    if ((unit >= u'a' && unit <= u'z') || (unit >= u'A' && unit <= u'Z') ||
        (unit >= u'0' && unit <= u'9')) {
      ++width;
    } else if ((unit >= 0x3400 && unit <= 0x9FFF) ||
               (unit >= 0xAC00 && unit <= 0xD7A3)) {
      width += 2;
    } else {
      return CreationResult::kInvalidName;
    }
  }

  return width >= 4 && width <= 12 ? CreationResult::kSuccess
                                   : CreationResult::kInvalidLength;
}

bool IsStarterSelection(std::uint32_t character_type,
                        std::uint32_t starter_selection) {
  // DlgCreateUser (VA 0x8FB960) offers (Dao, 1) and (Dizni, 2) with content
  // flag 483 disabled.
  return (character_type == 1 || character_type == 2) &&
         starter_selection == character_type;
}

std::string EncodeNickname(std::u16string_view nickname) {
  if (CheckNickname(nickname) != CreationResult::kSuccess) {
    throw std::invalid_argument("Character nickname is invalid");
  }

  std::string result;
  for (const auto unit : nickname) {
    if (unit < 0x80) {
      result.push_back(static_cast<char>(unit));
    } else {
      result.push_back(static_cast<char>(0xE0 | (unit >> 12)));
      result.push_back(static_cast<char>(0x80 | ((unit >> 6) & 0x3F)));
      result.push_back(static_cast<char>(0x80 | (unit & 0x3F)));
    }
  }
  return result;
}

std::u16string DecodeNickname(std::string_view nickname) {
  std::u16string result;
  for (std::size_t index = 0; index < nickname.size();) {
    const auto first = static_cast<unsigned char>(nickname[index++]);
    if (first < 0x80) {
      result.push_back(first);
    } else {
      if (first < 0xE0 || first > 0xEF || nickname.size() - index < 2) {
        throw std::runtime_error(
            "Stored character nickname encoding is invalid");
      }
      const auto second = static_cast<unsigned char>(nickname[index++]);
      const auto third = static_cast<unsigned char>(nickname[index++]);
      if ((second & 0xC0) != 0x80 || (third & 0xC0) != 0x80) {
        throw std::runtime_error(
            "Stored character nickname encoding is invalid");
      }
      result.push_back(static_cast<char16_t>(
          ((first & 15) << 12) | ((second & 63) << 6) | (third & 63)));
    }
  }
  if (EncodeNickname(result) != nickname) {
    throw std::runtime_error(
        "Stored character nickname is not canonical UTF-8");
  }
  return result;
}

CharacterRepository::CharacterRepository(
    database::MySqlDatabase& database,
    const progression::ProgressionPolicy& policy)
    : database_(database), policy_(policy) {}

boost::asio::awaitable<std::optional<CharacterProfile>>
CharacterRepository::FindByAccount(std::uint32_t account_id) {
  database::SqlCommand query{
      "SELECT characters.id, nickname, character_type, starter_selection, character_progression.experience "
      "FROM characters JOIN character_progression ON character_progression.character_id = characters.id "
      "WHERE account_id = ?",
      {boost::mysql::field(account_id)}};
  const auto result = co_await database_.Execute(std::move(query));
  if (result.rows().empty()) {
    co_return std::nullopt;
  }

  const auto row = result.rows()[0];
  CharacterProfile profile{
      static_cast<std::uint32_t>(row[0].as_uint64()),
      account_id,
      DecodeNickname(std::string(row[1].as_string())),
      static_cast<std::uint32_t>(row[2].as_uint64()),
      static_cast<std::uint32_t>(row[3].as_uint64()),
      policy_.Describe(static_cast<std::uint32_t>(row[4].as_uint64())).level};
  co_return profile;
}

boost::asio::awaitable<CreationResult> CharacterRepository::CheckName(
    std::u16string nickname) {
  const auto validation = CheckNickname(nickname);
  if (validation != CreationResult::kSuccess) {
    co_return validation;
  }

  database::SqlCommand query{"SELECT id FROM characters WHERE nickname = ?",
                             {boost::mysql::field(EncodeNickname(nickname))}};
  const auto result = co_await database_.Execute(std::move(query));
  co_return result.rows().empty() ? CreationResult::kSuccess
                                  : CreationResult::kNameTaken;
}

boost::asio::awaitable<CharacterStats> CharacterRepository::FindStats(
    std::uint32_t character_id) {
  database::SqlCommand query{
      "SELECT lucci, cash, experience FROM character_stats JOIN character_progression "
      "ON character_progression.character_id = character_stats.character_id WHERE character_stats.character_id = ?",
      {boost::mysql::field(character_id)},
      {},
      1};
  const auto result = co_await database_.Execute(std::move(query));
  const auto row = result.rows()[0];
  co_return CharacterStats{
      static_cast<std::uint32_t>(row[0].as_uint64()),
      static_cast<std::uint32_t>(row[1].as_uint64()),
      policy_.Describe(static_cast<std::uint32_t>(row[2].as_uint64()))};
}

boost::asio::awaitable<CreationResult> CharacterRepository::Create(
    std::uint32_t account_id, std::u16string nickname,
    std::uint32_t character_type, std::uint32_t starter_selection) {
  const auto validation = CheckNickname(nickname);
  if (validation != CreationResult::kSuccess) {
    co_return validation;
  }
  if (!IsStarterSelection(character_type, starter_selection)) {
    co_return CreationResult::kInvalidCharacter;
  }

  // The unique character and starter inventory must commit together, including
  // simultaneous sessions.
  database::SqlCommand insert{
      "INSERT INTO characters (account_id, nickname, character_type, starter_selection, created_at, updated_at) "
      "VALUES (?, ?, ?, ?, UTC_TIMESTAMP(6), UTC_TIMESTAMP(6))",
      {boost::mysql::field(account_id),
       boost::mysql::field(EncodeNickname(nickname)),
       boost::mysql::field(character_type),
       boost::mysql::field(starter_selection)},
      1};
  bool duplicate = false;
  try {
    std::vector<database::SqlCommand> commands;
    commands.push_back(std::move(insert));
    commands.push_back(
        {"INSERT INTO character_stats (character_id, lucci, cash, updated_at) "
         "SELECT id, 10000, 0, UTC_TIMESTAMP(6) FROM characters WHERE account_id = ?",
         {boost::mysql::field(account_id)},
         1});
    commands.push_back(
        {"INSERT INTO character_progression (character_id, experience, updated_at) "
         "SELECT id, 0, UTC_TIMESTAMP(6) FROM characters WHERE account_id = ?",
         {boost::mysql::field(account_id)},
         1});
    commands.push_back(
        {"INSERT INTO character_items (character_id, item_id, quantity, equipped_slot, created_at, updated_at) "
         "SELECT characters.id, item_definitions.id, 1, "
         "CASE WHEN item_definitions.starter_equipped THEN item_definitions.equipment_slot ELSE NULL END, "
         "UTC_TIMESTAMP(6), UTC_TIMESTAMP(6) FROM characters CROSS JOIN item_definitions "
         "WHERE characters.account_id = ? AND item_definitions.id IN (201001, 202001, 203001, 204001, 207001)",
         {boost::mysql::field(account_id)},
         5});
    commands.push_back(
        {"INSERT INTO character_items (character_id, item_id, quantity, equipped_slot, created_at, updated_at) "
         "SELECT characters.id, character_appearances.item_id, 1, 63, UTC_TIMESTAMP(6), UTC_TIMESTAMP(6) "
         "FROM characters JOIN character_appearances "
         "ON character_appearances.character_type = characters.character_type WHERE characters.account_id = ?",
         {boost::mysql::field(account_id)},
         1});
    co_await database_.ExecuteTransaction(std::move(commands));
  } catch (const boost::system::system_error& error) {
    if (error.code() != boost::mysql::common_server_errc::er_dup_entry) {
      throw;
    }
    duplicate = true;
  }
  if (duplicate) {
    if (co_await FindByAccount(account_id)) {
      co_return CreationResult::kStorageFailure;
    }
    const auto name_result = co_await CheckName(nickname);
    co_return name_result == CreationResult::kNameTaken
        ? name_result
        : CreationResult::kStorageFailure;
  }
  co_return CreationResult::kSuccess;
}
}  // namespace bubblefighter::characters
