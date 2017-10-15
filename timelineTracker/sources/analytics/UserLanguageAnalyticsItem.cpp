#include "UserLanguageAnalyticsItem.h"

UserLanguageAnalyticsItem::UserLanguageAnalyticsItem()
{
}

UserLanguageAnalyticsItem::~UserLanguageAnalyticsItem()
{
}

QVector<QPair<QString, QString>> UserLanguageAnalyticsItem::createPayload() const
{
    QVector<QPair<QString, QString>> payload;
    payload.append(qMakePair(QStringLiteral("ul"), QLocale::languageToString(QLocale::system().language())));
    return payload;
}
