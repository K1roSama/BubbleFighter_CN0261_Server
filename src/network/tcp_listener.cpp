#include "network/tcp_listener.h"

#include <spdlog/spdlog.h>

#include <array>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <chrono>
#include <deque>
#include <exception>
#include <utility>

namespace bubblefighter::network {
class TcpSession : public std::enable_shared_from_this<TcpSession> {
 public:
  TcpSession(boost::asio::ip::tcp::socket socket, std::uint64_t id,
             unsigned int timeout, BytesHandler handler)
      : socket_(std::move(socket)),
        timer_(socket_.get_executor()),
        id_(id),
        timeout_(timeout),
        handler_(std::move(handler)) {}

  boost::asio::awaitable<void> Run() {
    spdlog::info("[Network] session={} connected", id_);
    std::array<std::uint8_t, 8192> buffer{};
    while (socket_.is_open()) {
      timer_.expires_after(std::chrono::seconds(timeout_));
      timer_.async_wait(
          [self = shared_from_this()](boost::system::error_code error) {
            if (!error) {
              spdlog::info("[Network] session={} idle timeout", self->id_);
              self->Stop();
            }
          });

      boost::system::error_code error;
      const auto size = co_await socket_.async_read_some(
          boost::asio::buffer(buffer),
          boost::asio::redirect_error(boost::asio::use_awaitable, error));
      if (error) {
        if (error != boost::asio::error::eof &&
            error != boost::asio::error::operation_aborted) {
          spdlog::debug("[Network] session={} receive failed: {}", id_,
                        error.message());
        }
        break;
      }

      if (handler_) {
        handler_(id_, std::span<const std::uint8_t>(buffer.data(), size));
      }
    }

    Stop();
    spdlog::info("[Network] session={} disconnected", id_);
  }

  void Stop() {
    timer_.cancel();
    boost::system::error_code ignored;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored);
    socket_.close(ignored);
  }

  bool Send(std::vector<std::uint8_t> bytes) {
    if (!socket_.is_open() || closing_) {
      return false;
    }

    if (bytes.size() > kMaxQueuedBytes - queued_bytes_) {
      spdlog::warn("[Network] session={} send queue limit reached", id_);
      Stop();
      return false;
    }

    if (bytes.empty()) {
      return true;
    }

    queued_bytes_ += bytes.size();
    outgoing_.push_back(std::move(bytes));
    if (!writing_) {
      writing_ = true;
      boost::asio::co_spawn(
          socket_.get_executor(), Write(),
          [self = shared_from_this()](std::exception_ptr failure) {
            if (failure) {
              self->writing_ = false;
              spdlog::error("[Network] session={} send failed", self->id_);
              self->Stop();
            }
          });
    }

    return true;
  }

  void CloseAfterSend() {
    closing_ = true;
    if (!writing_) {
      Stop();
    }
  }

  boost::asio::ip::tcp::endpoint GetPeerEndpoint() const {
    return socket_.remote_endpoint();
  }

  boost::asio::ip::tcp::endpoint GetServerEndpoint() const {
    return socket_.local_endpoint();
  }

 private:
  boost::asio::awaitable<void> Write() {
    while (socket_.is_open() && !outgoing_.empty()) {
      auto bytes = std::move(outgoing_.front());
      outgoing_.pop_front();
      boost::system::error_code error;
      co_await boost::asio::async_write(
          socket_, boost::asio::buffer(bytes),
          boost::asio::redirect_error(boost::asio::use_awaitable, error));
      queued_bytes_ -= bytes.size();
      if (error) {
        Stop();
        break;
      }
    }

    outgoing_.clear();
    queued_bytes_ = 0;
    writing_ = false;
    if (closing_) {
      Stop();
    }
  }

  static constexpr std::size_t kMaxQueuedBytes = 8 * 1024 * 1024;

  boost::asio::ip::tcp::socket socket_;
  boost::asio::steady_timer timer_;
  std::uint64_t id_;
  unsigned int timeout_;
  BytesHandler handler_;
  std::deque<std::vector<std::uint8_t>> outgoing_;
  std::size_t queued_bytes_ = 0;
  bool writing_ = false;
  bool closing_ = false;
};

