#include "features/authentication/account_repository.h"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <limits>
#include <utility>

#include "features/authentication/password_credentials.h"

namespace bubblefighter::authentication {
namespace {
class PasswordJob {
 public:
  explicit PasswordJob(std::size_t& count) : count_(count) {
    if (count_ >= 8) {
      throw AuthenticationBusy();
    }
    ++count_;
  }

  ~PasswordJob() { --count_; }

  PasswordJob(const PasswordJob&) = delete;
  PasswordJob& operator=(const PasswordJob&) = delete;

 private:
  std::size_t& count_;
};

boost::asio::awaitable<PasswordCredentials> DeriveCredentials(
    std::string password) {
  co_return CreatePasswordCredentials(password);
}

boost::asio::awaitable<bool> CheckPassword(std::string password,
                                           PasswordCredentials credentials) {
  co_return VerifyPassword(password, credentials);
}
}  // namespace

AccountRepository::AccountRepository(database::MySqlDatabase& database)
    : database_(database) {}

boost::asio::awaitable<AccountIdentity> AccountRepository::CreateAccount(
    std::string account_name, std::string password) {
  ValidateAccountName(account_name);
  PasswordJob job(password_jobs_);
  const auto credentials = co_await boost::asio::co_spawn(
      password_workers_, DeriveCredentials(std::move(password)),
      boost::asio::use_awaitable);
  database::SqlCommand command{
      "INSERT INTO accounts (account_name, password_scheme, password_salt, password_hash, created_at, updated_at) "
      "VALUES (?, ?, ?, ?, UTC_TIMESTAMP(6), UTC_TIMESTAMP(6))",
      {boost::mysql::field(account_name),
       boost::mysql::field(std::string(kPasswordScheme)),
       boost::mysql::field(credentials.salt),
       boost::mysql::field(credentials.digest)}};
  const auto result = co_await database_.Execute(std::move(command));
  if (result.last_insert_id() == 0 ||
      result.last_insert_id() > std::numeric_limits<std::uint32_t>::max()) {
    throw std::runtime_error("Account identifier is outside the CN0261 range");
  }
  co_return AccountIdentity{static_cast<std::uint32_t>(result.last_insert_id()),
                            std::move(account_name)};
}

boost::asio::awaitable<std::string> AccountRepository::IssueLoginTicket(
    std::string account_name, std::string password,
    unsigned int lifetime_seconds) {
  ValidateAccountName(account_name);
  ValidatePassword(password);
  PasswordJob job(password_jobs_);
  if (lifetime_seconds < 5 || lifetime_seconds > 600) {
    throw std::invalid_argument(
        "Login ticket lifetime must be between 5 and 600 seconds");
  }
  database::SqlCommand query{
      "SELECT id, password_scheme, password_salt, password_hash, disabled FROM accounts WHERE account_name = ?",
      {boost::mysql::field(account_name)}};
  const auto rows = co_await database_.Execute(std::move(query));
  PasswordCredentials credentials{std::string(32, '0'), std::string(64, '0')};
  bool eligible = false;
  std::uint64_t account_id = 0;
  if (rows.rows().size() == 1) {
    const auto row = rows.rows()[0];
    account_id = row[0].as_uint64();
    eligible = row[1].as_string() == kPasswordScheme && row[4].as_int64() == 0;
    credentials.salt = row[2].as_string();
    credentials.digest = row[3].as_string();
  }
  // Apply the same derivation cost when no matching account exists.
  const auto verified = co_await boost::asio::co_spawn(
      password_workers_,
      CheckPassword(std::move(password), std::move(credentials)),
      boost::asio::use_awaitable);
  if (!eligible || !verified) {
    throw AuthenticationRejected();
  }

  const auto token = CreateLoginToken();
  database::SqlCommand insert{
      "INSERT INTO login_tickets (account_id, token_hash, expires_at, created_at) "
      "SELECT id, ?, TIMESTAMPADD(SECOND, ?, UTC_TIMESTAMP(6)), UTC_TIMESTAMP(6) "
      "FROM accounts WHERE id = ? AND disabled = 0",
      {boost::mysql::field(HashLoginToken(token)),
       boost::mysql::field(lifetime_seconds), boost::mysql::field(account_id)},
      1};
  try {
    co_await database_.Execute(std::move(insert));
  } catch (const database::SqlPreconditionFailed&) {
    throw AuthenticationRejected();
  }
  co_return token;
}

boost::asio::awaitable<std::optional<AccountIdentity>>
AccountRepository::ConsumeLoginTicket(std::string token) {
  if (!IsLoginToken(token)) {
    co_return std::nullopt;
  }
  const auto digest = HashLoginToken(token);
  std::vector<database::SqlCommand> commands;
  commands.push_back(
      {"UPDATE login_tickets INNER JOIN accounts ON accounts.id = login_tickets.account_id "
       "SET login_tickets.consumed_at = UTC_TIMESTAMP(6) WHERE login_tickets.token_hash = ? "
       "AND login_tickets.consumed_at IS NULL AND login_tickets.expires_at > UTC_TIMESTAMP(6) AND "
       "accounts.disabled = 0",
       {boost::mysql::field(digest)},
       1});
  commands.push_back(
      {"SELECT accounts.id, accounts.account_name FROM accounts INNER JOIN login_tickets "
       "ON login_tickets.account_id = accounts.id WHERE login_tickets.token_hash = ?",
       {boost::mysql::field(digest)}});
  std::vector<boost::mysql::results> results;
  try {
    results = co_await database_.ExecuteTransaction(std::move(commands));
  } catch (const database::SqlPreconditionFailed&) {
    co_return std::nullopt;
  }
  const auto row = results.at(1).rows().at(0);
  co_return AccountIdentity{static_cast<std::uint32_t>(row[0].as_uint64()),
                            std::string(row[1].as_string())};
}
}  // namespace bubblefighter::authentication
