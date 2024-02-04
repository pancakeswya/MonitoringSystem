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


inline Plot::Range PairToRange(const std::pair<double, double>& pair) noexcept {
    return {
        .min = pair.first,
        .max = pair.second
    };
}

} // namespace

MonitoringView::MonitoringView(Controller* controller)
    : controller_(controller), bot_(kTeleChatId, kTeleToken), ui_(new Ui::MonitoringView) {
    ui_->setupUi(this);
    Setup();
    StartMonitoring();
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

void MonitoringView::StartMonitoring() {
    controller_->LoadAgents();
    metrics_worker_.SetWork([&] { UpdateMetrics(); });
    metrics_worker_.Start();
}

void MonitoringView::UpdateCharts() {
    ui_->cpu_load_view->update();
    ui_->cpu_process_view->update();
    ui_->ram_total_view->update();
    ui_->ram_view->update();
    ui_->hard_volume_view->update();
    ui_->hard_ops_view->update();
    ui_->hard_throughput_view->update();
    ui_->url_available_view->update();
    ui_->inet_throughput_view->update();
}

void MonitoringView::UpdateMetrics() {
    SystemConfig config = controller_->GetConfig();

    Metrics metrics = controller_->GetMetrics();

    qint64 curr_time = QDateTime::currentDateTime().toMSecsSinceEpoch();

    plots_[kCpuLoadPlot].AddValue(curr_time, metrics.cpu_load, PairToRange(config.at("cpu").range));
    plots_[kCpuProcessPlot].AddValue(curr_time, metrics.cpu_processes, PairToRange(config.at("processes").range));
    plots_[kRamTotalPlot].AddValue(curr_time, metrics.ram_total, PairToRange(config.at("ram_total").range));
    plots_[kRamPlot].AddValue(curr_time, metrics.ram, PairToRange(config.at("ram").range));
    plots_[kHardVolumePlot].AddValue(curr_time, metrics.hard_volume, PairToRange(config.at("hard_volume").range));
    plots_[kHardOpsPlot].AddValue(curr_time, metrics.hard_ops, PairToRange(config.at("hard_ops").range));
    plots_[kHardThroughputPlot].AddValue(curr_time, metrics.hard_throughput, PairToRange(config.at("hard_throughput").range));
    plots_[kUrlAvailablePlot].AddValue(curr_time, metrics.url_available, PairToRange(config.at("url").range));
    plots_[kInetThroughputPlot].AddValue(curr_time, metrics.inet_throughput, PairToRange(config.at("inet_throughput").range));

    UpdateCharts();
}

MonitoringView::~MonitoringView() {
    delete ui_;
}

} // namespace monsys
