#include <spdlog/spdlog.h>

#include "game/game_session.h"
#include "protocol/chat_messages.h"

namespace bubblefighter::game {
void GameSession::RegisterChat() {
  chats_.Join({session_id_, account_->id, character_->nickname,
               channel_type_ == 0 ? 60 : channel_type_, channel_index_});
}

void GameSession::HandleSessionChat(const protocol::LambdaPacket& packet) {
  protocol::ModuleChat chat;
  const bool legacy = packet.message_id == 501;
  if (legacy) {
    const auto request = protocol::ReadSessionChat(packet);
    chat = {request.team_only ? 1u : 0u, request.text};
  } else if (packet.message_id == 304) {
    chat = {2, protocol::ReadPartyChat(packet)};
  } else {
    chat = protocol::ReadModuleChat(packet);
  }

  if (!chats_.Admit(session_id_, chat.text)) {
    const auto now = ChatDirectory::Clock::now();
    if (now >= next_chat_notice_) {
      connection_.Send(
          protocol::CreateChatNotice(u"发言过于频繁，请稍后再试。"));
      next_chat_notice_ = now + std::chrono::seconds(10);
    }
    return;
  }

  // Explicit server commands supplement the original messenger, which uses a
  // separate vendor transport. Only Nearby may invoke them; a team/party/couple
  // message must never be made public.
  if (chat.channel == 0 && HandleChatCommand(chat.text)) {
    return;
  }
  if (chat.channel == 2) {
    // S201 currently establishes a real one-member party. Room membership is
    // not party membership.
    connection_.Send(
        packet.message_id == 304
            ? protocol::CreatePartyChatReply(account_->id, chat.text)
            : protocol::CreateModuleChatReply(0xFFFD, character_->nickname, 2,
                                              chat.text));
    return;
  }
  if (chat.channel > 1) {
    connection_.Send(protocol::CreateChatNotice(
        u"该聊天频道尚未开放，请使用附近、队内或私聊。"));
    return;
  }

  const auto reply = [this, &chat, legacy](std::uint16_t slot) {
    return legacy ? protocol::CreateSessionChatReply(
                        slot, {chat.channel == 1, chat.text})
                  : protocol::CreateModuleChatReply(slot, character_->nickname,
                                                    chat.channel, chat.text);
  };
  if (const auto plaza = plazas_.Find(session_id_)) {
    if (chat.channel == 0 && plaza->plaza.members[plaza->slot].initialized) {
      BroadcastPlaza(plaza->plaza,
                     reply(static_cast<std::uint16_t>(plaza->slot)), 0, true);
    } else if (!legacy) {
      connection_.Send(
          protocol::CreateChatNotice(u"广场没有对战队伍，请使用附近聊天。"));
    }
    return;
  }
  if (const auto room = rooms_.FindBySession(session_id_)) {
    for (std::size_t slot = 0; slot < room->members.size(); ++slot) {
      const auto& sender = room->members[slot];
      if (sender.session_id != session_id_) {
        continue;
      }
      const auto notification = reply(static_cast<std::uint16_t>(slot));
      for (const auto& recipient : room->members) {
        if (recipient.session_id != 0 &&
            (chat.channel == 0 ||
             recipient.profile.team == sender.profile.team)) {
          notify_(recipient.session_id, notification);
        }
      }
      spdlog::debug("[Chat] session={} roomId={} channel={} units={}",
                    session_id_, room->setting.room_id, chat.channel,
                    chat.text.size());
      return;
    }
  }
  if (!legacy) {
    connection_.Send(protocol::CreateChatNotice(
        u"附近聊天需要先进入广场或房间。频道发言请用 #频道 内容。"));
  }
}

bool GameSession::HandleChatCommand(std::u16string_view text) {
  if (text.starts_with(u"#频道 ")) {
    const auto message = text.substr(4);
    if (message.empty() || message.size() > 220) {
      connection_.Send(protocol::CreateChatNotice(
          u"用法：#频道 内容，内容最多 220 个字符。"));
      return true;
    }
    const auto reply = protocol::CreateModuleChatReply(
        0xFFFD, character_->nickname, 0, u"[频道] " + std::u16string(message));
    for (const auto& recipient : chats_.Channel(session_id_)) {
      notify_(recipient.session_id, reply);
    }
    return true;
  }
  if (text.starts_with(u"@")) {
    const auto split = text.find(u' ');
    if (split == std::u16string_view::npos || split <= 1 || split > 17 ||
        split + 1 >= text.size() || text.size() - split - 1 > 220) {
      connection_.Send(protocol::CreateChatNotice(
          u"私聊用法：@玩家昵称 内容，内容最多 220 个字符。"));
      return true;
    }
    const auto target = chats_.FindNickname(text.substr(1, split - 1));
    if (!target) {
      connection_.Send(protocol::CreateChatNotice(
          u"该玩家不在线或存在重复连接，私聊未发送。"));
      return true;
    }
    const auto body = std::u16string(text.substr(split + 1));
    notify_(target->session_id,
            protocol::CreateModuleChatReply(0xFFFD, character_->nickname, 0,
                                            u"[私聊] " + body));
    if (target->session_id != session_id_) {
      connection_.Send(protocol::CreateModuleChatReply(
          0xFFFD, character_->nickname, 0,
          u"[私聊→" + target->nickname + u"] " + body));
    }
    return true;
  }
  return false;
}
}  // namespace bubblefighter::game
