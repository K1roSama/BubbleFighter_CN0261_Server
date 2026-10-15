#include "protocol/session_setting.h"

#include <stdexcept>

namespace bubblefighter::protocol {
namespace {
void WriteSettingString(PacketWriter& writer, std::u16string_view value) {
  if (value.size() > kSessionStringLimit) {
    throw std::invalid_argument(
        "Session setting string exceeds the supported length");
  }

  writer.WriteString(value);
}
}  // namespace

GameSetting ReadGameSetting(PacketReader& reader) {
  GameSetting setting;
  setting.mode = reader.ReadUnsigned();
  setting.sub_mode = reader.ReadUnsigned();
  for (auto& value : setting.strings16) {
    value = reader.ReadString(kSessionStringLimit);
  }
  for (auto& value : setting.values128) {
    value = reader.ReadUnsigned();
  }
  setting.flag4 = reader.ReadBoolean();
  setting.flag5 = reader.ReadBoolean();
  for (auto& value : setting.values148) {
    value = reader.ReadUnsigned();
  }
  for (auto& value : setting.strings180) {
    value = reader.ReadString(kSessionStringLimit);
  }
  setting.value348 = reader.ReadUnsigned();
  const auto count = reader.ReadUnsigned();
  if (count > kGameStringCountLimit || count > reader.Remaining() / 4) {
    throw std::invalid_argument(
        "Game setting string count exceeds the supported capacity");
  }
  setting.strings352.reserve(count);
  for (std::uint32_t index = 0; index < count; ++index) {
    setting.strings352.push_back(reader.ReadString(kSessionStringLimit));
  }
  setting.flag368 = reader.ReadBoolean();
  setting.value372 = reader.ReadUnsigned();
  setting.flag376 = reader.ReadBoolean();
  setting.flag377 = reader.ReadBoolean();
  setting.value380 = reader.ReadUnsigned();
  setting.value384 = reader.ReadUnsigned();
  setting.string388 = reader.ReadString(kSessionStringLimit);
  setting.string416 = reader.ReadString(kSessionStringLimit);
  setting.flag444 = reader.ReadBoolean();
  setting.value448 = reader.ReadUnsigned();
  return setting;
}

SessionSetting ReadSessionSetting(PacketReader& reader) {
  SessionSetting setting;
  setting.state = reader.ReadUnsigned();
  setting.room_id = reader.ReadShort();
  setting.title = reader.ReadString(kSessionStringLimit);
  setting.member_count = reader.ReadShort();
  setting.value42 = reader.ReadShort();
  setting.capacity = reader.ReadShort();
  setting.password_required = reader.ReadBoolean();
  setting.password = reader.ReadString(kSessionStringLimit);
  setting.kind = reader.ReadUnsigned();
  setting.game = ReadGameSetting(reader);
  setting.value532 = reader.ReadUnsigned();
  setting.value536 = reader.ReadUnsigned();
  setting.value536 |= static_cast<std::uint64_t>(reader.ReadUnsigned()) << 32;
  setting.string544 = reader.ReadString(kSessionStringLimit);
  for (auto& value : setting.values572) {
    value = reader.ReadUnsigned();
  }
  setting.flag588 = reader.ReadBoolean();
  return setting;
}

void WriteGameSetting(PacketWriter& writer, const GameSetting& setting) {
  // GameSetting::Serialize 0x41A2B0, with defaults from 0x41A130.
  if (setting.strings352.size() > kGameStringCountLimit) {
    throw std::invalid_argument(
        "Game setting string count exceeds the supported capacity");
  }

  writer.WriteUnsigned(setting.mode);
  writer.WriteUnsigned(setting.sub_mode);
  for (const auto& value : setting.strings16) {
    WriteSettingString(writer, value);
  }
  for (const auto value : setting.values128) {
    writer.WriteUnsigned(value);
  }
  writer.WriteUnsigned(setting.flag4 ? 1 : 0);
  writer.WriteUnsigned(setting.flag5 ? 1 : 0);
  for (const auto value : setting.values148) {
    writer.WriteUnsigned(value);
  }
  for (const auto& value : setting.strings180) {
    WriteSettingString(writer, value);
  }
  writer.WriteUnsigned(setting.value348);
  writer.WriteUnsigned(static_cast<std::uint32_t>(setting.strings352.size()));
  for (const auto& value : setting.strings352) {
    WriteSettingString(writer, value);
  }
  writer.WriteUnsigned(setting.flag368 ? 1 : 0);
  writer.WriteUnsigned(setting.value372);
  writer.WriteUnsigned(setting.flag376 ? 1 : 0);
  writer.WriteUnsigned(setting.flag377 ? 1 : 0);
  writer.WriteUnsigned(setting.value380);
  writer.WriteUnsigned(setting.value384);
  WriteSettingString(writer, setting.string388);
  WriteSettingString(writer, setting.string416);
  writer.WriteUnsigned(setting.flag444 ? 1 : 0);
  writer.WriteUnsigned(setting.value448);
}

void WriteSessionSetting(PacketWriter& writer, const SessionSetting& setting) {
  // SessionSetting::Serialize 0x41AC80: u16 fields and a little-endian u64 are
  // unaligned on the wire.
  writer.WriteUnsigned(setting.state);
  writer.WriteShort(setting.room_id);
  WriteSettingString(writer, setting.title);
  writer.WriteShort(setting.member_count);
  writer.WriteShort(setting.value42);
  writer.WriteShort(setting.capacity);
  writer.WriteUnsigned(setting.password_required ? 1 : 0);
  WriteSettingString(writer, setting.password);
  writer.WriteUnsigned(setting.kind);
  WriteGameSetting(writer, setting.game);
  writer.WriteUnsigned(setting.value532);
  writer.WriteUnsigned(static_cast<std::uint32_t>(setting.value536));
  writer.WriteUnsigned(static_cast<std::uint32_t>(setting.value536 >> 32));
  WriteSettingString(writer, setting.string544);
  for (const auto value : setting.values572) {
    writer.WriteUnsigned(value);
  }
  writer.WriteUnsigned(setting.flag588 ? 1 : 0);
}
}  // namespace bubblefighter::protocol
