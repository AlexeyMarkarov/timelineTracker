#ifndef APPINFOANALYTICSITEM_H
#define APPINFOANALYTICSITEM_H

#include "AbstractAnalyticsItem.h"
#include <QtGui>

class AppInfoAnalyticsItem : public AbstractAnalyticsItem
{
public:
    AppInfoAnalyticsItem();
    ~AppInfoAnalyticsItem();

    // AbstractAnalyticsItem interface
    virtual QVector<QPair<QString, QString>> createPayload() const override;
};

#endif // APPINFOANALYTICSITEM_H
