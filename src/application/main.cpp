#include <spdlog/spdlog.h>

#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string_view>

#include "application/account_command.h"
#include "application/server_application.h"
#include "configuration/server_config.h"
#include "game/game_character_catalog.h"
#include "game/game_defense_catalog.h"
#include "game/game_map_catalog.h"
#include "game/game_respawn_catalog.h"
#include "game/game_weapon_catalog.h"
#include "logging/server_logger.h"

int wmain(int argc, wchar_t* argv[]) {
  try {
    std::filesystem::path config_path = L"config/server.json";
    bool check_only = false;
    std::optional<bubblefighter::application::AccountOperation>
        account_operation;
    std::string account_name;
    for (int index = 1; index < argc; ++index) {
      const std::wstring_view argument(argv[index]);
      if (argument == L"--help") {
        std::cout
            << "BubbleFighter_CN0261_Server [--config <path>] [--check-config]\n"
               "Database password: BF_DATABASE_PASSWORD environment variable\n"
               "Account administration: --create-account <name> or --issue-ticket <name>\n"
               "Account password: BF_ACCOUNT_PASSWORD environment variable\n";
        return 0;
      }
      if (argument == L"--config" && index + 1 < argc) {
        config_path = argv[++index];
      } else if (argument == L"--check-config") {
        check_only = true;
      } else if ((argument == L"--create-account" ||
                  argument == L"--issue-ticket") &&
                 index + 1 < argc) {
        if (account_operation) {
          throw std::invalid_argument(
              "Only one account operation may be specified");
        }
        account_operation =
            argument == L"--create-account"
                ? bubblefighter::application::AccountOperation::kCreate
                : bubblefighter::application::AccountOperation::kIssueTicket;
        for (const auto character : std::wstring_view(argv[++index])) {
          if (character > 127) {
            throw std::invalid_argument(
                "Account name must contain ASCII characters");
          }
          account_name.push_back(static_cast<char>(character));
        }
      } else {
        throw std::invalid_argument("Invalid command line; use --help");
      }
    }

    auto config = bubblefighter::configuration::LoadConfig(config_path);
    if (check_only && account_operation) {
      throw std::invalid_argument(
          "Account operations cannot be combined with --check-config");
    }
    if (check_only) {
      static_cast<void>(
          bubblefighter::game::GameMapCatalog::Load(config.game_maps_file));
      static_cast<void>(bubblefighter::game::GameWeaponCatalog::Load(
          config.game_weapons_file));
      static_cast<void>(bubblefighter::game::GameDefenseCatalog::Load(
          config.game_defense_file));
      static_cast<void>(bubblefighter::game::GameCharacterCatalog::Load(
          config.game_characters_file));
      static_cast<void>(bubblefighter::game::GameRespawnCatalog::Load(
          config.game_respawn_file));
      static_cast<void>(bubblefighter::progression::ProgressionPolicy::Load(
          config.progression_file));
      std::cout
          << "Configuration is valid. Database connectivity has not been checked.\n";
      return 0;
    }

    const auto password =
        bubblefighter::configuration::ReadEnvironment("BF_DATABASE_PASSWORD");
    if (password.empty()) {
      throw std::invalid_argument("BF_DATABASE_PASSWORD is required");
    }

    if (account_operation) {
      const auto account_password =
          bubblefighter::configuration::ReadEnvironment("BF_ACCOUNT_PASSWORD");
      std::cout << bubblefighter::application::RunAccountCommand(
                       config.database, password, *account_operation,
                       account_name, account_password)
                << '\n';
      return 0;
    }

    bubblefighter::logging::InitializeLogging(config.logging);
    bubblefighter::application::ServerApplication application(std::move(config),
                                                              password);
    return application.Run();
  } catch (const std::exception& error) {
    std::cerr << "Startup error: " << error.what() << '\n';
    return 1;
  }
}
