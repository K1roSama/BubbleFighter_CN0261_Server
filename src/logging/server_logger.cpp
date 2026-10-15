#include "logging/server_logger.h"

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <vector>

namespace bubblefighter::logging {
void InitializeLogging(const configuration::LoggingConfig& config) {
  std::filesystem::create_directories(config.directory);
  const auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  const auto file = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      (config.directory / L"Server.log").wstring(), config.max_file_size_bytes,
      config.max_files);
  const std::vector<spdlog::sink_ptr> sinks{console, file};
  auto logger =
      std::make_shared<spdlog::logger>("Server", sinks.begin(), sinks.end());
  logger->set_pattern("[%Y-%m-%dT%H:%M:%S.%eZ] [%l] %v",
                      spdlog::pattern_time_type::utc);
  logger->set_level(spdlog::level::from_str(config.level));
  logger->flush_on(spdlog::level::info);
  spdlog::set_default_logger(std::move(logger));
}
}  // namespace bubblefighter::logging
