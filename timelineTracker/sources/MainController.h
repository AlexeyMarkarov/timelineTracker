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
    void updateTotalTime();

private:
    MainWindow *mWnd;
    TimelineModel *mTimeline;
};

#endif // MAINCONTROLLER_H
