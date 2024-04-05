#include <QApplication>
#include <clocale>
#include <cstdlib>
#include <string>
#include <utility>

#include "controller/controller.h"
#include "model/model.h"
#include "view/monitoringview.h"

std::pair<size_t, std::string> TelegramInfo() {
  constexpr std::string_view kTeleTokenEnv = "TELE_TOKEN";
  constexpr std::string_view kTeleChatEnv = "TELE_CHAT";
  std::string tele_token = "";
  const char* kTeleToken = std::getenv(kTeleTokenEnv.data());
  if (kTeleToken) {
    tele_token = kTeleToken;
  }
  size_t tele_chat = 0;
  const char* kTeleChat = std::getenv(kTeleChatEnv.data());
  if (kTeleChat) {
    tele_chat = std::stoul(kTeleChat);
  }
  return {tele_chat, tele_token};
}

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  std::setlocale(LC_NUMERIC, "C");
  auto [tele_chat, tele_token] = TelegramInfo();
  monsys::TelegramBot t(tele_chat, tele_token);
  monsys::Model m;
  monsys::Controller c(&m);
  monsys::MonitoringView v(&c, &t);
  v.show();
  return a.exec();
}
