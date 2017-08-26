#include "MainController.h"
#include "MainWindow.h"
#include "TimelineModel.h"

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

    connect(mTimeline, &TimelineModel::rowsInserted,    this, &MainController::updateTotalTime);
    connect(mTimeline, &TimelineModel::rowsRemoved,     this, &MainController::updateTotalTime);
    connect(mTimeline, &TimelineModel::dataChanged,     this, &MainController::updateTotalTime);

    return mWnd->isCreated();
}

void MainController::release()
{
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
