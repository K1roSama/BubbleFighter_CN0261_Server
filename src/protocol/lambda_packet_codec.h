#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_LAMBDA_PACKET_CODEC_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_LAMBDA_PACKET_CODEC_H_

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace bubblefighter::protocol {
struct LambdaPacket {
  std::uint16_t message_id{};
  std::vector<std::uint8_t> payload;
};

enum class PacketEndpoint { kServer, kClient };

// Shared Lambda TCP transport. Each connection owns one codec; cipher positions
// span packet boundaries.
class LambdaPacketCodec {
 public:
  explicit LambdaPacketCodec(PacketEndpoint endpoint);

  static constexpr std::size_t kHeaderSize = 5;
  static constexpr std::size_t kMaxPayloadSize = 65535;
  static constexpr std::size_t kGreetingSize = 258;

  static std::vector<std::uint8_t> CreateGreeting();
  std::vector<LambdaPacket> Feed(std::span<const std::uint8_t> bytes);
  std::vector<std::uint8_t> Encode(const LambdaPacket& packet);

 private:
  struct CipherPosition {
    std::size_t first{};
    std::size_t second{};
  };

  static std::uint8_t Advance(CipherPosition& position);
  void ExtractPacket(std::vector<LambdaPacket>& packets);

  CipherPosition receive_;
  CipherPosition send_;
  std::vector<std::uint8_t> pending_;
  std::size_t expected_size_ = kHeaderSize;
  bool failed_ = false;
};
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_LAMBDA_PACKET_CODEC_H_
