#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_PACKET_BUFFER_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_PACKET_BUFFER_H_

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace bubblefighter::protocol {
class PacketReader {
 public:
  explicit PacketReader(std::span<const std::uint8_t> bytes);

  std::uint8_t ReadByte();
  std::uint16_t ReadShort();
  std::uint32_t ReadUnsigned();
  bool ReadBoolean();
  std::u16string ReadString(std::size_t max_code_units = 32767);
  std::size_t Remaining() const;
  void RequireEnd() const;

 private:
  std::span<const std::uint8_t> bytes_;
  std::size_t position_ = 0;
};

class PacketWriter {
 public:
  void WriteByte(std::uint8_t value);
  void WriteShort(std::uint16_t value);
  void WriteUnsigned(std::uint32_t value);
  void WriteDouble(double value);
  void WriteString(std::u16string_view value);
  void WriteBytes(std::span<const std::uint8_t> bytes);
  const std::vector<std::uint8_t>& GetBytes() const;

 private:
  std::vector<std::uint8_t> bytes_;
};
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_PACKET_BUFFER_H_
