#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <QtCore>
#include <QtNetwork>
#include <QtQml>
#include <random>
#include "AbstractAnalyticsItem.h"

// Main analytics manager. Sends provided analytics items to Google Analytics.
class Analytics : public QObject
{
    Q_OBJECT

public:
    enum class Type
    {
        // Screenviews
        MainWindowView,

        // Events
        StartupEvent,
        ShutdownEvent,
        HelpButtonClickEvent,
        LogsButtonClickEvent,
        AboutQtButtonClickEvent,
        AddButtonClickEvent,
        RemoveButtonClickEvent,
        ClearButtonClickEvent,
    };
    Q_ENUM(Type)

    // Global instance.
    static Analytics &inst();
    // A way for QML to get access to Analytics.
    static QObject *qmlSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

    ~Analytics();

    // Initialize/cleanup methods. Called once on startup and shutdown.
    bool init();
    void release();

public slots:
    // Send analytics data. These methods are thread-safe.
    void send(const Type type);
    void send(const QVector<Type> types);

private slots:
    void onNetworkReplyError(const QNetworkReply::NetworkError error);
    void onNetworkReplyFinished();
    void send_p(const QVector<Type> types);

private:
    Analytics(QObject *parent = nullptr);
    QByteArrayList createPayloads(const QVector<Type> types);
    void send(const QByteArrayList payloads);

    QNetworkAccessManager mNet;
    std::default_random_engine mRandGen;
    std::uniform_int_distribution<uint> mRand;
    QList<QNetworkReply*> mReplies;
};

#endif // ANALYTICS_H
