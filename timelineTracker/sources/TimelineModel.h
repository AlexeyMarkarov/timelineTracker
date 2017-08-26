#ifndef TIMELINEMODEL_H
#define TIMELINEMODEL_H

#include <QtCore>

class TimeSpan
{
public:
    QDateTime start;
    QDateTime end;

    TimeSpan(const QDateTime s = QDateTime(), const QDateTime e = QDateTime())
        : start(std::min(s, e)), end(std::max(s, e))
    {}

    bool isValid() const
    {
        return start.isValid() && end.isValid();
    }

    qint64 getSpanMsec() const
    {
        return start.msecsTo(end);
    }
};

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

    TimelineModel(QObject *parent = nullptr);
    ~TimelineModel();

    // QAbstractListModel interface
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    QVector<TimeSpan> getTimeline() const;

public slots:
    void addTime(const TimeSpan time);
    void addTime(const QDateTime start, const QDateTime end);
    void removeRow(const int row);

private:
    QVector<TimeSpan> mTimeline;
};

#endif // TIMELINEMODEL_H
