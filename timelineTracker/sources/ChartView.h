#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QtCore>
#include <QtCharts>

// A wrapper over QML ChartView.
class ChartView : public QObject
{
    Q_OBJECT

public:
    ChartView(QObject *parent = nullptr);
    ~ChartView();

    void setWrappedObject(QObject *chartWidget);

    // QML ChartView interface
    void removeAllSeries();
    void removeSeries(QAbstractSeries *series);
    QAbstractSeries *createSeries(QAbstractSeries::SeriesType type, const QString name, QAbstractAxis *axisX, QAbstractAxis *axisY);
    void setAxisX(QAbstractAxis *axis, QAbstractSeries *series);
    void setAxisY(QAbstractAxis *axis, QAbstractSeries *series);
    QVector<QAbstractAxis*> axes() const;
    int count() const;
    QAbstractSeries *series(const int index);

private:
    QPointer<QObject> mChart;
};

#endif // CHARTVIEW_H
