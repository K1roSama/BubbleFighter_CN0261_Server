#ifndef BUBBLEFIGHTER_SERVER_GAME_CHAT_DIRECTORY_H_
#define BUBBLEFIGHTER_SERVER_GAME_CHAT_DIRECTORY_H_

#include <chrono>
#include <cstdint>
#include <deque>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace bubblefighter::game {
struct ChatIdentity {
  std::uint64_t session_id = 0;
  std::uint32_t account_id = 0;
  std::u16string nickname;
  std::uint32_t channel_type = 60;
  std::uint32_t channel_index = 0;
};

// Online identities and throttles are serialized on the application's Asio
// event loop.
class ChatDirectory {
 public:
  using Clock = std::chrono::steady_clock;

  void Join(ChatIdentity identity);
  void Leave(std::uint64_t session_id);
  std::optional<ChatIdentity> Find(std::uint64_t session_id) const;
  std::optional<ChatIdentity> FindNickname(std::u16string_view nickname) const;
  std::vector<ChatIdentity> Channel(std::uint64_t session_id) const;
  bool Admit(std::uint64_t session_id, std::u16string_view text,
             Clock::time_point now = Clock::now());

 private:
  struct Throttle {
    std::deque<Clock::time_point> sent;
    std::u16string previous;
    std::uint32_t repeated = 0;
    Clock::time_point blocked_until{};
    Clock::time_point last_activity{};
  };

  std::map<std::uint64_t, ChatIdentity> identities_;
  std::map<std::uint32_t, Throttle> throttles_;
};
}  // namespace bubblefighter::game

#endif  // BUBBLEFIGHTER_SERVER_GAME_CHAT_DIRECTORY_H_
