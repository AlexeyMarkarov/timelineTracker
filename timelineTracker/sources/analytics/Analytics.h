#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <QtCore>
#include <QtNetwork>
#include <random>
#include "AbstractAnalyticsItem.h"

// Main analytics manager. Sends provided analytics items to Google Analytics.
class Analytics : public QObject
{
    Q_OBJECT

public:
    // Global instance.
    static Analytics &inst();

    ~Analytics();

    // Initialize/cleanup methods. Called once on startup and shutdown.
    bool init();
    void release();

    // Send analytics data.
    void send(const AbstractAnalyticsItemPtr item);
    void send(const QVector<AbstractAnalyticsItemPtr> items);

private:
    Analytics(QObject *parent = nullptr);
    QByteArrayList createPayloads(const QVector<AbstractAnalyticsItemPtr> items);
    void send(const QByteArrayList payloads);

    QNetworkAccessManager mNet;
    QString mUserId;
    std::default_random_engine mRandGen;
    std::uniform_int_distribution<uint> mRand;
};

#endif // ANALYTICS_H
