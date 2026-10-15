#ifndef BUBBLEFIGHTER_SERVER_PROTOCOL_SHOP_MESSAGES_H_
#define BUBBLEFIGHTER_SERVER_PROTOCOL_SHOP_MESSAGES_H_

#include "features/shop/shop_repository.h"
#include "protocol/lambda_packet_codec.h"

namespace bubblefighter::protocol {
struct ShopPurchaseRequest {
  std::uint32_t stock_id = 0;
  bool alternate_purchase = false;
};

std::uint32_t ReadShopEntry(const LambdaPacket& packet);
ShopPurchaseRequest ReadShopPurchase(const LambdaPacket& packet);
LambdaPacket CreateShopReply(const shop::ShopCatalog& catalog,
                             std::uint32_t preferred_category);
LambdaPacket CreateShopPurchaseReply(std::uint32_t stock_id,
                                     shop::PurchaseResult result);
}  // namespace bubblefighter::protocol

#endif  // BUBBLEFIGHTER_SERVER_PROTOCOL_SHOP_MESSAGES_H_
