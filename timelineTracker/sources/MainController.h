#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QtCore>
#include <QtCharts>
#include <QtWidgets>

class MainWindow;
class TimelineModel;

class MainController : public QObject
{
    Q_OBJECT
public:
    MainController(QObject *parent = nullptr);
    ~MainController();

    bool init();
    void release();

private slots:
    void updateOutput();
    void onWindowClosing();
    void onLogsClicked();

private:
    MainWindow *mWnd;
    TimelineModel *mTimeline;

    void updateChart();
    void updateTotalTime();
    static QDateTime roundHour(const QDateTime &dt);
};

#endif // MAINCONTROLLER_H
