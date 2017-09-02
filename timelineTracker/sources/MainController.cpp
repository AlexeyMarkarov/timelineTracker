#include "MainController.h"
#include "MainWindow.h"
#include "TimelineModel.h"
#include "ChartView.h"
#include "Settings.h"
#include <QtCharts>

MainController::MainController(QObject *parent)
    : QObject(parent)
{
}

MainController::~MainController()
{
}

bool MainController::init()
{
    mTimeline = new TimelineModel(this);
    mWnd = new MainWindow(this);
    mWnd->setTimeModel(mTimeline);

    connect(mWnd, &MainWindow::addTimeClicked,  mTimeline, QOverload<const QDateTime, const QDateTime>::of(&TimelineModel::addTime));
    connect(mWnd, &MainWindow::removeTimeEntry, mTimeline, &TimelineModel::removeRow);
    connect(mWnd, &MainWindow::closing, this, &MainController::onWindowClosing);

    connect(mTimeline, &TimelineModel::rowsInserted,    this, &MainController::updateOutput);
    connect(mTimeline, &TimelineModel::rowsRemoved,     this, &MainController::updateOutput);
    connect(mTimeline, &TimelineModel::dataChanged,     this, &MainController::updateOutput);

    if(mWnd->isCreated())
    {
        QWindow::Visibility visibility = static_cast<QWindow::Visibility>(Settings::get(Settings::Type::WindowVisibility).toInt());
        switch(visibility)
        {
        case QWindow::Windowed:
        case QWindow::Minimized:
        case QWindow::Maximized:
        case QWindow::FullScreen:
        case QWindow::AutomaticVisibility:
            // do nothing
            break;

        default:
            visibility = QWindow::Windowed;
        }

        const QRect prevWindowRect = QRect(Settings::get(Settings::Type::WindowPosition).toPoint(),
                                           Settings::get(Settings::Type::WindowSize).toSize())
                                     .normalized()
                                     .intersected(qApp->desktop()->availableGeometry())
                                     .adjusted(0, qApp->style()->pixelMetric(QStyle::PM_TitleBarHeight), 0, 0);

        mWnd->setPosition(prevWindowRect.topLeft());
        mWnd->setSize(prevWindowRect.size().expandedTo(mWnd->getMinimumSize()));
        mWnd->setVisibility(visibility);
    }

    return mWnd->isCreated();
}

void MainController::release()
{
    Settings::set(Settings::Type::FirstRun, false);
    Settings::set(Settings::Type::WindowPosition, mWnd->getPosition());
    Settings::set(Settings::Type::WindowSize, mWnd->getSize());
    Settings::set(Settings::Type::WindowVisibility, mWnd->getVisibility());
}

void MainController::updateOutput()
{
    updateChart();
    updateTotalTime();
}

