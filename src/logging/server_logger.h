#ifndef BUBBLEFIGHTER_SERVER_LOGGING_SERVER_LOGGER_H_
#define BUBBLEFIGHTER_SERVER_LOGGING_SERVER_LOGGER_H_

#include "configuration/server_config.h"

namespace bubblefighter::logging {
void InitializeLogging(const configuration::LoggingConfig& config);
}  // namespace bubblefighter::logging

#endif  // BUBBLEFIGHTER_SERVER_LOGGING_SERVER_LOGGER_H_
