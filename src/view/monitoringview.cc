#include "view/monitoringview.h"

#include <QDateTime>
#include <QMessageBox>
#include <QValidator>
#include <cstdlib>
#include <string>
#include <thread>

#include "view/ui_monitoringview.h"

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
  return {.min = pair.first, .max = pair.second};
}

}  // namespace

MonitoringView::MonitoringView(Controller* controller, TelegramBot* bot)
    : controller_(controller), bot_(bot), ui_(new Ui::MonitoringView) {
  ui_->setupUi(this);
  Setup();
  StartMonitoring();
}

void MonitoringView::resizeEvent(QResizeEvent*) { UpdateSize(); }

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

void MonitoringView::SaveConfig() {
  Config config;
  config.cpu.load.range = {
      ui_->line_edit_range_min_cpu_load->text().toDouble(),
      ui_->line_edit_range_max_cpu_load->text().toDouble()};
  config.cpu.load.timeout =
      ui_->line_edit_timeout_cpu_load_value->text().toUInt();

  config.cpu.processes.range = {
      ui_->line_edit_range_min_cpu_processes->text().toDouble(),
      ui_->line_edit_range_max_cpu_processes->text().toDouble()};
  config.cpu.processes.timeout =
      ui_->line_edit_timeout_cpu_processes_value->text().toUInt();

  config.memory.hard_ops.range = {
      ui_->line_edit_range_min_hard_ops->text().toDouble(),
      ui_->line_edit_range_max_hard_ops->text().toDouble()};
  config.memory.hard_ops.timeout =
      ui_->line_edit_timeout_hard_ops_value->text().toUInt();

  config.memory.hard_throughput.range = {
      ui_->line_edit_range_min_hard_throughput->text().toDouble(),
      ui_->line_edit_range_max_hard_throughput->text().toDouble()};
  config.memory.hard_throughput.timeout =
      ui_->line_edit_timeout_hard_throughput_value->text().toUInt();

  config.memory.hard_volume.range = {
      ui_->line_edit_range_min_hard_volume->text().toDouble(),
      ui_->line_edit_range_max_hard_volume->text().toDouble()};
  config.memory.hard_volume.timeout =
      ui_->line_edit_timeout_hard_volume_value->text().toUInt();

  config.memory.ram.range = {ui_->line_edit_range_min_ram->text().toDouble(),
                             ui_->line_edit_range_max_ram->text().toDouble()};
  config.memory.ram.timeout = ui_->line_edit_timeout_ram_value->text().toUInt();

  config.memory.ram_total.range = {
      ui_->line_edit_range_min_ram_total->text().toDouble(),
      ui_->line_edit_range_max_ram_total->text().toDouble()};
  config.memory.ram_total.timeout =
      ui_->line_edit_timeout_ram_value->text().toUInt();

  config.network.url_available.range = {
      ui_->line_edit_range_min_url_available->text().toDouble(),
      ui_->line_edit_range_max_url_available->text().toDouble()};
  config.network.url_available.timeout =
      ui_->line_edit_timeout_url_available_value->text().toUInt();
  config.network.url_available.url =
      ui_->line_edit_url_value->text().toStdString();

  config.network.inet_throughout.range = {
      ui_->line_edit_range_min_inet_throughput->text().toDouble(),
      ui_->line_edit_range_max_inet_throughput->text().toDouble()};
  config.network.inet_throughout.timeout =
      ui_->line_edit_timeout_inet_throughput_value->text().toUInt();

  controller_->SetConfig(config);
  controller_->SaveConfig();
}

void MonitoringView::ErrorSlot(const QString& err) {
  QMessageBox::critical(this, "Error", err);
  bot_->SendText(err.toStdString());
}

