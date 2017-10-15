#include "ScreenResolutionAnalyticsItem.h"

ScreenResolutionAnalyticsItem::ScreenResolutionAnalyticsItem()
{
}

ScreenResolutionAnalyticsItem::~ScreenResolutionAnalyticsItem()
{
}

QVector<QPair<QString, QString>> ScreenResolutionAnalyticsItem::createPayload() const
{
    QVector<QPair<QString, QString>> payload;
    const QSize screenSize = qApp->primaryScreen()->size();
    payload.append(qMakePair(QStringLiteral("sr"),
                             QStringLiteral("%1x%2").arg(screenSize.width()).arg(screenSize.height())));
    return payload;
}
