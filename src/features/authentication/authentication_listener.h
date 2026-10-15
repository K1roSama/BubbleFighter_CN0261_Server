#ifndef BUBBLEFIGHTER_SERVER_FEATURES_AUTHENTICATION_AUTHENTICATION_LISTENER_H_
#define BUBBLEFIGHTER_SERVER_FEATURES_AUTHENTICATION_AUTHENTICATION_LISTENER_H_

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <chrono>
#include <memory>
#include <unordered_map>

#include "features/authentication/account_repository.h"

namespace bubblefighter::authentication {
class AuthenticationListener {
 public:
  AuthenticationListener(boost::asio::io_context& context,
                         configuration::AuthenticationConfig config,
                         std::uint16_t game_port, AccountRepository& accounts);
  void Start();
  void Stop();

 private:
  using Stream = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;

  struct RateWindow {
    std::chrono::steady_clock::time_point start;
    unsigned int attempts = 0;
  };

  boost::asio::awaitable<void> AcceptConnections();
  boost::asio::awaitable<void> HandleConnection(std::shared_ptr<Stream> stream);
  bool Admit(const std::string& address);

  boost::asio::io_context& context_;
  configuration::AuthenticationConfig config_;
  std::uint16_t game_port_;
  AccountRepository& accounts_;
  boost::asio::ssl::context tls_{boost::asio::ssl::context::tls_server};
  boost::asio::ip::tcp::acceptor acceptor_;
  std::unordered_map<std::uint64_t, std::shared_ptr<Stream>> streams_;
  std::unordered_map<std::string, RateWindow> rates_;
  std::uint64_t next_id_ = 0;
  bool stopping_ = false;
};
}  // namespace bubblefighter::authentication

#endif  // BUBBLEFIGHTER_SERVER_FEATURES_AUTHENTICATION_AUTHENTICATION_LISTENER_H_
