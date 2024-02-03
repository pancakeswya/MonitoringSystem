#ifndef MONITORINGVIEW_H
#define MONITORINGVIEW_H

#include "view/plot.h"
#include "controller/controller.h"
#include "tools/telegram_bot.h"

#include <QMainWindow>
#include <QDateTime>
#include <string>

namespace monsys {

QT_BEGIN_NAMESPACE
namespace Ui { class MonitoringView; }
QT_END_NAMESPACE

class MonitoringView : public QMainWindow {
    Q_OBJECT
public:
    MonitoringView(Controller* controller);
    ~MonitoringView();

private:
    static constexpr size_t kTeleChatId = 2;
    static inline const std::string kTeleToken = "";

    enum PlotType {
      kCpuLoadPlot = 0,
      kCpuProcessPlot,
      kRamTotalPlot,
      kRamPlot,
      kHardVolumePlot,
      kHardOpsPlot,
      kHardThroughputPlot,
      kUrlAvailablePlot,
      kInetThroughputPlot,
      kPlotsSize
    };

    void Setup();

    Controller* controller_{};
    TelegramBot bot_;

    Plot plots_[kPlotsSize];
    QDateTime timer_;

    Ui::MonitoringView *ui_;
};

} // namespace monsys

#endif // MONITORINGVIEW_H
