#ifndef MONITORINGSYSTEM_SRC_VIEW_MONITORINGVIEW_H
#define MONITORINGSYSTEM_SRC_VIEW_MONITORINGVIEW_H

#include "view/plot.h"
#include "concurrency/worker.h"
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
    void resizeEvent(QResizeEvent*) override;

    static constexpr size_t kTeleChatId = 961646986;
    static inline const std::string kTeleToken = "6639133805:AAF_DKy2Uq_Zes87Cevbg2f4YqBK11-pNDI";

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
    void StartMonitoring();
    void UpdateSize();
    void UpdateMetrics();
    void UpdateCharts();


    Controller* controller_{};
    TelegramBot bot_;

    Plot plots_[kPlotsSize];
    QDateTime timer_;
    Worker metrics_worker_;

    Ui::MonitoringView *ui_;
};

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_VIEW_MONITORINGVIEW_H
