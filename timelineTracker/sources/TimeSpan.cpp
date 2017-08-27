#include "TimeSpan.h"

TimeSpan::TimeSpan(const QDateTime s, const QDateTime e)
    : id(QUuid::createUuid().toString())
    , start(std::min(s, e))
    , end(std::max(s, e))
{
}

bool TimeSpan::isValid() const
{
    return start.isValid() && end.isValid();
}

qint64 TimeSpan::getSpanMsec() const
{
    return start.msecsTo(end);
}
