#include "model/model.h"
#include "view/monitoringview.h"
#include "controller/controller.h"

#include <clocale>
#include <QApplication>

int main(int argc, char *argv[]) {
  constexpr size_t kTeleChatId = 961646986;
  const std::string kTeleToken = "6639133805:AAF_DKy2Uq_Zes87Cevbg2f4YqBK11-pNDI";

  QApplication a(argc, argv);
  std::setlocale(LC_NUMERIC, "C");

  monsys::TelegramBot t(kTeleChatId, kTeleToken);
  monsys::Model m;
  monsys::Controller c(&m);
  monsys::MonitoringView v(&c, &t);
  v.show();
  return a.exec();
}



