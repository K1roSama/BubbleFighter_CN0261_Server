#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_NX_PACKET_CODEC_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_NX_PACKET_CODEC_H_

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace bubblefighter::protocol {
struct NxPacket {
  std::uint16_t message_id{};
  std::vector<std::uint8_t> payload;
};

// Implements the confirmed NxPacket path only. The connection's transform mode
// must be supplied explicitly.
class NxPacketCodec {
 public:
  explicit NxPacketCodec(bool transform_payload);

  static constexpr std::size_t kHeaderSize = 4;
  static constexpr std::size_t kMaxPayloadSize = 65531;

  std::vector<NxPacket> Feed(std::span<const std::uint8_t> bytes);
  std::vector<std::uint8_t> Encode(const NxPacket& packet) const;
  void Reset();

 private:
  bool transform_payload_;
  std::vector<std::uint8_t> pending_;
};
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_NX_PACKET_CODEC_H_