void MonitoringView::SetupConfig() {
  Config config = controller_->GetConfig();
  ui_->line_edit_range_max_cpu_load->setText(
      QString::number(config.cpu.load.range.second));
  ui_->line_edit_range_max_cpu_load->setValidator(new QDoubleValidator(this));
  ui_->line_edit_range_min_cpu_load->setText(
      QString::number(config.cpu.load.range.first));
  ui_->line_edit_range_min_cpu_load->setValidator(new QDoubleValidator(this));
  ui_->line_edit_timeout_cpu_load_value->setText(
      QString::number(config.cpu.load.timeout));
  ui_->line_edit_timeout_cpu_load_value->setValidator(new QIntValidator(this));

  ui_->line_edit_range_max_cpu_processes->setText(
      QString::number(config.cpu.processes.range.second));
  ui_->line_edit_range_max_cpu_processes->setValidator(
      new QDoubleValidator(this));
  ui_->line_edit_range_min_cpu_processes->setText(
      QString::number(config.cpu.processes.range.first));
  ui_->line_edit_range_min_cpu_processes->setValidator(
      new QDoubleValidator(this));
  ui_->line_edit_timeout_cpu_processes_value->setText(
      QString::number(config.cpu.processes.timeout));
  ui_->line_edit_timeout_cpu_processes_value->setValidator(
      new QIntValidator(this));

  ui_->line_edit_range_max_hard_ops->setText(
      QString::number(config.memory.hard_ops.range.second));
  ui_->line_edit_range_max_hard_ops->setValidator(new QDoubleValidator(this));
  ui_->line_edit_range_min_hard_ops->setText(
      QString::number(config.memory.hard_ops.range.first));
  ui_->line_edit_range_min_hard_ops->setValidator(new QDoubleValidator(this));
  ui_->line_edit_timeout_hard_ops_value->setText(
      QString::number(config.memory.hard_ops.timeout));
  ui_->line_edit_timeout_hard_ops_value->setValidator(new QIntValidator(this));

  ui_->line_edit_range_max_hard_throughput->setText(
      QString::number(config.memory.hard_throughput.range.second));
  ui_->line_edit_range_max_hard_throughput->setValidator(
      new QDoubleValidator(this));
  ui_->line_edit_range_min_hard_throughput->setText(
      QString::number(config.memory.hard_throughput.range.first));
  ui_->line_edit_range_min_hard_throughput->setValidator(
      new QDoubleValidator(this));
  ui_->line_edit_timeout_hard_throughput_value->setText(
      QString::number(config.memory.hard_throughput.timeout));
  ui_->line_edit_timeout_hard_throughput_value->setValidator(
      new QIntValidator(this));

  ui_->line_edit_range_max_hard_volume->setText(
      QString::number(config.memory.hard_volume.range.second));
  ui_->line_edit_range_max_hard_volume->setValidator(
      new QDoubleValidator(this));
  ui_->line_edit_range_min_hard_volume->setText(
      QString::number(config.memory.hard_volume.range.first));
  ui_->line_edit_range_min_hard_volume->setValidator(
      new QDoubleValidator(this));
  ui_->line_edit_timeout_hard_volume_value->setText(
      QString::number(config.memory.hard_volume.timeout));
  ui_->line_edit_timeout_hard_volume_value->setValidator(
      new QIntValidator(this));

  ui_->line_edit_range_max_ram->setText(
      QString::number(config.memory.ram.range.second));
  ui_->line_edit_range_max_ram->setValidator(new QDoubleValidator(this));
  ui_->line_edit_range_min_ram->setText(
      QString::number(config.memory.ram.range.first));
  ui_->line_edit_range_min_ram->setValidator(new QDoubleValidator(this));
  ui_->line_edit_timeout_ram_value->setText(
      QString::number(config.memory.ram.timeout));
  ui_->line_edit_timeout_ram_value->setValidator(new QIntValidator(this));

  ui_->line_edit_range_max_ram_total->setText(
      QString::number(config.memory.ram_total.range.second));
  ui_->line_edit_range_max_ram_total->setValidator(new QDoubleValidator(this));
  ui_->line_edit_range_min_ram_total->setText(
      QString::number(config.memory.ram_total.range.first));
  ui_->line_edit_range_min_ram_total->setValidator(new QDoubleValidator(this));
  ui_->line_edit_timeout_ram_value->setText(
      QString::number(config.memory.ram_total.timeout));
  ui_->line_edit_timeout_ram_value->setValidator(new QIntValidator(this));

  ui_->line_edit_range_max_url_available->setText(
      QString::number(config.network.url_available.range.second));
  ui_->line_edit_range_max_url_available->setValidator(
      new QDoubleValidator(this));
  ui_->line_edit_range_min_url_available->setText(
      QString::number(config.network.url_available.range.first));
  ui_->line_edit_range_min_url_available->setValidator(
      new QDoubleValidator(this));
  ui_->line_edit_timeout_url_available_value->setText(
      QString::number(config.network.url_available.timeout));
  ui_->line_edit_timeout_url_available_value->setValidator(
      new QIntValidator(this));
  ui_->line_edit_url_value->setText(
      QString::fromStdString(config.network.url_available.url));

  ui_->line_edit_range_max_inet_throughput->setText(
      QString::number(config.network.inet_throughout.range.second));
  ui_->line_edit_range_max_inet_throughput->setValidator(
      new QDoubleValidator(this));
  ui_->line_edit_range_min_inet_throughput->setText(
      QString::number(config.network.inet_throughout.range.first));
  ui_->line_edit_range_min_inet_throughput->setValidator(
      new QDoubleValidator(this));
  ui_->line_edit_timeout_inet_throughput_value->setText(
      QString::number(config.network.inet_throughout.timeout));
  ui_->line_edit_timeout_inet_throughput_value->setValidator(
      new QIntValidator(this));
}

