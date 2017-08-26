#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtQml>

class MainWindow : public QObject
{
    Q_OBJECT

public:
    MainWindow(QObject *parent = nullptr);
    ~MainWindow();

    bool isCreated() const;

    void setTimeModel(QAbstractItemModel *model);
    void setTotalTimeText(const QString text);

signals:
    void addTimeClicked(const QDateTime start, const QDateTime end);
    void removeTimeEntry(const int index);

private:
    QQmlEngine mEngine;
    QQmlComponent *mRootComponent;
    QObject *mWindow;

    void createMembers();
    void connectSignals();
};

#endif // MAINWINDOW_H
