#ifndef MONITORINGSYSTEM_SRC_VIEW_MONITORINGVIEW_H
#define MONITORINGSYSTEM_SRC_VIEW_MONITORINGVIEW_H

#include "view/plot.h"
#include "concurrency/worker.h"
#include "controller/controller.h"
#include "telegram/telegram_bot.h"

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
    explicit MonitoringView(Controller* controller, TelegramBot* bot);
    ~MonitoringView() override;

private:
    void resizeEvent(QResizeEvent*) override;

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
    void UpdatePlots(qint64 curr_time, const Metrics& metrics, const Config& config);
    void UpdateMetrics();
    void UpdateValues(const Metrics& metrics);
    void UpdateCharts();

    Controller* controller_{};
    TelegramBot* bot_;

    Plot plots_[kPlotsSize];
    QDateTime timer_;

    Worker metrics_worker_;
    Worker logger_worker_;
    Worker loader_worker_;

    Ui::MonitoringView *ui_;
};

} // namespace monsys

#endif // MONITORINGSYSTEM_SRC_VIEW_MONITORINGVIEW_H
