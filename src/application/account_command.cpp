#include "application/account_command.h"

#include <boost/asio/co_spawn.hpp>
#include <exception>
#include <nlohmann/json.hpp>
#include <utility>

#include "features/authentication/account_repository.h"

namespace bubblefighter::application {
namespace {
boost::asio::awaitable<std::string> Execute(database::MySqlDatabase& database,
                                            AccountOperation operation,
                                            std::string account_name,
                                            std::string account_password) {
  co_await database.CheckConnection();
  co_await database.CheckSchema();
  authentication::AccountRepository accounts(database);
  if (operation == AccountOperation::kCreate) {
    const auto account = co_await accounts.CreateAccount(
        std::move(account_name), std::move(account_password));
    co_return nlohmann::json{{"accountId", account.id},
                             {"accountName", account.account_name}}
        .dump();
  }

  const auto ticket = co_await accounts.IssueLoginTicket(
      std::move(account_name), std::move(account_password));
  co_return nlohmann::json{{"loginTicket", ticket}, {"expiresInSeconds", 120}}
      .dump();
}
}  // namespace

std::string RunAccountCommand(const configuration::DatabaseConfig& config,
                              const std::string& database_password,
                              AccountOperation operation,
                              std::string account_name,
                              std::string account_password) {
  boost::asio::io_context context;
  database::MySqlDatabase database(context, config, database_password);
  database.Start();
  std::exception_ptr failure;
  std::string output;
  boost::asio::co_spawn(context,
                        Execute(database, operation, std::move(account_name),
                                std::move(account_password)),
                        [&](std::exception_ptr error, std::string result) {
                          failure = error;
                          output = std::move(result);
                          database.Stop();
                        });
  context.run();
  if (failure) {
    std::rethrow_exception(failure);
  }
  return output;
}
}  // namespace bubblefighter::application
