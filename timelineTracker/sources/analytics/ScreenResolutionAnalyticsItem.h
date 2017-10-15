#ifndef SCREENRESOLUTIONANALYTICSITEM_H
#define SCREENRESOLUTIONANALYTICSITEM_H

#include <QtGui>
#include "AbstractAnalyticsItem.h"

class ScreenResolutionAnalyticsItem : public AbstractAnalyticsItem
{
public:
    ScreenResolutionAnalyticsItem();
    ~ScreenResolutionAnalyticsItem();

    // AbstractAnalyticsItem interface
    virtual QVector<QPair<QString, QString>> createPayload() const override;
};

#endif // SCREENRESOLUTIONANALYTICSITEM_H
