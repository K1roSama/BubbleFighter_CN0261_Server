#include "game/client_connection.h"

#include <stdexcept>
#include <utility>

#include "protocol/lobby_messages.h"
#include "protocol/packet_buffer.h"

namespace bubblefighter::game {
ClientConnection::ClientConnection(SendHandler send, PacketHandler packet)
    : send_(std::move(send)), packet_(std::move(packet)) {
  if (!send_ || !packet_) {
    throw std::invalid_argument(
        "ClientConnection requires send and packet handlers");
  }
}

void ClientConnection::Start() {
  if (started_) {
    throw std::logic_error("ClientConnection was already started");
  }

  started_ = true;
  send_(protocol::LambdaPacketCodec::CreateGreeting());
}

void ClientConnection::Receive(std::span<const std::uint8_t> bytes) {
  if (!started_) {
    throw std::logic_error("ClientConnection has not started");
  }

  for (const auto& packet : codec_.Feed(bytes)) {
    if (!initial_request_received_) {
      if (packet.message_id == 14) {
        // 0x898710 sends C14 directly after the transport greeting on a channel
        // connection.
        static_cast<void>(protocol::ReadMoveIntoRequest(packet));
        initial_request_received_ = true;
        packet_(packet);
      } else {
        HandleFirstConnect(packet);
      }
    } else {
      if (packet.message_id == 1 || packet.message_id == 14) {
        throw std::invalid_argument("Duplicate initial connection request");
      }

      packet_(packet);
    }
  }
}

void ClientConnection::Send(const protocol::LambdaPacket& packet) {
  send_(codec_.Encode(packet));
}

bool ClientConnection::HasCompletedHandshake() const {
  return initial_request_received_;
}

void ClientConnection::HandleFirstConnect(
    const protocol::LambdaPacket& packet) {
  if (packet.message_id != 1) {
    throw std::invalid_argument("Expected CN0261 FirstConnect packet");
  }

  protocol::PacketReader reader(packet.payload);
  if (reader.ReadUnsigned() != 717) {
    throw std::invalid_argument(
        "Unsupported client protocol version; expected 717");
  }

  // VA 0x8CFCE0: seven strings, one scalar, fifteen strings. Their business
  // meanings remain unconfirmed.
  for (int index = 0; index < 7; ++index) {
    static_cast<void>(reader.ReadString());
  }
  static_cast<void>(reader.ReadUnsigned());
  for (int index = 0; index < 15; ++index) {
    static_cast<void>(reader.ReadString());
  }
  reader.RequireEnd();

  // VA 0x8BC720: content flags only insert missing keys; existing keys are
  // never overwritten. The launcher adapter disables preconfigured flag 1004
  // before LoginStage (VA 0x6DA700).
  protocol::PacketWriter operator_login;
  operator_login.WriteUnsigned(1004);
  operator_login.WriteUnsigned(0);
  Send({960, operator_login.GetBytes()});
  protocol::PacketWriter launcher_login;
  launcher_login.WriteUnsigned(1503);
  launcher_login.WriteUnsigned(1);
  Send({960, launcher_login.GetBytes()});

  protocol::PacketWriter starter_selection;
  starter_selection.WriteUnsigned(483);
  starter_selection.WriteUnsigned(0);
  Send({960, starter_selection.GetBytes()});

  // gspRepGamePatch, dispatch 0x8C0080 -> 0x89D7E0: 32-bit false followed by an
  // empty UTF-16 string.
  protocol::PacketWriter response;
  response.WriteUnsigned(0);
  response.WriteString(u"");
  Send({2, response.GetBytes()});

  // VA 0x982650 explicitly treats an empty CenterList as one center on the
  // current connection. VA 0x990660 waits for this message before leaving the
  // startup loading screen.
  protocol::PacketWriter centers;
  centers.WriteUnsigned(0);
  Send({17, centers.GetBytes()});
  initial_request_received_ = true;
}
}  // namespace bubblefighter::game
