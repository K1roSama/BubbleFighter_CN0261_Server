#include "protocol/lambda_packet_codec.h"

#include <array>
#include <stdexcept>

namespace bubblefighter::protocol {
namespace {
// CN0261 packet tables at VA 0xEA72D0 and 0xEA7308.
constexpr std::array<std::uint8_t, 49> kFirstTable{
    0x5F, 0x3E, 0x32, 0x72, 0xC1, 0x18, 0x46, 0x87, 0x5D, 0x2E,
    0x29, 0x4E, 0x68, 0x79, 0x86, 0x6C, 0xB5, 0x8D, 0x56, 0xF1,
    0x38, 0xC1, 0x48, 0x42, 0x5F, 0x0C, 0xCD, 0x80, 0xE8, 0xE2,
    0xFE, 0xF0, 0x42, 0x5F, 0xF7, 0x59, 0x7E, 0x5F, 0xA5, 0x8D,
    0x3D, 0x76, 0xE7, 0x11, 0x41, 0x62, 0x67, 0x59, 0x7F};
constexpr std::array<std::uint8_t, 24> kSecondTable{
    0x23, 0x00, 0x24, 0x00, 0x25, 0x00, 0x73, 0x00, 0x65, 0x00, 0x72, 0x00,
    0x74, 0x00, 0x33, 0x00, 0x34, 0x00, 0x25, 0x00, 0x77, 0x00, 0x65, 0x00};

std::uint16_t ReadHeaderWord(std::span<const std::uint8_t> bytes) {
  return static_cast<std::uint16_t>(
      bytes[0] | (static_cast<std::uint16_t>(bytes[1]) << 8));
}
}  // namespace

LambdaPacketCodec::LambdaPacketCodec(PacketEndpoint endpoint) {
  receive_ = endpoint == PacketEndpoint::kServer ? CipherPosition{0, 1}
                                                 : CipherPosition{5, 3};
  send_ = endpoint == PacketEndpoint::kServer ? CipherPosition{5, 3}
                                              : CipherPosition{0, 1};
  pending_.reserve(kHeaderSize);
}

std::vector<std::uint8_t> LambdaPacketCodec::CreateGreeting() {
  // ServerConnection::OnGreeting (0x8995D0) accepts the untransformed 16-bit
  // result 2.
  std::vector<std::uint8_t> greeting(kGreetingSize, 0);
  greeting[0] = 2;
  return greeting;
}

std::uint8_t LambdaPacketCodec::Advance(CipherPosition& position) {
  const auto mask = static_cast<std::uint8_t>(kFirstTable[position.first] +
                                              kSecondTable[position.second]);
  position.first = (position.first + 1) % kFirstTable.size();
  position.second = (position.second + 1) % kSecondTable.size();
  return mask;
}

std::vector<LambdaPacket> LambdaPacketCodec::Feed(
    std::span<const std::uint8_t> bytes) {
  if (failed_) {
    throw std::logic_error(
        "LambdaPacket stream already failed; close the connection");
  }

  std::vector<LambdaPacket> packets;
  for (const auto byte : bytes) {
    pending_.push_back(static_cast<std::uint8_t>(byte - Advance(receive_)));
    if (pending_.size() == kHeaderSize) {
      expected_size_ = kHeaderSize + ReadHeaderWord(pending_);
    }

    if (pending_.size() == expected_size_) {
      ExtractPacket(packets);
    }
  }

  return packets;
}

void LambdaPacketCodec::ExtractPacket(std::vector<LambdaPacket>& packets) {
  std::uint8_t checksum = 0;
  for (std::size_t index = kHeaderSize; index < pending_.size(); ++index) {
    checksum ^= pending_[index];
  }

  if (checksum != pending_[4]) {
    failed_ = true;
    pending_.clear();
    throw std::invalid_argument("LambdaPacket payload checksum mismatch");
  }

  packets.push_back(
      {ReadHeaderWord(std::span<const std::uint8_t>(pending_).subspan(2)),
       {pending_.begin() + kHeaderSize, pending_.end()}});
  pending_.clear();
  expected_size_ = kHeaderSize;
}

std::vector<std::uint8_t> LambdaPacketCodec::Encode(
    const LambdaPacket& packet) {
  if (packet.payload.size() > kMaxPayloadSize) {
    throw std::invalid_argument("LambdaPacket payload exceeds 65535 bytes");
  }

  std::vector<std::uint8_t> bytes;
  bytes.reserve(kHeaderSize + packet.payload.size());
  bytes.push_back(static_cast<std::uint8_t>(packet.payload.size()));
  bytes.push_back(static_cast<std::uint8_t>(packet.payload.size() >> 8));
  bytes.push_back(static_cast<std::uint8_t>(packet.message_id));
  bytes.push_back(static_cast<std::uint8_t>(packet.message_id >> 8));
  bytes.push_back(0);
  for (const auto byte : packet.payload) {
    bytes[4] ^= byte;
    bytes.push_back(byte);
  }

  for (auto& byte : bytes) {
    byte = static_cast<std::uint8_t>(byte + Advance(send_));
  }

  return bytes;
}
}  // namespace bubblefighter::protocol
