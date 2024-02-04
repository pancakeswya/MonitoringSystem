#ifndef MONITORINGSYSTEM_SRC_VIEW_PLOT_H_
#define MONITORINGSYSTEM_SRC_VIEW_PLOT_H_

#include <QtCharts/QtCharts>
#include <QString>

namespace monsys {

class Plot {
public:
    struct Range {
      qreal min;
      qreal max;
    };

    Plot();
    ~Plot();
    void SetTitle(const QString& title);
    void AddValue(qreal x, qreal y, const Range& range_y);
    QChart* Chart() const noexcept;
private:
    void Setup();

    static constexpr int kPenWidth = 3;
    static constexpr int kColorPen = 0x059605;

    static constexpr int kColorFirtsGrad = 0x3cc63c;
    static constexpr int kColorSecondGrad = 0x26f626;

    QLineSeries* line_series_;
    QAreaSeries* area_series_;
    QChart* chart_;
    QDateTimeAxis* x_axis_;
    QValueAxis* y_axis_;
};

} // namespace monsys


#endif // MONITORINGSYSTEM_SRC_VIEW_PLOT_H_
