#include "view/monitoringview.h"
#include "view/ui_monitoringview.h"

#include <QDateTime>
#include <QMessageBox>
#include <thread>

namespace monsys {

namespace {

inline void SetupChartView(QChartView* view, Plot& plot) {
    view->setVisible(true);
    view->setRenderHint(QPainter::Antialiasing, true);
    view->setChart(plot.Chart());
}

} // namespace

MonitoringView::MonitoringView(Controller* controller)
    : controller_(controller), bot_(kTeleChatId, kTeleToken), ui_(new Ui::MonitoringView) {
    ui_->setupUi(this);
    Setup();
}

void MonitoringView::Setup() {
    SetupChartView(ui_->cpu_load_view, plots_[kCpuLoadPlot]);
    SetupChartView(ui_->cpu_process_view, plots_[kCpuProcessPlot]);

    SetupChartView(ui_->ram_total_view, plots_[kRamTotalPlot]);
    SetupChartView(ui_->ram_view, plots_[kRamPlot]);
    SetupChartView(ui_->hard_volume_view, plots_[kHardVolumePlot]);
    SetupChartView(ui_->hard_ops_view, plots_[kHardOpsPlot]);
    SetupChartView(ui_->hard_throughput_view, plots_[kHardThroughputPlot]);

    SetupChartView(ui_->url_available_view, plots_[kUrlAvailablePlot]);
    SetupChartView(ui_->inet_throughput_view, plots_[kInetThroughputPlot]);

    controller_->OnException([&](const std::string& error) {
        bot_.SendText(error);
        QMessageBox::critical(this, "Error", QString::fromStdString(error));
    });
}


//void MonitoringView::DrawPlot(double val, const Plot::Range& range) {
//  // plot_.AddValue(QDateTime::currentDateTime().toMSecsSinceEpoch(), val, range);
//}

MonitoringView::~MonitoringView() {
    delete ui_;
}

} // namespace monsys
