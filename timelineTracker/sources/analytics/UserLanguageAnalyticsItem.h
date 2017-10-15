#ifndef USERLANGUAGEANALYTICSITEM_H
#define USERLANGUAGEANALYTICSITEM_H

#include "AbstractAnalyticsItem.h"

class UserLanguageAnalyticsItem : public AbstractAnalyticsItem
{
public:
    UserLanguageAnalyticsItem();
    ~UserLanguageAnalyticsItem();

    // AbstractAnalyticsItem interface
    virtual QVector<QPair<QString, QString>> createPayload() const override;
};

#endif // USERLANGUAGEANALYTICSITEM_H
