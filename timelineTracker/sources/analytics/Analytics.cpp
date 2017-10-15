#include "Analytics.h"
#include "../Settings.h"

static const QString kAnalyticsVersion("1");
static const QString kTrackingID("UA-XXXX-Y");
static const QString kGaEndpointCollect("https://www.google-analytics.com/collect");
static const QString kGaEndpointBatch("https://www.google-analytics.com/batch");

Analytics &Analytics::inst()
{
    static Analytics a;
    return a;
}

Analytics::Analytics(QObject *parent)
    : QObject(parent)
    , mRandGen(QDateTime::currentMSecsSinceEpoch())
    , mRand(0, UINT_MAX)
{
}

Analytics::~Analytics()
{
}

bool Analytics::init()
{
    mUserId = QUuid::createUuid().toString();
    return true;
}

void Analytics::release()
{
}

void Analytics::send(const AbstractAnalyticsItem &item)
{
    if(!Settings::get(Settings::Type::AnalyticsEnabled).toBool())
        return;

    const QVector<QPair<QString, QString>> itemPayload = item.createPayload();
    if(itemPayload.isEmpty())
    {
        qWarning() << "item" << typeid(item).name() << "produces empty query";
        return;
    }

    QUrlQuery payload;
    payload.addQueryItem("v", kAnalyticsVersion);
    payload.addQueryItem("tid", kTrackingID);
    payload.addQueryItem("uid", mUserId);
    for(auto payloadItem : itemPayload)
    {
        payload.addQueryItem(payloadItem.first, payloadItem.second);
    }
    payload.addQueryItem("z", QString::number(mRand(mRandGen)));
    const QByteArray payloadData = payload.query().toLatin1();

    QNetworkReply *reply = mNet.post(QNetworkRequest(QUrl(kGaEndpointCollect)), payloadData);
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), [reply](const QNetworkReply::NetworkError code)
    {
        qWarning() << "analytics error" << code << reply->errorString();
    });
    connect(reply, &QNetworkReply::finished, [reply]()
    {
        const QByteArray data = reply->readAll();
        qDebug() << "analytics respond:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) << data;
        reply->deleteLater();
    });

}
