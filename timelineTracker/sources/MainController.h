#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QtCore>

class MainWindow;

class MainController : public QObject
{
    Q_OBJECT
public:
    MainController(QObject *parent = nullptr);
    ~MainController();

    bool init();
    void release();

private:
    MainWindow *mWnd;
};

#endif // MAINCONTROLLER_H
