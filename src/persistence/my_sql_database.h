#ifndef BUBBLEFIGHTER_SERVER_PERSISTENCE_MY_SQL_DATABASE_H_
#define BUBBLEFIGHTER_SERVER_PERSISTENCE_MY_SQL_DATABASE_H_

#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/mysql/connection_pool.hpp>
#include <boost/mysql/field.hpp>
#include <boost/mysql/results.hpp>
#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "configuration/server_config.h"

namespace bubblefighter::database {
class MySqlDatabase;

struct SqlCommand {
  std::string sql;
  std::vector<boost::mysql::field> parameters;
  std::optional<std::uint64_t> expected_affected_rows{};
  std::optional<std::size_t> expected_rows{};
};

class SqlPreconditionFailed : public std::runtime_error {
 public:
  SqlPreconditionFailed()
      : std::runtime_error(
            "Database operation precondition was not satisfied") {}
};

// A transaction-scoped executor. It cannot outlive its ExecuteTransaction
// callback.
class SqlTransaction {
 public:
  SqlTransaction(const SqlTransaction&) = delete;
  SqlTransaction& operator=(const SqlTransaction&) = delete;
  boost::asio::awaitable<boost::mysql::results> Execute(SqlCommand command);

 private:
  friend class MySqlDatabase;
  SqlTransaction(MySqlDatabase& database,
                 boost::mysql::any_connection& connection);
  MySqlDatabase& database_;
  boost::mysql::any_connection& connection_;
};

// All operations are serialized on the supplied io_context. Commands own their
// parameter values.
class MySqlDatabase {
 public:
  MySqlDatabase(boost::asio::io_context& context,
                configuration::DatabaseConfig config,
                const std::string& password);

  void Start();
  void Stop();
  boost::asio::awaitable<void> CheckConnection();
  boost::asio::awaitable<void> CheckSchema();
  boost::asio::awaitable<boost::mysql::results> Execute(SqlCommand command);
  boost::asio::awaitable<std::vector<boost::mysql::results>> ExecuteTransaction(
      std::vector<SqlCommand> commands);
  boost::asio::awaitable<void> ExecuteTransaction(
      std::function<boost::asio::awaitable<void>(SqlTransaction&)> operation);

 private:
  friend class SqlTransaction;
  boost::asio::awaitable<boost::mysql::pooled_connection> Acquire();
  boost::asio::awaitable<boost::mysql::results> ExecuteOn(
      boost::mysql::any_connection& connection, const SqlCommand& command);

  configuration::DatabaseConfig config_;
  boost::mysql::connection_pool pool_;
};
}  // namespace bubblefighter::database

#endif  // BUBBLEFIGHTER_SERVER_PERSISTENCE_MY_SQL_DATABASE_H_
