#include "MainController.h"
#include "MainWindow.h"
#include "TimelineModel.h"
#include "ChartView.h"
#include "Settings.h"
#include "Util.h"
#include "Logger.h"
#include "analytics/Analytics.h"

MainController::MainController(QObject *parent)
    : QObject(parent)
{
}

MainController::~MainController()
{
}

bool MainController::init()
{
    Logger::init();
    qInfo() << "App starting... version is" << qApp->applicationVersion();

    qInfo() << "Initializing analytics...";
    mAnalyticsThread.start(QThread::LowestPriority);
    Analytics::inst().moveToThread(&mAnalyticsThread);
    if(!Analytics::inst().init())
    {
        qWarning() << "Analytics initialization failed.";
    }

    Analytics::inst().send(Analytics::Type::StartupEvent);

    qInfo() << "Initializing meta types...";
    qRegisterMetaType<QStyle::PixelMetric>("QStyle::PixelMetric");
    qRegisterMetaType<Analytics::Type>("Analytics::Type");
    qRegisterMetaType<QVector<Analytics::Type>>("QVector<Analytics::Type>");
    qmlRegisterUncreatableMetaObject(QStyle::staticMetaObject, "Qt.Widgets", 1, 0, "QStyle", "QStyle metaobject only.");
    qmlRegisterSingletonType<Util>("TimelineTracker", 1, 0, "Util", Util::qmlSingletonProvider);
    qmlRegisterSingletonType<Analytics>("TimelineTracker", 1, 0, "Analytics", Analytics::qmlSingletonProvider);
    qmlRegisterSingletonType<Settings>("TimelineTracker", 1, 0, "Settings", Settings::qmlSingletonProvider);

    qInfo() << "Creating members...";
    mTimeline = new TimelineModel(this);
    mWnd = new MainWindow(this);
    mWnd->setTimeModel(mTimeline);

    qInfo() << "Connecting signals...";
    connect(mWnd, &MainWindow::addTimeClicked,         mTimeline, QOverload<const QDateTime, const QDateTime>::of(&TimelineModel::addTime));
    connect(mWnd, &MainWindow::removeTimeEntryClicked, mTimeline, &TimelineModel::removeRow);
    connect(mWnd, &MainWindow::clearTimeClicked,       mTimeline, &TimelineModel::clear);
    connect(mWnd, &MainWindow::logsClicked,            this,      &MainController::onLogsClicked);
    connect(mWnd, &MainWindow::aboutQtClicked,         qApp,      &QApplication::aboutQt);
    connect(mWnd, &MainWindow::closing,                this,      &MainController::onWindowClosing);

    connect(mTimeline, &TimelineModel::rowsInserted, this, &MainController::updateOutput);
    connect(mTimeline, &TimelineModel::rowsRemoved,  this, &MainController::updateOutput);
    connect(mTimeline, &TimelineModel::dataChanged,  this, &MainController::updateOutput);

    qInfo() << "Initializing main window...";
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
        mWnd->setFlashHelpIndicator(Settings::get(Settings::Type::FirstRun).toBool());

        Analytics::inst().send(Analytics::Type::MainWindowView);
    }
    else
    {
        const QString logPath = QUrl::fromLocalFile((Logger::getLogFilePath())).toString();
        QMessageBox::critical(nullptr,
                              qApp->applicationName(),
                              tr("Startup failed. See <a href='%1'>log</a> for more details.").arg(logPath));
    }

    return mWnd->isCreated();
}

void MainController::release()
{
    qInfo() << "Saving settings...";
    Settings::set(Settings::Type::FirstRun, false);
    Settings::set(Settings::Type::WindowPosition, mWnd->getPosition());
    Settings::set(Settings::Type::WindowSize, mWnd->getSize());
    Settings::set(Settings::Type::WindowVisibility, mWnd->getVisibility());

    qInfo() << "Destroying main window...";
    delete mWnd;

    qInfo() << "Destroying analytics...";
    Analytics::inst().send(Analytics::Type::ShutdownEvent);
    Analytics::inst().release();
    mAnalyticsThread.quit();
    if(!mAnalyticsThread.wait(1000))
    {
        mAnalyticsThread.terminate();
    }

    qInfo() << "Closing logs...";
    Logger::release();
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
    valueAxis->setMinorTickCount(9);
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
                areaSeries->setPointLabelsFormat(QString("%1m/%2h")
                                                 .arg(span.getSpanMsec() / 1000 / 60)
                                                 .arg(span.getSpanMsec() / 1000.0 / 60.0 / 60.0, 0, 'f', 2));

                connect(areaSeries, &QAreaSeries::hovered, [areaSeries](const QPointF &point, bool state) {
                    Q_UNUSED(point);
                    areaSeries->setPointLabelsVisible(state);
                });
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
    std::sort(timeline.begin(), timeline.end(), [](const TimeSpan &a, const TimeSpan &b) {
        // sort by start time (earlier goes first), and then by duration (larger goes first)
        return a.start == b.start ? a.getSpanMsec() > b.getSpanMsec() : a.start < b.start;
    });
    for(int i = timeline.count() - 1; i > 0; --i)
    {
        const TimeSpan &thisSpan = timeline.at(i);
        bool remove = false;
        for(int j = i - 1; j >= 0; --j)
        {
            TimeSpan &prevSpan = timeline[j];
            if(prevSpan.end >= thisSpan.start)
            {
                prevSpan.end = std::max(prevSpan.end, thisSpan.end);
                remove = true;
            }
        }
        if(remove)
        {
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

void MainController::onLogsClicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(Logger::getLogFilePath()));
}
