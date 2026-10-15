#ifndef BUBBLEFIGHTER_SERVER_CONFIGURATION_SERVER_CONFIG_H_
#define BUBBLEFIGHTER_SERVER_CONFIGURATION_SERVER_CONFIG_H_

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

namespace bubblefighter::configuration {
struct NetworkConfig {
  std::string listen_address;
  std::uint16_t listen_port{};
  std::size_t max_connections{};
  unsigned int idle_timeout_seconds{};
};

struct DatabaseConfig {
  std::string host;
  std::uint16_t port{};
  std::string database_name;
  std::string username;
  unsigned int connect_timeout_seconds{};
  unsigned int query_timeout_seconds{};
  std::size_t pool_size{};
  std::string tls_mode;
  bool verify_certificate{};
  std::string ca_file;
};

struct LoggingConfig {
  std::string level;
  std::filesystem::path directory;
  std::size_t max_file_size_bytes{};
  std::size_t max_files{};
};

struct AuthenticationConfig {
  bool enabled = false;
  std::string listen_address;
  std::uint16_t listen_port{};
  std::filesystem::path certificate_file;
  std::filesystem::path private_key_file;
  std::string game_host;
};

struct ServerConfig {
  std::filesystem::path game_maps_file = "config/game_maps.json";
  std::filesystem::path game_weapons_file = "config/game_weapons.json";
  std::filesystem::path game_defense_file = "config/game_defense.json";
  std::filesystem::path game_characters_file = "config/game_characters.json";
  std::filesystem::path game_respawn_file = "config/game_respawn.json";
  std::filesystem::path progression_file = "config/progression.json";
  NetworkConfig network;
  DatabaseConfig database;
  LoggingConfig logging;
  AuthenticationConfig authentication;
  bool relay_enabled = false;
  NetworkConfig relay;
  std::string relay_advertised_address = "127.0.0.1";
};

ServerConfig ParseConfig(std::string_view text);
ServerConfig LoadConfig(const std::filesystem::path& path);
std::string ReadEnvironment(const char* name);
}  // namespace bubblefighter::configuration

#endif  // BUBBLEFIGHTER_SERVER_CONFIGURATION_SERVER_CONFIG_H_
