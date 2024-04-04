#include "view/monitoringview.h"
#include "view/ui_monitoringview.h"

#include <cstdlib>
#include <string>
#include <thread>

#include <QDateTime>
#include <QMessageBox>

namespace monsys {

namespace {

constexpr std::string_view kLoggingDelayEnv = "LOGGING_DELAY";
constexpr std::string_view kAgentsDelayEnv = "AGENTS_DELAY";

inline void SetupChartView(const QString& title, QChartView* view, Plot& plot) {
    plot.SetTitle(title);
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

MonitoringView::MonitoringView(Controller* controller, TelegramBot* bot)
    : controller_(controller), bot_(bot), ui_(new Ui::MonitoringView) {
    ui_->setupUi(this);
    Setup();
    StartMonitoring();
}

void MonitoringView::resizeEvent(QResizeEvent*) {
    UpdateSize();
}

void MonitoringView::UpdateSize() {
    const QSize size = ui_->cpu_load_view->size();

    ui_->ram_total_view->setMinimumSize(size);
    ui_->ram_view->setMinimumSize(size);
    ui_->hard_volume_view->setMinimumSize(size);
    ui_->hard_ops_view->setMinimumSize(size);
    ui_->hard_throughput_view->setMinimumSize(size);
    ui_->url_available_view->setMinimumSize(size);
    ui_->inet_throughput_view->setMinimumSize(size);
}

void MonitoringView::Setup() {
    SetupChartView("Load", ui_->cpu_load_view, plots_[kCpuLoadPlot]);
    SetupChartView("Process", ui_->cpu_process_view, plots_[kCpuProcessPlot]);

    SetupChartView("Ram total", ui_->ram_total_view, plots_[kRamTotalPlot]);
    SetupChartView("Ram", ui_->ram_view, plots_[kRamPlot]);
    SetupChartView("Hard volume", ui_->hard_volume_view, plots_[kHardVolumePlot]);
    SetupChartView("Hard ops", ui_->hard_ops_view, plots_[kHardOpsPlot]);
    SetupChartView("Hard throughput", ui_->hard_throughput_view, plots_[kHardThroughputPlot]);

    SetupChartView("Url available", ui_->url_available_view, plots_[kUrlAvailablePlot]);
    SetupChartView("Inet throughput", ui_->inet_throughput_view, plots_[kInetThroughputPlot]);

    controller_->OnError([&bot = bot_](const std::string& error) {
      bot->SendText(error);
    });
    controller_->LoadAgents();
}

void MonitoringView::StartMonitoring() {
    if (!controller_->LoadConfig()) {
      QMessageBox::critical(this, "Error", "Config not loaded");
      return;
    }
    const std::string_view kLoggingDelay = std::getenv(kLoggingDelayEnv.data());
    if (kLoggingDelay.empty()) {
      QMessageBox::critical(this, "Error", "No logging delay");
      return;
    }
    const std::string_view kAgentsDelay = std::getenv(kAgentsDelayEnv.data());
    if (kAgentsDelay.empty()) {
      QMessageBox::critical(this, "Error", "No agents loading delay");
      return;
    }
    loader_worker_.SetTimeout(std::stoul(kLoggingDelay.data()));
    logger_worker_.SetTimeout(std::stoul(kAgentsDelay.data()));

    loader_worker_.SetWork(&Controller::LoadAgents, controller_);
    metrics_worker_.SetWork(&MonitoringView::UpdateMetrics, this);
    logger_worker_.SetWork(&Controller::LogMetrics, controller_);

    loader_worker_.Start();
    metrics_worker_.Start();
    logger_worker_.Start();
}

inline void MonitoringView::UpdateCharts() {
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

inline void MonitoringView::UpdateValues(const Metrics& metrics) {
    ui_->current_value_cpu_load_lable->setText(QString::number(metrics.cpu_load));
    ui_->current_value_cpu_process_lable->setText(QString::number(metrics.cpu_processes));

    ui_->current_value_ram_total_lable->setText(QString::number(metrics.ram_total));
    ui_->current_value_ram_lable->setText(QString::number(metrics.ram));
    ui_->current_value_hard_volume_lable->setText(QString::number(metrics.hard_volume));
    ui_->current_value_hard_ops_lable->setText(QString::number(metrics.hard_ops));
    ui_->current_value_hard_throughput_lable->setText(QString::number(metrics.hard_throughput));

    ui_->current_value_url_available_lable->setText(QString::number(metrics.url_available));
    ui_->current_value_inet_throughput_lable->setText(QString::number(metrics.inet_throughput));
}

inline void MonitoringView::UpdatePlots(qint64 curr_time, const Metrics& metrics, const Config& config) {
    plots_[kCpuLoadPlot].AddValue(curr_time, metrics.cpu_load, PairToRange(config.cpu.load.range));
    plots_[kCpuProcessPlot].AddValue(curr_time, metrics.cpu_processes, PairToRange(config.cpu.processes.range));
    plots_[kRamTotalPlot].AddValue(curr_time, metrics.ram_total, PairToRange(config.memory.ram_total.range));
    plots_[kRamPlot].AddValue(curr_time, metrics.ram, PairToRange(config.memory.ram.range));
    plots_[kHardVolumePlot].AddValue(curr_time, metrics.hard_volume, PairToRange(config.memory.hard_volume.range));
    plots_[kHardOpsPlot].AddValue(curr_time, metrics.hard_ops, PairToRange(config.memory.hard_ops.range));
    plots_[kHardThroughputPlot].AddValue(curr_time, metrics.hard_throughput, PairToRange(config.memory.hard_throughput.range));
    plots_[kUrlAvailablePlot].AddValue(curr_time, metrics.url_available, PairToRange(config.network.url_available.range));
    plots_[kInetThroughputPlot].AddValue(curr_time, metrics.inet_throughput, PairToRange(config.network.inet_throughout.range));
}

void MonitoringView::UpdateMetrics() {
    const Config config = controller_->GetConfig();
    const Metrics metrics = controller_->CollectMetrics();

    qint64 curr_time = QDateTime::currentDateTime().toMSecsSinceEpoch();

    UpdatePlots(curr_time, metrics, config);
    UpdateValues(metrics);
    UpdateCharts();
}

MonitoringView::~MonitoringView() {
    delete ui_;
}

} // namespace monsys
