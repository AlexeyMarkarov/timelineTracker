#include "TimelineModel.h"

TimelineModel::TimelineModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

TimelineModel::~TimelineModel()
{
}

int TimelineModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    return parent.isValid() ? 0 : mTimeline.count();
}

QVariant TimelineModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const TimeSpan t = mTimeline.at(index.row());
    switch(role)
    {
    case R_TotalSpan:
        return t.start.toString() + QStringLiteral(" - ") + t.end.toString();
    case R_StartDateTime:
        return t.start.toString();
    case R_EndDateTime:
        return t.end.toString();
    }

    return QVariant();
}

QHash<int, QByteArray> TimelineModel::roleNames() const
{
    static const QHash<int, QByteArray> roles = {
        { R_TotalSpan,      "totalSpan" },
        { R_StartDateTime,  "startDateTime" },
        { R_EndDateTime,    "endDateTime" }
    };
    return roles;
}

QVector<TimeSpan> TimelineModel::getTimeline() const
{
    return mTimeline;
}

void TimelineModel::addTime(const TimeSpan time)
{
    if(!time.isValid())
    {
        qDebug() << Q_FUNC_INFO << "time is invalid:" << time.start << time.end;
        return;
    }

    beginInsertRows(QModelIndex(), mTimeline.count(), mTimeline.count());
    mTimeline.append(time);
    endInsertRows();
}

void TimelineModel::addTime(const QDateTime start, const QDateTime end)
{
    addTime(TimeSpan(start, end));
}

bool TimelineModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || row > mTimeline.count())
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    mTimeline.remove(row, count);
    endRemoveRows();
    return true;
}

void TimelineModel::removeRow(const int row)
{
    QAbstractListModel::removeRow(row);
}

QString TimelineModel::getStdText()
{
    const QDateTime dt = QDateTime::currentDateTime();
    return dt.toString() + QStringLiteral(" - ") + dt.toString();
}
