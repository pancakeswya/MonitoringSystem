#include "plot.h"


namespace monsys {


Plot::Plot() : line_series_(new QLineSeries),
         area_series_(new QAreaSeries(line_series_)),
         chart_(new QChart),
         x_axis_(new QDateTimeAxis),
         y_axis_(new QValueAxis) {
    Setup();
}

void Plot::Setup() {
    chart_->legend()->hide();

    x_axis_->setFormat("hh:mm:ss,z");
    x_axis_->setTickCount(5);

    y_axis_->setLabelFormat("%.1f");
    y_axis_->setTickCount(10);

    chart_->addAxis(x_axis_, Qt::AlignBottom);
    chart_->addAxis(y_axis_, Qt::AlignLeft);
    chart_->addSeries(area_series_);

    QPen pen(kColorPen);
    pen.setWidth(kPenWidth);

    QLinearGradient gradient(QPointF(0, 0), QPointF(0, 1));
    gradient.setColorAt(0.0, kColorFirtsGrad);
    gradient.setColorAt(1.0, kColorSecondGrad);
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);

    area_series_->setPen(pen);
    area_series_->setBrush(gradient);
    area_series_->attachAxis(x_axis_);
    area_series_->attachAxis(y_axis_);
}

void Plot::SetTitle(const QString& title) {
    chart_->setTitle(title);
}

void Plot::AddValue(qreal x, qreal y, const Range& range_y) {
    QDateTime min_date = x_axis_->min();
    if (min_date.toMSecsSinceEpoch() == 0) {
        min_date = QDateTime::fromMSecsSinceEpoch(x);
    }

    x_axis_->setRange(min_date, QDateTime::fromMSecsSinceEpoch(x));
    y_axis_->setRange(range_y.min, range_y.max);

    line_series_->setColor(Qt::red);
    line_series_->append(x, y);
}

QChart* Plot::Chart() const noexcept {
    return chart_;
}

Plot::~Plot() {
    delete line_series_;
    delete area_series_;
    delete chart_;
}

} // namespace monsys
