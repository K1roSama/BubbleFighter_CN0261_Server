#include "persistence/my_sql_database.h"

#include <boost/asio/cancel_after.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/ssl/host_name_verification.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/mysql/statement.hpp>
#include <chrono>
#include <exception>
#include <stdexcept>
#include <utility>

namespace bubblefighter::database {
namespace {
boost::mysql::pool_params MakePoolParams(
    const configuration::DatabaseConfig& config, const std::string& password) {
  boost::mysql::pool_params parameters;
  parameters.server_address.emplace_host_and_port(config.host, config.port);
  parameters.username = config.username;
  parameters.password = password;
  parameters.database = config.database_name;
  parameters.initial_size = 1;
  parameters.max_size = config.pool_size;
  parameters.connect_timeout =
      std::chrono::seconds(config.connect_timeout_seconds);
  parameters.ping_timeout = std::chrono::seconds(config.query_timeout_seconds);
  parameters.retry_interval = std::chrono::seconds(1);
  parameters.ssl = config.tls_mode == "require"
                       ? boost::mysql::ssl_mode::require
                       : boost::mysql::ssl_mode::disable;

  if (config.tls_mode == "require") {
    boost::asio::ssl::context tls(boost::asio::ssl::context::tls_client);
    if (config.verify_certificate) {
      tls.set_verify_mode(boost::asio::ssl::verify_peer);
      tls.set_verify_callback(
          boost::asio::ssl::host_name_verification(config.host));
      if (config.ca_file.empty()) {
        tls.set_default_verify_paths();
      } else {
        tls.load_verify_file(config.ca_file);
      }
    } else {
      tls.set_verify_mode(boost::asio::ssl::verify_none);
    }
    parameters.ssl_ctx = std::move(tls);
  }

  return parameters;
}
}  // namespace

MySqlDatabase::MySqlDatabase(boost::asio::io_context& context,
                             configuration::DatabaseConfig config,
                             const std::string& password)
    : config_(std::move(config)),
      pool_(context, MakePoolParams(config_, password)) {}

void MySqlDatabase::Start() { pool_.async_run(boost::asio::detached); }

void MySqlDatabase::Stop() { pool_.cancel(); }

boost::asio::awaitable<boost::mysql::pooled_connection>
MySqlDatabase::Acquire() {
  boost::mysql::diagnostics diagnostics;
  boost::system::error_code error;
  auto connection = co_await pool_.async_get_connection(
      diagnostics,
      boost::asio::cancel_after(
          std::chrono::seconds(config_.connect_timeout_seconds),
          boost::asio::redirect_error(boost::asio::use_awaitable, error)));
  if (error) {
    const auto detail = diagnostics.server_message().empty()
                            ? diagnostics.client_message()
                            : diagnostics.server_message();
    if (detail.find("Access denied") != boost::mysql::string_view::npos) {
      throw std::runtime_error(
          "Database authentication failed; verify database.username and BF_DATABASE_PASSWORD");
    }
    if (detail.find("Unknown database") != boost::mysql::string_view::npos) {
      throw std::runtime_error(
          "Database schema not found; run the initialization SQL");
    }
    if (detail.find("certificate") != boost::mysql::string_view::npos) {
      throw std::runtime_error(
          "Database TLS certificate verification failed; check database.host and database.caFile");
    }
    throw boost::system::system_error(error);
  }

  boost::mysql::results result;
  co_await connection->async_execute(
      "SET time_zone = '+00:00'", result,
      boost::asio::cancel_after(
          std::chrono::seconds(config_.query_timeout_seconds),
          boost::asio::use_awaitable));
  co_return std::move(connection);
}

boost::asio::awaitable<boost::mysql::results> MySqlDatabase::ExecuteOn(
    boost::mysql::any_connection& connection, const SqlCommand& command) {
  const auto timeout = std::chrono::seconds(config_.query_timeout_seconds);
  const auto statement = co_await connection.async_prepare_statement(
      command.sql,
      boost::asio::cancel_after(timeout, boost::asio::use_awaitable));
  boost::mysql::results results;
  co_await connection.async_execute(
      statement.bind(command.parameters.begin(), command.parameters.end()),
      results, boost::asio::cancel_after(timeout, boost::asio::use_awaitable));
  co_await connection.async_close_statement(
      statement,
      boost::asio::cancel_after(timeout, boost::asio::use_awaitable));
  if (command.expected_affected_rows &&
      results.affected_rows() != *command.expected_affected_rows) {
    throw SqlPreconditionFailed();
  }
  if (command.expected_rows &&
      results.rows().size() != *command.expected_rows) {
    throw SqlPreconditionFailed();
  }
  co_return results;
}

boost::asio::awaitable<boost::mysql::results> MySqlDatabase::Execute(
    SqlCommand command) {
  auto connection = co_await Acquire();
  co_return co_await ExecuteOn(connection.get(), command);
}

boost::asio::awaitable<void> MySqlDatabase::CheckConnection() {
  SqlCommand command{"SELECT 1", {}};
  const auto result = co_await Execute(std::move(command));
  if (result.rows().size() != 1 || result.rows()[0][0].as_int64() != 1) {
    throw std::runtime_error(
        "Database health query returned an unexpected result");
  }
}

boost::asio::awaitable<void> MySqlDatabase::CheckSchema() {
  SqlCommand command{
      "SELECT version, name FROM schema_migrations ORDER BY version", {}};
  const auto result = co_await Execute(std::move(command));
  if (result.rows().size() != 14 || result.rows()[0][0].as_uint64() != 1 ||
      result.rows()[0][1].as_string() != "InitializeSchema" ||
      result.rows()[1][0].as_uint64() != 2 ||
      result.rows()[1][1].as_string() != "CreateAccounts" ||
      result.rows()[2][0].as_uint64() != 3 ||
      result.rows()[2][1].as_string() != "CreateCharacters" ||
      result.rows()[3][0].as_uint64() != 4 ||
      result.rows()[3][1].as_string() != "CreateInventory" ||
      result.rows()[4][0].as_uint64() != 5 ||
      result.rows()[4][1].as_string() != "CreateCharacterStats" ||
      result.rows()[5][0].as_uint64() != 6 ||
      result.rows()[5][1].as_string() != "CreateShop" ||
      result.rows()[6][0].as_uint64() != 7 ||
      result.rows()[6][1].as_string() != "CreateGameResults" ||
      result.rows()[7][0].as_uint64() != 8 ||
      result.rows()[7][1].as_string() != "ExpandShopCosmetics" ||
      result.rows()[8][0].as_uint64() != 9 ||
      result.rows()[8][1].as_string() != "CreateShopCharacters" ||
      result.rows()[9][0].as_uint64() != 10 ||
      result.rows()[9][1].as_string() != "ExpandShopEquipment" ||
      result.rows()[10][0].as_uint64() != 11 ||
      result.rows()[10][1].as_string() != "CreateRespawnEquipment" ||
      result.rows()[11][0].as_uint64() != 12 ||
      result.rows()[11][1].as_string() != "CreateResultAwards" ||
      result.rows()[12][0].as_uint64() != 13 ||
      result.rows()[12][1].as_string() != "CreateWeaponPresets" ||
      result.rows()[13][0].as_uint64() != 14 ||
      result.rows()[13][1].as_string() != "ExpandShopOther") {
    throw std::runtime_error(
        "Database schema version mismatch: expected versions 1 through 14 (ExpandShopOther)");
  }
}

boost::asio::awaitable<std::vector<boost::mysql::results>>
MySqlDatabase::ExecuteTransaction(std::vector<SqlCommand> commands) {
  std::vector<boost::mysql::results> results;
  co_await ExecuteTransaction(
      [&commands,
       &results](SqlTransaction& transaction) -> boost::asio::awaitable<void> {
        for (auto& command : commands) {
          results.push_back(co_await transaction.Execute(std::move(command)));
        }
      });
  co_return results;
}

SqlTransaction::SqlTransaction(MySqlDatabase& database,
                               boost::mysql::any_connection& connection)
    : database_(database), connection_(connection) {}

boost::asio::awaitable<boost::mysql::results> SqlTransaction::Execute(
    SqlCommand command) {
  co_return co_await database_.ExecuteOn(connection_, command);
}

boost::asio::awaitable<void> MySqlDatabase::ExecuteTransaction(
    std::function<boost::asio::awaitable<void>(SqlTransaction&)> operation) {
  auto connection = co_await Acquire();
  const auto timeout = std::chrono::seconds(config_.query_timeout_seconds);
  boost::mysql::results control;
  co_await connection->async_execute(
      "START TRANSACTION", control,
      boost::asio::cancel_after(timeout, boost::asio::use_awaitable));

  SqlTransaction transaction(*this, connection.get());
  std::exception_ptr failure;
  try {
    co_await operation(transaction);
    co_await connection->async_execute(
        "COMMIT", control,
        boost::asio::cancel_after(timeout, boost::asio::use_awaitable));
  } catch (...) {
    failure = std::current_exception();
  }

  if (failure) {
    try {
      co_await connection->async_execute(
          "ROLLBACK", control,
          boost::asio::cancel_after(timeout, boost::asio::use_awaitable));
    } catch (...) {
      // Returning to the pool resets or reconnects the failed connection.
    }
    std::rethrow_exception(failure);
  }
}
}  // namespace bubblefighter::database
