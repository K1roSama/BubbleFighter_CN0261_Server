#include "protocol/shop_messages.h"

#include <algorithm>
#include <bit>
#include <chrono>
#include <cmath>
#include <stdexcept>

#include "protocol/packet_buffer.h"

namespace bubblefighter::protocol {
namespace {
constexpr std::size_t kMaximumStocksPerCard = 3;

void WriteStock(PacketWriter& writer, const shop::ShopStock& stock) {
  // Stock::Serialize 0x530550: three u32, one u16, then three u32 (26 bytes).
  writer.WriteUnsigned(stock.id);
  writer.WriteUnsigned(stock.item_id);
  writer.WriteUnsigned(1);
  writer.WriteShort(stock.type);
  writer.WriteUnsigned(stock.duration_days);
  writer.WriteUnsigned(stock.show_price);
  writer.WriteUnsigned(stock.sell_price);
}
}  // namespace

std::uint32_t ReadShopEntry(const LambdaPacket& packet) {
  if (packet.message_id != 801) {
    throw std::invalid_argument("Expected EnterShop");
  }

  PacketReader reader(packet.payload);
  const auto low = reader.ReadUnsigned();
  const auto high = reader.ReadUnsigned();
  const auto timestamp =
      std::bit_cast<double>((static_cast<std::uint64_t>(high) << 32) | low);
  const auto category = reader.ReadUnsigned();
  reader.RequireEnd();
  if (!std::isfinite(timestamp)) {
    throw std::invalid_argument("Shop catalog timestamp is not finite");
  }
  return category;
}

ShopPurchaseRequest ReadShopPurchase(const LambdaPacket& packet) {
  if (packet.message_id != 808) {
    throw std::invalid_argument("Expected ReqShopBuy");
  }

  PacketReader reader(packet.payload);
  const auto stock_id = reader.ReadUnsigned();
  const auto flag = reader.ReadUnsigned();
  reader.RequireEnd();
  if (flag > 1) {
    throw std::invalid_argument("Shop purchase flag is not a boolean");
  }
  return {stock_id, flag != 0};
}

LambdaPacket CreateShopReply(const shop::ShopCatalog& catalog,
                             std::uint32_t preferred_category) {
  if (catalog.items.size() > 512 || catalog.categories.size() > 16) {
    throw std::invalid_argument("Shop catalog exceeds packet capacity");
  }

  std::size_t card_count = 0;
  for (const auto& item : catalog.items) {
    if (item.stocks.empty() || item.stocks.size() > 32) {
      throw std::invalid_argument("Shop item has an invalid stock count");
    }

    card_count += (item.stocks.size() + kMaximumStocksPerCard - 1) /
                  kMaximumStocksPerCard;
  }
  if (card_count > 512) {
    throw std::invalid_argument("Shop catalog exceeds display card capacity");
  }
  if (std::none_of(catalog.categories.begin(), catalog.categories.end(),
                   [preferred_category](const auto& category) {
                     return category.id == preferred_category;
                   })) {
    preferred_category =
        catalog.categories.empty() ? 0 : catalog.categories.front().id;
  }

  const auto timestamp =
      25569.0 + std::chrono::duration<double>(
                    std::chrono::system_clock::now().time_since_epoch())
                        .count() /
                    86400.0;
  PacketWriter writer;
  writer.WriteUnsigned(0);  // Optional alternate shop mode disabled (0x8A7B50).
  writer.WriteUnsigned(1);  // Always send a fresh authoritative catalog.
  writer.WriteDouble(timestamp);
  writer.WriteUnsigned(static_cast<std::uint32_t>(card_count));
  for (const auto& item : catalog.items) {
    // DlgBuy (0x9DB440) displays three options; ordinary pages bind each card
    // to its own Stock vector.
    for (std::size_t offset = 0; offset < item.stocks.size();
         offset += kMaximumStocksPerCard) {
      const auto stocks =
          std::span(item.stocks)
              .subspan(offset, std::min(kMaximumStocksPerCard,
                                        item.stocks.size() - offset));
      writer.WriteUnsigned(item.id);
      writer.WriteUnsigned(
          0);  // Ordinary item; kind 1 selects StockSet packages.
      writer.WriteString(item.name);
      writer.WriteUnsigned(item.display_order);
      writer.WriteUnsigned(item.main_category_id);
      writer.WriteUnsigned(item.sub_category_id);
      writer.WriteUnsigned(1);  // 0xA1B3A0 filters +52 enabled, +44 main
                                // category and +48 subcategory.
      writer.WriteDouble(item.sale_starts_at);
      writer.WriteDouble(item.sale_ends_at);
      WriteStock(writer, stocks.front());
      writer.WriteUnsigned(static_cast<std::uint32_t>(stocks.size()));
      for (const auto& stock : stocks) {
        WriteStock(writer, stock);
      }
    }
  }

  // ShopCategoryAll and its two nested vectors (0x8CE060 / 0x73F230 /
  // 0x4C5E00).
  writer.WriteDouble(timestamp);
  writer.WriteUnsigned(static_cast<std::uint32_t>(catalog.categories.size()));
  for (const auto& category : catalog.categories) {
    writer.WriteUnsigned(category.id);
    writer.WriteString(category.name);
    writer.WriteUnsigned(category.display_order);
    writer.WriteUnsigned(static_cast<std::uint32_t>(category.children.size()));
    for (const auto& child : category.children) {
      writer.WriteUnsigned(child.id);
      writer.WriteString(child.name);
      writer.WriteUnsigned(child.display_order);
      writer.WriteUnsigned(child.page_type);
    }
  }
  writer.WriteUnsigned(0);  // Empty additional identifier list.
  writer.WriteUnsigned(preferred_category);
  if (writer.GetBytes().size() > 60000) {
    throw std::invalid_argument(
        "Shop catalog requires pagination before it can be sent");
  }
  return {800, writer.GetBytes()};
}

LambdaPacket CreateShopPurchaseReply(std::uint32_t stock_id,
                                     shop::PurchaseResult result) {
  PacketWriter writer;
  writer.WriteUnsigned(stock_id);  // DlgResponseBuy resolves this identifier
                                   // through the Stock table.
  writer.WriteUnsigned(static_cast<std::uint32_t>(result));
  return {813, writer.GetBytes()};
}
}  // namespace bubblefighter::protocol
