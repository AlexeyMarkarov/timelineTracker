#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtQml>
#include <QtGui>

class ChartView;

class MainWindow : public QObject
{
    Q_OBJECT

public:
    MainWindow(QObject *parent = nullptr);
    ~MainWindow();

    // QML Window interface
    QWindow::Visibility getVisibility() const;
    void setVisibility(const QWindow::Visibility visibility);
    QSize getSize() const;
    void setSize(const QSize &size);
    QSize getMinimumSize() const;
    void setMinimumSize(const QSize &size);
    QPoint getPosition() const;
    void setPosition(const QPoint &pos);
    void setFlashHelpIndicator(const bool flash);

    bool isCreated() const;
    ChartView &getChartView() const;
    void setTimeModel(QAbstractItemModel *model);
    void setTotalTimeText(const QString text);

signals:
    void addTimeClicked(const QDateTime start, const QDateTime end);
    void removeTimeEntryClicked(const int index);
    void clearTimeClicked();
    void closing();

private:
    QQmlEngine mEngine;
    QQmlComponent *mRootComponent;
    QObject *mWindow;
    ChartView *mChart;

    void createMembers();
    void connectSignals();
};

#endif // MAINWINDOW_H
