#include "configuration/server_config.h"

#include <Windows.h>

#include <algorithm>
#include <boost/asio/ip/address.hpp>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <memory>
#include <nlohmann/json.hpp>
#include <regex>
#include <stdexcept>
#include <vector>

namespace bubblefighter::configuration {
namespace {
using Json = nlohmann::json;

const Json& Object(const Json& parent, const char* key) {
  if (!parent.contains(key) || !parent.at(key).is_object()) {
    throw std::invalid_argument(std::string(key) + " must be an object");
  }

  return parent.at(key);
}

std::string Text(const Json& object, const char* key,
                 const std::string& section, bool allow_empty = false) {
  if (!object.contains(key) || !object.at(key).is_string()) {
    throw std::invalid_argument(section + "." + key + " must be a string");
  }

  auto value = object.at(key).get<std::string>();
  if ((!allow_empty && value.empty()) ||
      value.find('\0') != std::string::npos) {
    throw std::invalid_argument(section + "." + key +
                                " is empty or contains a null character");
  }

  return value;
}

std::uint64_t Number(const Json& object, const char* key,
                     const std::string& section, std::uint64_t maximum) {
  if (!object.contains(key) || !object.at(key).is_number_integer()) {
    throw std::invalid_argument(section + "." + key + " must be an integer");
  }

  const auto& number = object.at(key);
  if (number.is_number_unsigned()) {
    const auto value = number.get<std::uint64_t>();
    if (value >= 1 && value <= maximum) {
      return value;
    }
  } else {
    const auto value = number.get<std::int64_t>();
    if (value >= 1 && static_cast<std::uint64_t>(value) <= maximum) {
      return static_cast<std::uint64_t>(value);
    }
  }

  throw std::invalid_argument(section + "." + key + " must be between 1 and " +
                              std::to_string(maximum));
}

void Keys(const Json& object, const std::vector<std::string>& allowed,
          const std::string& section) {
  for (const auto& item : object.items()) {
    if (std::find(allowed.begin(), allowed.end(), item.key()) ==
        allowed.end()) {
      throw std::invalid_argument(section + "." + item.key() +
                                  " is an unknown field");
    }
  }
}
}  // namespace

ServerConfig ParseConfig(std::string_view text) {
  Json root;
  try {
    root = Json::parse(text);
  } catch (const Json::exception&) {
    // Parser diagnostics can include user supplied secrets. Report only the
    // location category.
    throw std::invalid_argument("Configuration is not valid JSON");
  }

  if (!root.is_object()) {
    throw std::invalid_argument("Configuration root must be an object");
  }

  Keys(root,
       {"network", "database", "logging", "authentication", "gameMapsFile",
        "gameWeaponsFile", "gameDefenseFile", "gameCharactersFile",
        "gameRespawnFile", "progressionFile", "relay"},
       "configuration");
  const auto& network = Object(root, "network");
  const auto& database = Object(root, "database");
  const auto& logging = Object(root, "logging");

  Keys(network,
       {"listenAddress", "listenPort", "maxConnections", "idleTimeoutSeconds"},
       "network");
  Keys(database,
       {"host", "port", "databaseName", "username", "connectTimeoutSeconds",
        "queryTimeoutSeconds", "poolSize", "tlsMode", "verifyCertificate",
        "caFile"},
       "database");
  Keys(logging, {"level", "directory", "maxFileSizeBytes", "maxFiles"},
       "logging");

  ServerConfig config;
  if (root.contains("gameMapsFile")) {
    const auto file = Text(root, "gameMapsFile", "configuration");
    config.game_maps_file =
        std::filesystem::path(std::u8string(file.begin(), file.end()));
  }
  if (root.contains("gameWeaponsFile")) {
    const auto file = Text(root, "gameWeaponsFile", "configuration");
    config.game_weapons_file =
        std::filesystem::path(std::u8string(file.begin(), file.end()));
  }
  if (root.contains("gameDefenseFile")) {
    const auto file = Text(root, "gameDefenseFile", "configuration");
    config.game_defense_file =
        std::filesystem::path(std::u8string(file.begin(), file.end()));
  }
  if (root.contains("gameCharactersFile")) {
    const auto file = Text(root, "gameCharactersFile", "configuration");
    config.game_characters_file =
        std::filesystem::path(std::u8string(file.begin(), file.end()));
  }
  if (root.contains("progressionFile")) {
    const auto file = Text(root, "progressionFile", "configuration");
    config.progression_file =
        std::filesystem::path(std::u8string(file.begin(), file.end()));
  }
  if (root.contains("gameRespawnFile")) {
    const auto file = Text(root, "gameRespawnFile", "configuration");
    config.game_respawn_file =
        std::filesystem::path(std::u8string(file.begin(), file.end()));
  }

  config.network.listen_address = Text(network, "listenAddress", "network");
  boost::system::error_code address_error;
  boost::asio::ip::make_address(config.network.listen_address, address_error);
  if (address_error) {
    throw std::invalid_argument(
        "network.listenAddress must be an IPv4 or IPv6 address");
  }

  config.network.listen_port = static_cast<std::uint16_t>(
      Number(network, "listenPort", "network", 65535));
  config.network.max_connections = static_cast<std::size_t>(
      Number(network, "maxConnections", "network", 65535));
  config.network.idle_timeout_seconds = static_cast<unsigned int>(
      Number(network, "idleTimeoutSeconds", "network", 86400));

  config.database.host = Text(database, "host", "database");
  config.database.port =
      static_cast<std::uint16_t>(Number(database, "port", "database", 65535));
  config.database.database_name = Text(database, "databaseName", "database");
  if (config.database.database_name != "bubblefighter_cn0261" &&
      config.database.database_name != "bubblefighter_cn0261_tests") {
    throw std::invalid_argument(
        "database.databaseName must be bubblefighter_cn0261 or bubblefighter_cn0261_tests");
  }

  config.database.username = Text(database, "username", "database");
  config.database.connect_timeout_seconds = static_cast<unsigned int>(
      Number(database, "connectTimeoutSeconds", "database", 300));
  config.database.query_timeout_seconds = static_cast<unsigned int>(
      Number(database, "queryTimeoutSeconds", "database", 300));
  config.database.pool_size =
      static_cast<std::size_t>(Number(database, "poolSize", "database", 128));
  config.database.tls_mode = Text(database, "tlsMode", "database");
  if (config.database.tls_mode != "require" &&
      config.database.tls_mode != "disable") {
    throw std::invalid_argument("database.tlsMode must be require or disable");
  }

  if (!database.contains("verifyCertificate") ||
      !database.at("verifyCertificate").is_boolean()) {
    throw std::invalid_argument("database.verifyCertificate must be a boolean");
  }

  config.database.verify_certificate =
      database.at("verifyCertificate").get<bool>();
  config.database.ca_file = Text(database, "caFile", "database", true);
  if (config.database.tls_mode == "disable" &&
      config.database.verify_certificate) {
    throw std::invalid_argument(
        "database.verifyCertificate requires database.tlsMode=require");
  }

  config.logging.level = Text(logging, "level", "logging");
  const std::vector<std::string> levels = {"trace", "debug",    "info", "warn",
                                           "error", "critical", "off"};
  if (std::find(levels.begin(), levels.end(), config.logging.level) ==
      levels.end()) {
    throw std::invalid_argument("logging.level is not a supported log level");
  }

  const auto directory = Text(logging, "directory", "logging");
  config.logging.directory =
      std::filesystem::path(std::u8string(directory.begin(), directory.end()));
  config.logging.max_file_size_bytes = static_cast<std::size_t>(
      Number(logging, "maxFileSizeBytes", "logging", 1073741824));
  config.logging.max_files =
      static_cast<std::size_t>(Number(logging, "maxFiles", "logging", 100));

  if (root.contains("authentication")) {
    const auto& authentication = Object(root, "authentication");
    Keys(authentication,
         {"enabled", "listenAddress", "listenPort", "certificateFile",
          "privateKeyFile", "gameHost"},
         "authentication");
    if (!authentication.contains("enabled") ||
        !authentication.at("enabled").is_boolean()) {
      throw std::invalid_argument("authentication.enabled must be a boolean");
    }
    config.authentication.enabled = authentication.at("enabled").get<bool>();
    config.authentication.listen_address =
        Text(authentication, "listenAddress", "authentication");
    boost::asio::ip::make_address(config.authentication.listen_address,
                                  address_error);
    if (address_error) {
      throw std::invalid_argument(
          "authentication.listenAddress must be an IPv4 or IPv6 address");
    }
    config.authentication.listen_port = static_cast<std::uint16_t>(
        Number(authentication, "listenPort", "authentication", 65535));
    if (config.authentication.enabled &&
        config.authentication.listen_port == config.network.listen_port) {
      throw std::invalid_argument(
          "authentication.listenPort must differ from network.listenPort");
    }
    const auto certificate =
        Text(authentication, "certificateFile", "authentication");
    const auto key = Text(authentication, "privateKeyFile", "authentication");
    config.authentication.certificate_file = std::filesystem::path(
        std::u8string(certificate.begin(), certificate.end()));
    config.authentication.private_key_file =
        std::filesystem::path(std::u8string(key.begin(), key.end()));
    config.authentication.game_host =
        Text(authentication, "gameHost", "authentication");
    if (config.authentication.game_host.size() > 253 ||
        config.authentication.game_host.find_first_of("\r\n=") !=
            std::string::npos) {
      throw std::invalid_argument(
          "authentication.gameHost is not a valid client host");
    }
  }
  if (root.contains("relay")) {
    const auto& relay = Object(root, "relay");
    Keys(relay,
         {"enabled", "listenAddress", "listenPort", "advertisedAddress",
          "maxConnections", "idleTimeoutSeconds"},
         "relay");
    if (!relay.contains("enabled") || !relay.at("enabled").is_boolean()) {
      throw std::invalid_argument("relay.enabled must be a boolean");
    }
    config.relay_enabled = relay.at("enabled").get<bool>();
    config.relay.listen_address = Text(relay, "listenAddress", "relay");
    boost::asio::ip::make_address_v4(config.relay.listen_address,
                                     address_error);
    if (address_error) {
      throw std::invalid_argument(
          "relay.listenAddress must be an IPv4 address");
    }
    config.relay_advertised_address = Text(relay, "advertisedAddress", "relay");
    const auto advertised = boost::asio::ip::make_address_v4(
        config.relay_advertised_address, address_error);
    if (address_error || advertised.is_unspecified() ||
        advertised.is_multicast()) {
      throw std::invalid_argument(
          "relay.advertisedAddress must be a reachable unicast IPv4 address");
    }
    config.relay.listen_port =
        static_cast<std::uint16_t>(Number(relay, "listenPort", "relay", 65535));
    config.relay.max_connections = static_cast<std::size_t>(
        Number(relay, "maxConnections", "relay", 8192));
    config.relay.idle_timeout_seconds = static_cast<unsigned int>(
        Number(relay, "idleTimeoutSeconds", "relay", 86400));
    if (config.relay_enabled &&
        (config.relay.listen_port == config.network.listen_port ||
         (config.authentication.enabled &&
          config.relay.listen_port == config.authentication.listen_port))) {
      throw std::invalid_argument(
          "relay.listenPort must differ from other enabled listener ports");
    }
  }
  return config;
}

ServerConfig LoadConfig(const std::filesystem::path& path) {
  std::ifstream stream(path, std::ios::binary);
  if (!stream) {
    throw std::runtime_error("Cannot open configuration file");
  }

  const std::string content{std::istreambuf_iterator<char>(stream),
                            std::istreambuf_iterator<char>()};
  return ParseConfig(content);
}

std::string ReadEnvironment(const char* name) {
  wchar_t* buffer = nullptr;
  std::size_t length = 0;
  const std::string key(name);
  const std::wstring wide_key(key.begin(), key.end());
  if (_wdupenv_s(&buffer, &length, wide_key.c_str()) != 0) {
    throw std::runtime_error(std::string("Cannot read environment variable ") +
                             name);
  }

  const std::unique_ptr<wchar_t, decltype(&std::free)> owner(buffer,
                                                             &std::free);
  if (buffer == nullptr) {
    return {};
  }

  const auto required = WideCharToMultiByte(
      CP_UTF8, WC_ERR_INVALID_CHARS, buffer, -1, nullptr, 0, nullptr, nullptr);
  if (required <= 0) {
    throw std::runtime_error(
        std::string("Invalid Unicode in environment variable ") + name);
  }
  std::string value(static_cast<std::size_t>(required), '\0');
  if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, buffer, -1,
                          value.data(), required, nullptr,
                          nullptr) != required) {
    throw std::runtime_error(
        std::string("Cannot decode environment variable ") + name);
  }
  value.pop_back();
  return value;
}
}  // namespace bubblefighter::configuration
