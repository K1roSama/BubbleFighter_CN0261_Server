#include "protocol/packet_buffer.h"

#include <bit>
#include <limits>
#include <stdexcept>

namespace bubblefighter::protocol {
PacketReader::PacketReader(std::span<const std::uint8_t> bytes)
    : bytes_(bytes) {}

std::uint8_t PacketReader::ReadByte() {
  if (Remaining() == 0) {
    throw std::invalid_argument(
        "Packet is truncated while reading an 8-bit field");
  }

  return bytes_[position_++];
}

std::uint16_t PacketReader::ReadShort() {
  if (Remaining() < 2) {
    throw std::invalid_argument(
        "Packet is truncated while reading a 16-bit field");
  }

  const auto value = static_cast<std::uint16_t>(bytes_[position_] |
                                                (bytes_[position_ + 1] << 8));
  position_ += 2;
  return value;
}

std::uint32_t PacketReader::ReadUnsigned() {
  if (Remaining() < 4) {
    throw std::invalid_argument(
        "Packet is truncated while reading a 32-bit field");
  }

  std::uint32_t value = 0;
  for (unsigned int shift = 0; shift < 32; shift += 8) {
    value |= static_cast<std::uint32_t>(bytes_[position_++]) << shift;
  }

  return value;
}

bool PacketReader::ReadBoolean() {
  const auto value = ReadUnsigned();
  if (value > 1) {
    throw std::invalid_argument(
        "Packet boolean must be encoded as zero or one in four bytes");
  }

  return value != 0;
}

std::u16string PacketReader::ReadString(std::size_t max_code_units) {
  const auto length = ReadUnsigned();
  if (length > max_code_units || length > Remaining() / 2) {
    throw std::invalid_argument("Packet UTF-16 string length is invalid");
  }

  std::u16string value;
  value.reserve(length);
  for (std::uint32_t index = 0; index < length; ++index) {
    const auto code_unit =
        static_cast<char16_t>(bytes_[position_] | (bytes_[position_ + 1] << 8));
    value.push_back(code_unit);
    position_ += 2;
  }

  return value;
}

std::size_t PacketReader::Remaining() const {
  return bytes_.size() - position_;
}

void PacketReader::RequireEnd() const {
  if (Remaining() != 0) {
    throw std::invalid_argument("Packet has unexpected trailing fields");
  }
}

void PacketWriter::WriteByte(std::uint8_t value) { bytes_.push_back(value); }

void PacketWriter::WriteShort(std::uint16_t value) {
  bytes_.push_back(static_cast<std::uint8_t>(value));
  bytes_.push_back(static_cast<std::uint8_t>(value >> 8));
}

void PacketWriter::WriteUnsigned(std::uint32_t value) {
  for (unsigned int shift = 0; shift < 32; shift += 8) {
    bytes_.push_back(static_cast<std::uint8_t>(value >> shift));
  }
}

void PacketWriter::WriteDouble(double value) {
  const auto bits = std::bit_cast<std::uint64_t>(value);
  WriteUnsigned(static_cast<std::uint32_t>(bits));
  WriteUnsigned(static_cast<std::uint32_t>(bits >> 32));
}

void PacketWriter::WriteString(std::u16string_view value) {
  if (value.size() > std::numeric_limits<std::uint32_t>::max()) {
    throw std::invalid_argument(
        "Packet UTF-16 string exceeds the wire length range");
  }

  WriteUnsigned(static_cast<std::uint32_t>(value.size()));
  for (const auto code_unit : value) {
    bytes_.push_back(static_cast<std::uint8_t>(code_unit));
    bytes_.push_back(static_cast<std::uint8_t>(code_unit >> 8));
  }
}

const std::vector<std::uint8_t>& PacketWriter::GetBytes() const {
  return bytes_;
}

void PacketWriter::WriteBytes(std::span<const std::uint8_t> bytes) {
  bytes_.insert(bytes_.end(), bytes.begin(), bytes.end());
}
}  // namespace bubblefighter::protocol