void MainController::updateChart()
{
    static const QByteArray kPropSpanId("timespanId");

    const QVector<TimeSpan> timeline = mTimeline->getTimeline();
    if(timeline.isEmpty())
    {
        mWnd->getChartView().removeAllSeries();
        return;
    }

    // create series<->id map
    QHash<QString, QAbstractSeries*> mapIdToSeries;
    for(int i = 0; i < mWnd->getChartView().count(); ++i)
    {
        if(QAbstractSeries *series = mWnd->getChartView().series(i))
        {
            mapIdToSeries.insert(series->property(kPropSpanId).toString(), series);
        }
    }

    // remove data from chart that is not in current timeline
    for(auto iter = mapIdToSeries.constBegin(); iter != mapIdToSeries.constEnd();)
    {
        const QString spanId = iter.key();
        const auto spanIter = std::find_if(timeline.constBegin(),
                                           timeline.constEnd(),
                                           [spanId](const TimeSpan &ts) { return ts.id == spanId; });
        if(spanIter == timeline.constEnd())
        {
            mWnd->getChartView().removeSeries(iter.value());
            iter = mapIdToSeries.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    // try to find required axes
    const QVector<QAbstractAxis*> axes = mWnd->getChartView().axes();
    QDateTimeAxis *dateTimeAxis = nullptr;
    QValueAxis *valueAxis = nullptr;
    for(QAbstractAxis *axis : axes)
    {
        if(QDateTimeAxis *dtAxis = qobject_cast<QDateTimeAxis*>(axis))
        {
            dateTimeAxis = dtAxis;
        }
        else if(QValueAxis *vAxis = qobject_cast<QValueAxis*>(axis))
        {
            valueAxis = vAxis;
        }
    }
    // create axes if they don't already exist
    if(!dateTimeAxis)
        dateTimeAxis = new QDateTimeAxis(&mWnd->getChartView());
    if(!valueAxis)
        valueAxis = new QValueAxis(&mWnd->getChartView());

    // calc chart boundaries
    QDateTime minDt = timeline.first().start, maxDt = timeline.first().end;
    qint64 maxMsec = timeline.first().getSpanMsec();
    for(const TimeSpan &span : timeline)
    {
        minDt = std::min(minDt, span.start);
        maxDt = std::max(maxDt, span.end);
        maxMsec = std::max(maxMsec, span.getSpanMsec());
    }

    if(maxMsec <= 0)
    {
        qDebug() << "unable to update chart: maximum time span is zero msec";
        return;
    }

    // set chart boundaries
    dateTimeAxis->setMin(roundHour(minDt));
    dateTimeAxis->setMax(roundHour(maxDt).addSecs(60 * 60));
    const int maxMinutes = maxMsec / 1000 / 60;
    const int minutesMultiplier = powf(10.0f, floorf(log10f(maxMinutes))) + 0.5f;
    const int minutesNumber = maxMinutes / minutesMultiplier;
    valueAxis->setMin(0);
    valueAxis->setMax((minutesNumber + 1) * minutesMultiplier);
    valueAxis->setTickCount(minutesNumber + 2);
    valueAxis->setTitleText(tr("Minutes"));

    // add data that is not already in chart
    for(const TimeSpan &span : timeline)
    {
        if(mapIdToSeries.contains(span.id))
        {
            // TODO: update series data
        }
        else
        {
            QAbstractSeries *series = mWnd->getChartView().createSeries(QAbstractSeries::SeriesTypeArea,
                                                                        QString("%1<br/>%2")
                                                                        .arg(span.start.time().toString("HH:mm"))
                                                                        .arg(span.end.time().toString("HH:mm")),
                                                                        dateTimeAxis,
                                                                        valueAxis);
            if(QAreaSeries *areaSeries = qobject_cast<QAreaSeries*>(series))
            {
                QLineSeries *lineSeries = new QLineSeries(areaSeries);
                lineSeries->append(span.start.toMSecsSinceEpoch(), span.getSpanMsec() / 1000 / 60);
                lineSeries->append(span.end.toMSecsSinceEpoch(), span.getSpanMsec() / 1000 / 60);

                areaSeries->setUpperSeries(lineSeries);
                areaSeries->setOpacity(0.5);
                areaSeries->setProperty(kPropSpanId, span.id);
                areaSeries->setPointsVisible();
            }
            else
            {
                mWnd->getChartView().removeSeries(series);
            }
        }
    }
}

void MainController::updateTotalTime()
{
    QVector<TimeSpan> timeline = mTimeline->getTimeline();
    std::sort(timeline.begin(), timeline.end(), [](const TimeSpan &a, const TimeSpan &b) { return a.start < b.start; });
    for(int i = timeline.count() - 1; i > 0; --i)
    {
        const TimeSpan &thisSpan = timeline.at(i);
        TimeSpan &prevSpan = timeline[i - 1];
        if(prevSpan.end >= thisSpan.start)
        {
            prevSpan.end = std::max(prevSpan.end, thisSpan.end);
            timeline.removeAt(i);
        }
    }

    qint64 totalTimeMsec = 0;
    for(const TimeSpan &span : timeline)
    {
        totalTimeMsec += span.getSpanMsec();
    }

    mWnd->setTotalTimeText(QString("%1 minutes\n%2 hours")
                           .arg(totalTimeMsec / 1000 / 60)
                           .arg(totalTimeMsec / 1000.0 / 60.0 / 60.0, 0, 'f', 2));
}

QDateTime MainController::roundHour(const QDateTime &dt)
{
    return dt
            .addMSecs(-dt.time().msec())
            .addSecs(-dt.time().second())
            .addSecs(-dt.time().minute() * 60);
}

void MainController::onWindowClosing()
{
    qApp->exit();
}
