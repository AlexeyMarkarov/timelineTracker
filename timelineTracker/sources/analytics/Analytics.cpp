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
    return true;
}

void Analytics::release()
{
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

QByteArrayList Analytics::createPayloads(const QVector<Type> types)
{
    QString clientId = Settings::get(Settings::Type::AnalyticsClientId).toString();
    if(clientId.isEmpty())
    {
        clientId = QUuid::createUuid().toString();
        Settings::set(Settings::Type::AnalyticsClientId, clientId);
    }

    QByteArrayList payloads;
    for(const Type type : types)
    {
        QUrlQuery payload;
        payload.addQueryItem("v", kAnalyticsVersion);
        payload.addQueryItem("tid", kTrackingID);
        payload.addQueryItem("cid", clientId);
        payload.addQueryItem("aip", "1");

        bool isTypeValid = true;
        switch(type)
        {
        case Type::MainWindowView:
        {
            payload.addQueryItem("t", "screenview");
            payload.addQueryItem("cd", "Main Window");
            break;
        }
        case Type::StartupEvent:
        {
            payload.addQueryItem("t", "event");
            payload.addQueryItem("ec", "App");
            payload.addQueryItem("ea", "Startup");
            payload.addQueryItem("el", "App Startup");
            payload.addQueryItem("sc", "start");
            break;
        }
        case Type::ShutdownEvent:
        {
            payload.addQueryItem("t", "event");
            payload.addQueryItem("ec", "App");
            payload.addQueryItem("ea", "Shutdown");
            payload.addQueryItem("el", "App Shutdown");
            payload.addQueryItem("sc", "end");
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

        payload.addQueryItem("an", qApp->applicationName());
        payload.addQueryItem("av", qApp->applicationVersion());
        const QSize screenSize = qApp->primaryScreen()->size();
        payload.addQueryItem("sr", QStringLiteral("%1x%2").arg(screenSize.width()).arg(screenSize.height()));
        payload.addQueryItem("sd", QString::number(qApp->primaryScreen()->depth()));
        payload.addQueryItem("ul", QLocale::languageToString(QLocale::system().language()));
        payload.addQueryItem("cd1", QSysInfo::prettyProductName());
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
