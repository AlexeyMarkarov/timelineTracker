#include "Analytics.h"
#include "../Settings.h"
#include "../Util.h"

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
    const QString userInfo = QSysInfo::currentCpuArchitecture()
                             + QSysInfo::machineHostName()
                             + QSysInfo::productType()
                             + QSysInfo::productVersion()
                             + Util::getUserName();
    mUserId = QCryptographicHash::hash(userInfo.toUtf8(), QCryptographicHash::Sha512);
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
    payload.addQueryItem("aip", "1");
    for(auto payloadItem : itemPayload)
    {
        payload.addQueryItem(payloadItem.first, payloadItem.second);
    }
    payload.addQueryItem("z", QString::number(mRand(mRandGen)));
    const QByteArray payloadData = payload.query().toLatin1();

    QNetworkRequest request(kGaEndpointCollect);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *reply = mNet.post(request, payloadData);
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), [reply](const QNetworkReply::NetworkError code)
    {
        qWarning() << "analytics error" << code << reply->errorString();
    });
    connect(reply, &QNetworkReply::finished, [reply]()
    {
        if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString().startsWith('2'))
        {
            // Google Analytics returns 2xx code if request was received.
        }
        else
        {
            const QByteArray data = reply->readAll();
            qWarning() << "analytics error:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) << data;
        }
        reply->deleteLater();
    });

}
