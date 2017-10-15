#ifndef ABSTRACTANALYTICSITEM_H
#define ABSTRACTANALYTICSITEM_H

#include <QtCore>

// Base class for all analytics data items. Used with Analytics class.
class AbstractAnalyticsItem
{
public:
    AbstractAnalyticsItem();
    virtual ~AbstractAnalyticsItem();

    // Convert this item data to a query suitable for feeding into Google Analytics.
    // Put only variables essential to this item, Analytics object will add the rest of variables required by GA.
    virtual QVector<QPair<QString, QString>> createPayload() const = 0;
};

#endif // ABSTRACTANALYTICSITEM_H
