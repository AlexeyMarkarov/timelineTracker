#ifndef TIMELINEMODEL_H
#define TIMELINEMODEL_H

#include <QtCore>
#include "TimeSpan.h"

class TimelineModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles
    {
        R_TotalSpan = Qt::DisplayRole,
        R_StartDateTime = Qt::UserRole + 1,
        R_EndDateTime
    };

    // Get a general representation of what R_TotalSpan usually contains. Used for UI layouts.
    static QString getStdText();

    TimelineModel(QObject *parent = nullptr);
    ~TimelineModel();

    // QAbstractListModel interface
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Get all stored time spans
    QVector<TimeSpan> getTimeline() const;

public slots:
    void addTime(const TimeSpan time);
    void addTime(const QDateTime start, const QDateTime end);
    void removeRow(const int row);

private:
    QVector<TimeSpan> mTimeline;
};

#endif // TIMELINEMODEL_H
