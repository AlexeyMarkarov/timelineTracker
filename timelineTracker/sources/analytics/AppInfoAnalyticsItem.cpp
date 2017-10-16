#include "AppInfoAnalyticsItem.h"

AppInfoAnalyticsItem::AppInfoAnalyticsItem()
{
}

AppInfoAnalyticsItem::~AppInfoAnalyticsItem()
{
}

QVector<QPair<QString, QString>> AppInfoAnalyticsItem::createPayload() const
{
    QVector<QPair<QString, QString>> payload;
    payload.append(qMakePair(QStringLiteral("an"), qApp->applicationName()));
    payload.append(qMakePair(QStringLiteral("av"), qApp->applicationVersion()));
    return payload;
}
