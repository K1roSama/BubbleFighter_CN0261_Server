#ifndef BUBBLEFIGHTER_SERVER_FEATURES_AUTHENTICATION_ACCOUNT_REPOSITORY_H_
#define BUBBLEFIGHTER_SERVER_FEATURES_AUTHENTICATION_ACCOUNT_REPOSITORY_H_

#include <boost/asio/thread_pool.hpp>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>

#include "persistence/my_sql_database.h"

namespace bubblefighter::authentication {
struct AccountIdentity {
  std::uint32_t id;
  std::string account_name;
};

class AuthenticationRejected : public std::runtime_error {
 public:
  AuthenticationRejected()
      : std::runtime_error("Account credentials were rejected") {}
};

class AuthenticationBusy : public std::runtime_error {
 public:
  AuthenticationBusy()
      : std::runtime_error("Authentication capacity is currently exhausted") {}
};

class AccountRepository {
 public:
  explicit AccountRepository(database::MySqlDatabase& database);
  boost::asio::awaitable<AccountIdentity> CreateAccount(
      std::string account_name, std::string password);
  boost::asio::awaitable<std::string> IssueLoginTicket(
      std::string account_name, std::string password,
      unsigned int lifetime_seconds = 120);
  boost::asio::awaitable<std::optional<AccountIdentity>> ConsumeLoginTicket(
      std::string token);

 private:
  database::MySqlDatabase& database_;
  boost::asio::thread_pool password_workers_{2};
  std::size_t password_jobs_ = 0;
};
}  // namespace bubblefighter::authentication

#endif  // BUBBLEFIGHTER_SERVER_FEATURES_AUTHENTICATION_ACCOUNT_REPOSITORY_H_
