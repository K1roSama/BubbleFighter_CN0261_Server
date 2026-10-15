#ifndef BUBBLEFIGHTER_SERVER_GAME_CHANNEL_DIRECTORY_H_
#define BUBBLEFIGHTER_SERVER_GAME_CHANNEL_DIRECTORY_H_

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

#include "features/authentication/account_repository.h"
#include "features/characters/character_repository.h"
#include "features/inventory/inventory_expiry.h"

namespace bubblefighter::game {
struct ChannelTransfer {
  authentication::AccountIdentity account;
  characters::CharacterProfile character;
  std::uint64_t source_session_id = 0;
  std::uint32_t channel_type = 60;
  std::uint32_t channel_index = 0;
  inventory::InventoryExpiry inventory_expiry;
};

// All operations run on the application's single Asio event loop.
class ChannelDirectory {
 public:
  using Clock = std::chrono::steady_clock;

  static bool Contains(std::uint32_t channel_type, std::uint32_t channel_index);
  bool Prepare(ChannelTransfer transfer, std::string peer_address,
               Clock::time_point now = Clock::now());
  std::optional<ChannelTransfer> Consume(std::uint32_t account_id,
                                         const std::string& peer_address,
                                         std::uint64_t destination_session_id,
                                         Clock::time_point now = Clock::now());
  bool HasPending(std::uint64_t session_id,
                  Clock::time_point now = Clock::now());
  void Cancel(std::uint64_t session_id);

 private:
  struct PendingTransfer {
    ChannelTransfer identity;
    std::string peer_address;
    Clock::time_point expires_at;
  };

  void Expire(Clock::time_point now);
  std::unordered_map<std::uint32_t, PendingTransfer> pending_;
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_CHANNEL_DIRECTORY_H_
