#include "Analytics.h"
#include "../Settings.h"
#include "../Util.h"

//#define DEBUG_ANALYTICS

static const QString kAnalyticsVersion("1");
static const QString kTrackingID("UA-108074830-1");
static const QString kGaEndpointCollect
#ifdef DEBUG_ANALYTICS
("https://www.google-analytics.com/debug/collect");
#else
("https://www.google-analytics.com/collect");
#endif
static const QString kGaEndpointBatch
#ifdef DEBUG_ANALYTICS
("https://www.google-analytics.com/debug/batch");
#else
("https://www.google-analytics.com/batch");
#endif

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
    mUserId = QCryptographicHash::hash(userInfo.toUtf8(), QCryptographicHash::Sha512).toHex();
    return true;
}

void Analytics::release()
{
}

void Analytics::send(const AbstractAnalyticsItemPtr item)
{
    send(QVector<AbstractAnalyticsItemPtr>() << item);
}

void Analytics::send(const QVector<AbstractAnalyticsItemPtr> items)
{
    if(items.isEmpty())
        return;

    const QByteArrayList payloads = createPayloads(items);
    if(payloads.isEmpty())
    {
        qWarning() << "items produced empty payloads";
        return;
    }

    send(payloads);
}

void Analytics::send(const Type type)
{
    send(QVector<Type>() << type);
}

void Analytics::send(const QVector<Type> types)
{
    if(types.isEmpty())
        return;

    const QByteArrayList payloads = createPayloads(types);
    if(payloads.isEmpty())
    {
        qWarning() << "items produced empty payloads";
        return;
    }

    send(payloads);
}

QByteArrayList Analytics::createPayloads(const QVector<AbstractAnalyticsItemPtr> items)
{
    QByteArrayList payloads;
    for(AbstractAnalyticsItemPtr item : items)
    {
        if(item.isNull())
            continue;

        const QVector<QPair<QString, QString>> itemPayload = item->createPayload();
        if(itemPayload.isEmpty())
        {
            qWarning() << "item" << typeid(*item).name() << "produced empty payload";
            continue;
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

        payloads.append(payloadData);
    }
    return payloads;
}

QByteArrayList Analytics::createPayloads(const QVector<Type> types)
{
    QByteArrayList payloads;
    for(const Type type : types)
    {
        QUrlQuery payload;
        payload.addQueryItem("v", kAnalyticsVersion);
        payload.addQueryItem("tid", kTrackingID);
        payload.addQueryItem("uid", mUserId);
        payload.addQueryItem("aip", "1");

        bool isTypeValid = true;
        switch(type)
        {
        case Type::StartupEvent:
        {
            payload.addQueryItem("t", "event");
            payload.addQueryItem("ni", "1");
            payload.addQueryItem("ec", "App");
            payload.addQueryItem("ea", "Startup");
            payload.addQueryItem("el", "App Startup");
            payload.addQueryItem("an", qApp->applicationName());
            payload.addQueryItem("av", qApp->applicationVersion());
            const QSize screenSize = qApp->primaryScreen()->size();
            payload.addQueryItem("sr", QStringLiteral("%1x%2").arg(screenSize.width()).arg(screenSize.height()));
            payload.addQueryItem("sd", QString::number(qApp->primaryScreen()->depth()));
            payload.addQueryItem("ul", QLocale::languageToString(QLocale::system().language()));
            break;
        }
        default:
        {
            qCritical() << "unknown analytics type:" << type;
            isTypeValid = false;
            break;
        }
        }

        if(!isTypeValid)
            continue;

        payload.addQueryItem("z", QString::number(mRand(mRandGen)));
        payloads.append(payload.query().toLatin1());
    }
    return payloads;
}

void Analytics::send(const QByteArrayList payloads)
{
    if(payloads.isEmpty())
        return;
    if(!Settings::get(Settings::Type::AnalyticsEnabled).toBool())
        return;

    // Google Analytics allows for up to 20 hits per batch.
    static const int maxBatchSize = 20;
    for(int i = 0; i < payloads.count(); i += maxBatchSize)
    {
        const QByteArrayList payloadChunk = payloads.mid(i, maxBatchSize);
        if(payloadChunk.isEmpty())
            continue;

        QNetworkRequest request(payloadChunk.count() == 1 ? kGaEndpointCollect : kGaEndpointBatch);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        QNetworkReply *reply = mNet.post(request, payloadChunk.join('\n'));
        connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), [reply](const QNetworkReply::NetworkError code)
        {
            qWarning() << "analytics error" << code << reply->errorString();
        });
        connect(reply, &QNetworkReply::finished, [reply]()
        {
            const QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
#ifdef DEBUG_ANALYTICS
            qDebug() << "analytics debug reply:" << statusCode << reply->readAll();
#else
            if(statusCode.toString().startsWith('2'))
            {
                // Google Analytics returns 2xx code if request was received.
            }
            else
            {
                const QByteArray data = reply->readAll();
                qWarning() << "analytics error:" << statusCode << data;
            }
#endif
            reply->deleteLater();
        });

    }
}