void MonitoringView::Setup() {
  connect(this, &MonitoringView::OnErrorSignal, this,
          &MonitoringView::ErrorSlot);
  controller_->OnError([&](const std::string& error) {
    emit(OnErrorSignal(QString::fromStdString(error)));
  });
  controller_->LoadAgents();
  controller_->LoadConfig();

  connect(ui_->pushbutton_save_config, &QPushButton::clicked, this,
          &MonitoringView::SaveConfig);

  SetupConfig();

  SetupChartView("Load", ui_->cpu_load_view, plots_[kCpuLoadPlot]);
  SetupChartView("Process", ui_->cpu_process_view, plots_[kCpuProcessPlot]);

  SetupChartView("Ram total", ui_->ram_total_view, plots_[kRamTotalPlot]);
  SetupChartView("Ram", ui_->ram_view, plots_[kRamPlot]);
  SetupChartView("Hard volume", ui_->hard_volume_view, plots_[kHardVolumePlot]);
  SetupChartView("Hard ops", ui_->hard_ops_view, plots_[kHardOpsPlot]);
  SetupChartView("Hard throughput", ui_->hard_throughput_view,
                 plots_[kHardThroughputPlot]);

  SetupChartView("Url available", ui_->url_available_view,
                 plots_[kUrlAvailablePlot]);
  SetupChartView("Inet throughput", ui_->inet_throughput_view,
                 plots_[kInetThroughputPlot]);
}

void MonitoringView::StartMonitoring() {
  const char* kLoggingDelay = std::getenv(kLoggingDelayEnv.data());
  if (!kLoggingDelay) {
    QMessageBox::critical(this, "Error", "No logging delay");
    return;
  }
  const char* kAgentsDelay = std::getenv(kAgentsDelayEnv.data());
  if (!kAgentsDelay) {
    QMessageBox::critical(this, "Error", "No agents loading delay");
    return;
  }
  loader_worker_.SetTimeout(std::stoul(kLoggingDelay));
  logger_worker_.SetTimeout(std::stoul(kAgentsDelay));

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
  ui_->current_value_cpu_process_lable->setText(
      QString::number(metrics.cpu_processes));

  ui_->current_value_ram_total_lable->setText(
      QString::number(metrics.ram_total));
  ui_->current_value_ram_lable->setText(QString::number(metrics.ram));
  ui_->current_value_hard_volume_lable->setText(
      QString::number(metrics.hard_volume));
  ui_->current_value_hard_ops_lable->setText(QString::number(metrics.hard_ops));
  ui_->current_value_hard_throughput_lable->setText(
      QString::number(metrics.hard_throughput));

  ui_->current_value_url_available_lable->setText(
      QString::number(metrics.url_available));
  ui_->current_value_inet_throughput_lable->setText(
      QString::number(metrics.inet_throughput));
}

inline void MonitoringView::UpdatePlots(qint64 curr_time,
                                        const Metrics& metrics,
                                        const Config& config) {
  plots_[kCpuLoadPlot].AddValue(curr_time, metrics.cpu_load,
                                PairToRange(config.cpu.load.range));
  plots_[kCpuProcessPlot].AddValue(curr_time, metrics.cpu_processes,
                                   PairToRange(config.cpu.processes.range));
  plots_[kRamTotalPlot].AddValue(curr_time, metrics.ram_total,
                                 PairToRange(config.memory.ram_total.range));
  plots_[kRamPlot].AddValue(curr_time, metrics.ram,
                            PairToRange(config.memory.ram.range));
  plots_[kHardVolumePlot].AddValue(
      curr_time, metrics.hard_volume,
      PairToRange(config.memory.hard_volume.range));
  plots_[kHardOpsPlot].AddValue(curr_time, metrics.hard_ops,
                                PairToRange(config.memory.hard_ops.range));
  plots_[kHardThroughputPlot].AddValue(
      curr_time, metrics.hard_throughput,
      PairToRange(config.memory.hard_throughput.range));
  plots_[kUrlAvailablePlot].AddValue(
      curr_time, metrics.url_available,
      PairToRange(config.network.url_available.range));
  plots_[kInetThroughputPlot].AddValue(
      curr_time, metrics.inet_throughput,
      PairToRange(config.network.inet_throughout.range));
}

void MonitoringView::UpdateMetrics() {
  const Config config = controller_->GetConfig();
  const Metrics metrics = controller_->CollectMetrics();

  qint64 curr_time = QDateTime::currentDateTime().toMSecsSinceEpoch();

  UpdatePlots(curr_time, metrics, config);
  UpdateValues(metrics);
  UpdateCharts();
}

MonitoringView::~MonitoringView() { delete ui_; }

}  // namespace monsys
