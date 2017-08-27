#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QtCore>

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

private:
    MainWindow *mWnd;
    TimelineModel *mTimeline;

    void updateChart();
    void updateTotalTime();
};

#endif // MAINCONTROLLER_H
