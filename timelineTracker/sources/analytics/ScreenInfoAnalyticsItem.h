#ifndef SCREENRESOLUTIONANALYTICSITEM_H
#define SCREENRESOLUTIONANALYTICSITEM_H

#include <QtGui>
#include "AbstractAnalyticsItem.h"

class ScreenInfoAnalyticsItem : public AbstractAnalyticsItem
{
public:
    ScreenInfoAnalyticsItem();
    ~ScreenInfoAnalyticsItem();

    // AbstractAnalyticsItem interface
    virtual QVector<QPair<QString, QString>> createPayload() const override;
};

#endif // SCREENRESOLUTIONANALYTICSITEM_H
