#ifndef TIMESPAN_H
#define TIMESPAN_H

#include <QtCore>

// Class representing a time interval with start and end datetime.
class TimeSpan
{
public:
    QString id;
    QDateTime start;
    QDateTime end;

    TimeSpan(const QDateTime s = QDateTime(), const QDateTime e = QDateTime());

    bool isValid() const;
    qint64 getSpanMsec() const;
};

#endif // TIMESPAN_H
