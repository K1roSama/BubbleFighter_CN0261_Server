#ifndef BUBBLEFIGHTER_SERVER_NETWORK_TCP_LISTENER_H_
#define BUBBLEFIGHTER_SERVER_NETWORK_TCP_LISTENER_H_

#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include <unordered_map>
#include <vector>

#include "configuration/server_config.h"

namespace bubblefighter::network {
class TcpSession;

// Called on the event loop. The span is valid only until the callback returns.
using BytesHandler =
    std::function<void(std::uint64_t, std::span<const std::uint8_t>)>;
using SessionHandler = std::function<void(std::uint64_t)>;

class TcpListener {
 public:
  TcpListener(boost::asio::io_context& context,
              configuration::NetworkConfig config, BytesHandler handler,
              SessionHandler connected = {}, SessionHandler disconnected = {});
  ~TcpListener();

  TcpListener(const TcpListener&) = delete;
  TcpListener& operator=(const TcpListener&) = delete;

  // Start/Stop and destruction must be serialized on the owning event loop.
  void Start();
  void Stop();
  bool Send(std::uint64_t session_id, std::vector<std::uint8_t> bytes);
  void Disconnect(std::uint64_t session_id);
  void CloseAfterSend(std::uint64_t session_id);
  boost::asio::ip::tcp::endpoint GetPeerEndpoint(
      std::uint64_t session_id) const;
  boost::asio::ip::tcp::endpoint GetServerEndpoint(
      std::uint64_t session_id) const;
  std::uint16_t GetPort() const;

 private:
  boost::asio::awaitable<void> Accept();

  boost::asio::ip::tcp::acceptor acceptor_;
  configuration::NetworkConfig config_;
  BytesHandler handler_;
  SessionHandler connected_;
  SessionHandler disconnected_;
  std::unordered_map<std::uint64_t, std::shared_ptr<TcpSession>> sessions_;
  std::uint64_t next_session_id_ = 1;
  bool stopped_ = true;
};
}  // namespace bubblefighter::network

#endif  // BUBBLEFIGHTER_SERVER_NETWORK_TCP_LISTENER_H_
