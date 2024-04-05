#ifndef MONITORINGSYSTEM_SRC_TOOLS_TELEGRAM_BOT_H_
#define MONITORINGSYSTEM_SRC_TOOLS_TELEGRAM_BOT_H_

#include <string>

namespace monsys {

class TelegramBot {
 public:
  TelegramBot(size_t chat_id, const std::string& token) noexcept;
  bool SendText(const std::string& text) noexcept;

 private:
  std::string chat_id_;
  std::string url_;
};

}  // namespace monsys

#endif  // MONITORINGSYSTEM_SRC_TOOLS_TELEGRAM_BOT_H_