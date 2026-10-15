#ifndef BUBBLEFIGHTER_SERVER_APPLICATION_ACCOUNT_COMMAND_H_
#define BUBBLEFIGHTER_SERVER_APPLICATION_ACCOUNT_COMMAND_H_

#include <string>

#include "configuration/server_config.h"

namespace bubblefighter::application {
enum class AccountOperation { kCreate, kIssueTicket };

std::string RunAccountCommand(const configuration::DatabaseConfig& config,
                              const std::string& database_password,
                              AccountOperation operation,
                              std::string account_name,
                              std::string account_password);
}  // namespace bubblefighter::application

#endif  // BUBBLEFIGHTER_SERVER_APPLICATION_ACCOUNT_COMMAND_H_
