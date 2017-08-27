#include "ChartView.h"
#include <QtQml>

ChartView::ChartView(QObject *parent)
    : QObject(parent)
    , mChart(nullptr)
{
}

ChartView::~ChartView()
{
}

void ChartView::setWrappedObject(QObject *chartWidget)
{
    mChart = chartWidget;
}

void ChartView::removeAllSeries()
{
    if(!mChart) return;

    QMetaObject::invokeMethod(mChart, "removeAllSeries");
}

void ChartView::removeSeries(QAbstractSeries *series)
{
    if(!mChart) return;

    QMetaObject::invokeMethod(mChart, "removeSeries", Q_ARG(QAbstractSeries*, series));
}

QAbstractSeries *ChartView::createSeries(QAbstractSeries::SeriesType type, const QString name, QAbstractAxis *axisX, QAbstractAxis *axisY)
{
    if(!mChart) return nullptr;

    QAbstractSeries *returnValue;
    QMetaObject::invokeMethod(mChart, "createSeries",
                              Q_RETURN_ARG(QAbstractSeries*, returnValue),
                              Q_ARG(int, type),
                              Q_ARG(QString, name),
                              Q_ARG(QAbstractAxis*, axisX),
                              Q_ARG(QAbstractAxis*, axisY));
    return returnValue;
}

void ChartView::setAxisX(QAbstractAxis *axis, QAbstractSeries *series)
{
    if(!mChart) return;

    QMetaObject::invokeMethod(mChart, "setAxisX",
                              Q_ARG(QAbstractAxis*, axis),
                              Q_ARG(QAbstractSeries*, series));
}

void ChartView::setAxisY(QAbstractAxis *axis, QAbstractSeries *series)
{
    if(!mChart) return;

    QMetaObject::invokeMethod(mChart, "setAxisY",
                              Q_ARG(QAbstractAxis*, axis),
                              Q_ARG(QAbstractSeries*, series));
}

QVector<QAbstractAxis*> ChartView::axes() const
{
    if(!mChart) return QVector<QAbstractAxis*>();

    const QQmlListReference qmlList = QQmlProperty::read(mChart, "axes").value<QQmlListReference>();
    QVector<QAbstractAxis*> axes;
    for(int i = 0; i < qmlList.count(); ++i)
    {
        if(QAbstractAxis *axis = qobject_cast<QAbstractAxis*>(qmlList.at(i)))
        {
            axes.append(axis);
        }
    }
    return axes;
}

int ChartView::count() const
{
    if(!mChart) return 0;

    return QQmlProperty::read(mChart, "count").toInt();
}

QAbstractSeries *ChartView::series(const int index)
{
    if(!mChart) return nullptr;

    QAbstractSeries *returnValue;
    QMetaObject::invokeMethod(mChart, "series",
                              Q_RETURN_ARG(QAbstractSeries*, returnValue),
                              Q_ARG(int, index));
    return returnValue;
}