TcpListener::TcpListener(boost::asio::io_context& context,
                         configuration::NetworkConfig config,
                         BytesHandler handler, SessionHandler connected,
                         SessionHandler disconnected)
    : acceptor_(context),
      config_(std::move(config)),
      handler_(std::move(handler)),
      connected_(std::move(connected)),
      disconnected_(std::move(disconnected)) {}

TcpListener::~TcpListener() { Stop(); }

void TcpListener::Start() {
  const boost::asio::ip::tcp::endpoint endpoint(
      boost::asio::ip::make_address(config_.listen_address),
      config_.listen_port);
  acceptor_.open(endpoint.protocol());
  acceptor_.bind(endpoint);
  acceptor_.listen();
  stopped_ = false;

  boost::asio::co_spawn(acceptor_.get_executor(), Accept(),
                        [this](std::exception_ptr error) {
                          if (error && !stopped_) {
                            // Propagate fatal listener errors to the
                            // application run boundary.
                            std::rethrow_exception(error);
                          }
                        });
}

boost::asio::awaitable<void> TcpListener::Accept() {
  while (!stopped_) {
    boost::system::error_code error;
    auto socket = co_await acceptor_.async_accept(
        boost::asio::redirect_error(boost::asio::use_awaitable, error));
    if (stopped_) {
      break;
    }

    if (error) {
      if (error == boost::asio::error::connection_aborted) {
        continue;
      }
      throw boost::system::system_error(error);
    }

    if (sessions_.size() >= config_.max_connections) {
      spdlog::warn("[Network] connection rejected: capacity reached");
      socket.close(error);
      continue;
    }

    const auto id = next_session_id_++;
    auto session = std::make_shared<TcpSession>(
        std::move(socket), id, config_.idle_timeout_seconds, handler_);
    sessions_.emplace(id, session);
    try {
      if (connected_) {
        connected_(id);
      }
    } catch (const std::exception&) {
      spdlog::error("[Network] session={} connection handler failed", id);
      session->Stop();
    }

    boost::asio::co_spawn(
        acceptor_.get_executor(), session->Run(),
        [this, session, id](std::exception_ptr failure) {
          if (failure) {
            spdlog::error("[Network] session={} byte handler failed", id);
            session->Stop();
          }
          sessions_.erase(id);
          if (disconnected_) {
            try {
              disconnected_(id);
            } catch (const std::exception&) {
              spdlog::error("[Network] session={} disconnect handler failed",
                            id);
            }
          }
        });
  }
}

void TcpListener::Stop() {
  stopped_ = true;
  boost::system::error_code ignored;
  acceptor_.close(ignored);
  for (const auto& [id, session] : sessions_) {
    static_cast<void>(id);
    session->Stop();
  }
}

bool TcpListener::Send(std::uint64_t session_id,
                       std::vector<std::uint8_t> bytes) {
  const auto found = sessions_.find(session_id);
  return found != sessions_.end() && found->second->Send(std::move(bytes));
}

void TcpListener::Disconnect(std::uint64_t session_id) {
  const auto found = sessions_.find(session_id);
  if (found != sessions_.end()) {
    found->second->Stop();
  }
}

void TcpListener::CloseAfterSend(std::uint64_t session_id) {
  const auto found = sessions_.find(session_id);
  if (found != sessions_.end()) {
    found->second->CloseAfterSend();
  }
}

boost::asio::ip::tcp::endpoint TcpListener::GetPeerEndpoint(
    std::uint64_t session_id) const {
  return sessions_.at(session_id)->GetPeerEndpoint();
}

boost::asio::ip::tcp::endpoint TcpListener::GetServerEndpoint(
    std::uint64_t session_id) const {
  return sessions_.at(session_id)->GetServerEndpoint();
}

std::uint16_t TcpListener::GetPort() const {
  return acceptor_.local_endpoint().port();
}
}  // namespace bubblefighter::network
