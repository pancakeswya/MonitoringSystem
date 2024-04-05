#include "telegram/telegram_bot.h"

#include <curl/curl.h>

namespace monsys {

namespace {

inline std::string GetTelegramApiUrl(const std::string& token) {
  return "https://api.telegram.org/bot" + token + "/sendMessage";
}

}  // namespace

TelegramBot::TelegramBot(size_t chat_id, const std::string& token) noexcept
    : chat_id_(std::to_string(chat_id)), url_(GetTelegramApiUrl(token)) {}

bool TelegramBot::SendText(const std::string& text) noexcept {
  CURLcode response = CURLE_FAILED_INIT;
  if (CURL* curl = curl_easy_init(); curl) {
    std::string data =
        "chat_id=" + chat_id_ + "&disable_web_page_preview=1&text=" + text;

    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(curl, CURLOPT_URL, url_.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE,
                     static_cast<curl_off_t>(data.size()));
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION,
                     static_cast<long int>(CURL_HTTP_VERSION_2TLS));
    curl_easy_setopt(curl, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

    response = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }
  return response == CURLE_OK;
}

}  // namespace monsys