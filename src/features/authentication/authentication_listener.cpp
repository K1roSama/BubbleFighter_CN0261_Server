#include "features/authentication/authentication_listener.h"

#include <openssl/ssl.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <array>
#include <boost/asio/cancel_after.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <fstream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace bubblefighter::authentication {
namespace {
std::string ReadPem(const std::filesystem::path& path) {
  std::ifstream input(path, std::ios::binary);
  if (!input) {
    throw std::runtime_error(
        "Cannot open authentication TLS certificate or private key file");
  }
  return {std::istreambuf_iterator<char>(input),
          std::istreambuf_iterator<char>()};
}
}  // namespace

AuthenticationListener::AuthenticationListener(
    boost::asio::io_context& context,
    configuration::AuthenticationConfig config, std::uint16_t game_port,
    AccountRepository& accounts)
    : context_(context),
      config_(std::move(config)),
      game_port_(game_port),
      accounts_(accounts),
      acceptor_(context) {}

void AuthenticationListener::Start() {
  if (!config_.enabled) {
    return;
  }
  if (SSL_CTX_set_min_proto_version(tls_.native_handle(), TLS1_2_VERSION) !=
      1) {
    throw std::runtime_error(
        "Cannot configure authentication TLS minimum version");
  }
  tls_.use_certificate_chain(
      boost::asio::buffer(ReadPem(config_.certificate_file)));
  tls_.use_private_key(boost::asio::buffer(ReadPem(config_.private_key_file)),
                       boost::asio::ssl::context::pem);
  if (SSL_CTX_check_private_key(tls_.native_handle()) != 1) {
    throw std::runtime_error(
        "Authentication TLS certificate and private key do not match");
  }
  const boost::asio::ip::tcp::endpoint endpoint(
      boost::asio::ip::make_address(config_.listen_address),
      config_.listen_port);
  acceptor_.open(endpoint.protocol());
  acceptor_.bind(endpoint);
  acceptor_.listen(32);
  boost::asio::co_spawn(context_, AcceptConnections(),
                        [this](std::exception_ptr failure) {
                          if (failure && !stopping_) {
                            spdlog::error("[Authentication] listener failed");
                            Stop();
                          }
                        });
  spdlog::info("[Authentication] TLS listener ready port={}",
               config_.listen_port);
}

bool AuthenticationListener::Admit(const std::string& address) {
  const auto now = std::chrono::steady_clock::now();
  std::erase_if(rates_, [now](const auto& entry) {
    return now - entry.second.start >= std::chrono::minutes(1);
  });
  if (rates_.size() >= 1024 && !rates_.contains(address)) {
    return false;
  }
  auto [entry, inserted] = rates_.try_emplace(address, RateWindow{now, 0});
  static_cast<void>(inserted);
  return ++entry->second.attempts <= 10;
}

boost::asio::awaitable<void> AuthenticationListener::AcceptConnections() {
  while (!stopping_) {
    auto socket = co_await acceptor_.async_accept(boost::asio::use_awaitable);
    if (streams_.size() >= 32 ||
        !Admit(socket.remote_endpoint().address().to_string())) {
      socket.close();
      continue;
    }
    auto stream = std::make_shared<Stream>(std::move(socket), tls_);
    const auto id = ++next_id_;
    streams_.emplace(id, stream);
    boost::asio::co_spawn(context_, HandleConnection(stream),
                          [this, id, stream](std::exception_ptr) {
                            boost::system::error_code ignored;
                            stream->next_layer().close(ignored);
                            streams_.erase(id);
                          });
  }
}

boost::asio::awaitable<void> AuthenticationListener::HandleConnection(
    std::shared_ptr<Stream> stream) {
  using namespace std::chrono_literals;
  co_await stream->async_handshake(
      boost::asio::ssl::stream_base::server,
      boost::asio::cancel_after(10s, boost::asio::use_awaitable));
  std::array<std::uint8_t, 4> header{};
  co_await boost::asio::async_read(
      *stream, boost::asio::buffer(header),
      boost::asio::cancel_after(10s, boost::asio::use_awaitable));
  const auto size = static_cast<std::uint32_t>(header[0]) |
                    (static_cast<std::uint32_t>(header[1]) << 8) |
                    (static_cast<std::uint32_t>(header[2]) << 16) |
                    (static_cast<std::uint32_t>(header[3]) << 24);
  if (size == 0 || size > 4096) {
    co_return;
  }
  std::string body(size, '\0');
  co_await boost::asio::async_read(
      *stream, boost::asio::buffer(body),
      boost::asio::cancel_after(10s, boost::asio::use_awaitable));
  nlohmann::json response;
  try {
    auto request = nlohmann::json::parse(body);
    std::fill(body.begin(), body.end(), '\0');
    if (!request.is_object() || request.size() != 4 ||
        request.at("version") != 1 || request.at("action") != "login") {
      throw std::invalid_argument("Invalid authentication request");
    }
    const auto ticket = co_await accounts_.IssueLoginTicket(
        request.at("accountName").get<std::string>(),
        request.at("password").get<std::string>());
    response = {{"success", true},         {"loginTicket", ticket},
                {"expiresInSeconds", 120}, {"gameHost", config_.game_host},
                {"gamePort", game_port_},  {"protocolVersion", 717}};
  } catch (const AuthenticationRejected&) {
    response = {{"success", false}, {"error", "credentialsRejected"}};
  } catch (const AuthenticationBusy&) {
    response = {{"success", false}, {"error", "serviceBusy"}};
  } catch (const nlohmann::json::exception&) {
    response = {{"success", false}, {"error", "invalidRequest"}};
  } catch (const std::invalid_argument&) {
    response = {{"success", false}, {"error", "invalidRequest"}};
  } catch (const std::exception&) {
    response = {{"success", false}, {"error", "serviceUnavailable"}};
    spdlog::warn("[Authentication] credential service operation failed");
  }
  if (stopping_) {
    co_return;
  }
  const auto text = response.dump();
  for (unsigned int index = 0; index < 4; ++index) {
    header[index] = static_cast<std::uint8_t>(text.size() >> (index * 8));
  }
  const std::array<boost::asio::const_buffer, 2> buffers{
      boost::asio::buffer(header), boost::asio::buffer(text)};
  co_await boost::asio::async_write(
      *stream, buffers,
      boost::asio::cancel_after(10s, boost::asio::use_awaitable));
  // Responses have an explicit length; the client closes after reading its
  // single response.
}

void AuthenticationListener::Stop() {
  stopping_ = true;
  boost::system::error_code ignored;
  acceptor_.close(ignored);
  for (const auto& [id, stream] : streams_) {
    static_cast<void>(id);
    stream->next_layer().close(ignored);
  }
}
}  // namespace bubblefighter::authentication
