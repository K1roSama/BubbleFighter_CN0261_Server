#include "protocol/inventory_messages.h"

#include <algorithm>
#include <stdexcept>

#include "features/inventory/equipped_character.h"
#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
namespace {
// 0xA2ADC0 / 0xA221E0 render durations above 10000 days as permanent. This
// value is our wire convention.
constexpr double kPermanentDurationDays = 36500.0;
}  // namespace

std::uint8_t ReadWeaponPresetRequest(const LambdaPacket& packet) {
  if (packet.message_id != 830) {
    throw std::invalid_argument("Expected inventory weapon preset selection");
  }

  PacketReader reader(packet.payload);
  reader.ReadUnsigned();  // Client context (0xFFFFFFFF from the inventory);
                          // never an ownership identifier.
  const auto index = reader.ReadByte();
  reader.RequireEnd();
  return index;
}

LambdaPacket CreateWeaponPresetResult(std::uint8_t index, bool success) {
  // 0x8C4350 / 0x8BB110: selected byte, result u32; zero means success.
  PacketWriter writer;
  writer.WriteByte(index);
  writer.WriteUnsigned(success ? 0u : 1u);
  return {832, writer.GetBytes()};
}

LambdaPacket CreateWeaponPresetsReply(std::uint32_t identity,
                                      const inventory::WeaponPresets& presets) {
  if (presets.available_count < 2 || presets.available_count > 3 ||
      presets.selected >= presets.available_count) {
    throw std::invalid_argument(
        "Invalid weapon preset availability or selection");
  }

  PacketWriter writer;
  // 0x8C42F0 / 0x8976B0. 0xA32C60 indexes five entries regardless of vector
  // length.
  writer.WriteByte(presets.selected);
  writer.WriteUnsigned(identity);
  writer.WriteUnsigned(5);
  for (const auto item : presets.items) {
    writer.WriteUnsigned(item);
  }
  writer.WriteUnsigned(5);
  for (std::uint8_t index = 0; index < 5; ++index) {
    writer.WriteByte(index < presets.available_count ? 1 : 0);
  }
  writer.WriteUnsigned(5);
  writer.WriteBytes(
      std::array<std::uint8_t, 5>{});  // Reserved auxiliary flags; no
                                       // unsupported slots enabled.
  return {833, writer.GetBytes()};
}

std::uint32_t ReadEquipmentRequest(const LambdaPacket& packet) {
  if (packet.message_id != 803 && packet.message_id != 804) {
    throw std::invalid_argument(
        "Expected an inventory equip or unequip request");
  }

  PacketReader reader(packet.payload);
  const auto item_id = reader.ReadUnsigned();
  reader.RequireEnd();
  return item_id;
}

LambdaPacket CreateEquipmentResult(bool equipped, bool success) {
  // Original handlers 0x8ABEB0 / 0x8AC340: 0 succeeds, equip 3 is 'No item',
  // unequip 1 is failure.
  PacketWriter writer;
  writer.WriteUnsigned(success ? 0u : (equipped ? 3u : 1u));
  return {static_cast<std::uint16_t>(equipped ? 808 : 809), writer.GetBytes()};
}

LambdaPacket CreateInventoryReply(
    std::span<const inventory::InventoryItem> items) {
  if (items.size() > inventory::kMaximumInventoryItems) {
    throw std::invalid_argument("Inventory packet capacity exceeded");
  }

  PacketWriter writer;
  writer.WriteUnsigned(static_cast<std::uint32_t>(items.size()));
  for (const auto& item : items) {
    // InventorySlot::Serialize (0x8CD7F0); bools occupy four bytes on the wire.
    writer.WriteUnsigned(item.item_id);
    writer.WriteUnsigned(item.quantity);
    writer.WriteDouble(item.remaining_days.value_or(kPermanentDurationDays));
    writer.WriteUnsigned(item.equipped_slot != 0 ? 1 : 0);
    writer.WriteUnsigned(0);
    writer.WriteUnsigned(0);
    writer.WriteUnsigned(0);
    writer.WriteUnsigned(0);
    writer.WriteString(u"");
    writer.WriteUnsigned(0);  // No item attributes.
  }
  return {801, writer.GetBytes()};
}

LambdaPacket CreateEquipmentReply(
    std::uint32_t identity, std::uint32_t character_type,
    std::span<const inventory::InventoryItem> items) {
  const auto equipped_character =
      inventory::ResolveEquippedCharacter(character_type, items);

  PacketWriter writer;
  // EquippedItemList::Serialize (0x569150), wrapped by S805 at 0x890F20.
  writer.WriteUnsigned(identity);
  // 0xA31510 / 0x530510 resolve the equipped character item, not the C16
  // selection value.
  writer.WriteUnsigned(300000u + equipped_character);
  writer.WriteUnsigned(0);  // Additional appearance scalar; only the default is
                            // currently supported.
  writer.WriteUnsigned(static_cast<std::uint32_t>(std::count_if(
      items.begin(), items.end(), [](const inventory::InventoryItem& item) {
        return item.equipped_slot != 0 &&
               item.equipped_slot != inventory::kCharacterEquipmentSlot;
      })));
  for (const auto& item : items) {
    if (item.equipped_slot != 0 &&
        item.equipped_slot != inventory::kCharacterEquipmentSlot) {
      writer.WriteUnsigned(item.item_id);
    }
  }
  writer.WriteUnsigned(0);  // Empty byte vector.
  writer.WriteUnsigned(0);  // Empty item-attribute vector.
  writer.WriteUnsigned(0);  // Empty auxiliary vector.
  return {805, writer.GetBytes()};
}
}  // namespace bubblefighter::protocol
