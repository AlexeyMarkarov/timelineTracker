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

static const QString kEventCategoryApp("App");
static const QString kEventCategoryGui("GUI");

static const QString kEventActionStartup("Startup");
static const QString kEventActionShutdown("Shutdown");
static const QString kEventActionButtonClick("ButtonClick");

Analytics &Analytics::inst()
{
    static Analytics a;
    return a;
}

QObject *Analytics::qmlSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine)
    engine->setObjectOwnership(&inst(), QQmlEngine::CppOwnership);
    return &inst();
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
    while(!mReplies.isEmpty())
    {
        qApp->processEvents(QEventLoop::AllEvents, 1000);
    }
}

void Analytics::send(const Type type)
{
    QMetaObject::invokeMethod(this, "send_p", Q_ARG(const QVector<Type>, QVector<Type>() << type));
}

void Analytics::send(const QVector<Type> types)
{
    QMetaObject::invokeMethod(this, "send_p", Q_ARG(const QVector<Type>, types));
}

void Analytics::send_p(const QVector<Type> types)
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
    static const QMetaEnum metaEnum = QMetaEnum::fromType<Type>();

    QString clientId = Settings::get(Settings::Type::AnalyticsClientId).toString();
    if(clientId.isEmpty())
    {
        clientId = QUuid::createUuid().toString();
        Settings::set(Settings::Type::AnalyticsClientId, clientId);
    }

    static const QString osVersion = QString("%1 %2 %3 %4 %5")
                                     .arg(QSysInfo::productType())
                                     .arg(QSysInfo::productVersion())
                                     .arg(QSysInfo::kernelType())
                                     .arg(QSysInfo::kernelVersion())
                                     .arg(QSysInfo::currentCpuArchitecture());
    const QString screenSize = QStringLiteral("%1x%2")
                               .arg(qApp->primaryScreen()->size().width())
                               .arg(qApp->primaryScreen()->size().height());
    const QString screenDepth = QString::number(qApp->primaryScreen()->depth());
    const QString language = QLocale::languageToString(QLocale::system().language());

    QByteArrayList payloads;
    for(const Type type : types)
    {
        if(metaEnum.valueToKey(static_cast<int>(type)) == nullptr)
        {
            qCritical() << "unknown analytics type:" << type;
            continue;
        }

        QUrlQuery payload;
        payload.addQueryItem("v", kAnalyticsVersion);
        payload.addQueryItem("tid", kTrackingID);
        payload.addQueryItem("cid", clientId);
        payload.addQueryItem("aip", "1");

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
            payload.addQueryItem("ec", kEventCategoryApp);
            payload.addQueryItem("ea", kEventActionStartup);
            payload.addQueryItem("el", "App Startup");
            payload.addQueryItem("sc", "start");
            break;
        }
        case Type::ShutdownEvent:
        {
            payload.addQueryItem("t", "event");
            payload.addQueryItem("ec", kEventCategoryApp);
            payload.addQueryItem("ea", kEventActionShutdown);
            payload.addQueryItem("el", "App Shutdown");
            payload.addQueryItem("sc", "end");
            break;
        }
        case Type::HelpButtonClickEvent:
        {
            payload.addQueryItem("t", "event");
            payload.addQueryItem("ec", kEventCategoryGui);
            payload.addQueryItem("ea", kEventActionButtonClick);
            payload.addQueryItem("el", "Help");
            break;
        }
        case Type::LogsButtonClickEvent:
        {
            payload.addQueryItem("t", "event");
            payload.addQueryItem("ec", kEventCategoryGui);
            payload.addQueryItem("ea", kEventActionButtonClick);
            payload.addQueryItem("el", "Logs");
            break;
        }
        case Type::AboutQtButtonClickEvent:
        {
            payload.addQueryItem("t", "event");
            payload.addQueryItem("ec", kEventCategoryGui);
            payload.addQueryItem("ea", kEventActionButtonClick);
            payload.addQueryItem("el", "AboutQt");
            break;
        }
        case Type::AddButtonClickEvent:
        {
            payload.addQueryItem("t", "event");
            payload.addQueryItem("ec", kEventCategoryGui);
            payload.addQueryItem("ea", kEventActionButtonClick);
            payload.addQueryItem("el", "Add");
            break;
        }
        case Type::RemoveButtonClickEvent:
        {
            payload.addQueryItem("t", "event");
            payload.addQueryItem("ec", kEventCategoryGui);
            payload.addQueryItem("ea", kEventActionButtonClick);
            payload.addQueryItem("el", "Remove");
            break;
        }
        case Type::ClearButtonClickEvent:
        {
            payload.addQueryItem("t", "event");
            payload.addQueryItem("ec", kEventCategoryGui);
            payload.addQueryItem("ea", kEventActionButtonClick);
            payload.addQueryItem("el", "Clear");
            break;
        }
        }

        payload.addQueryItem("an", qApp->applicationName());
        payload.addQueryItem("av", qApp->applicationVersion());
        payload.addQueryItem("sr", screenSize);
        payload.addQueryItem("sd", screenDepth);
        payload.addQueryItem("ul", language);
        // Google Analytics allows for up to 150 bytes of data for a custom dimention.
        payload.addQueryItem("cd1", osVersion.left(150));
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

    // Google Analytics batch limitations:
    // - A maximum of 20 hits can be specified per request.
    // - The total size of all hit payloads cannot be greater than 16K bytes.
    // - No single hit payload can be greater than 8K bytes.
    // https://developers.google.com/analytics/devguides/collection/protocol/v1/devguide#batch-limitations

    QByteArrayList payloadsCopy = payloads;
    while(!payloadsCopy.isEmpty())
    {
        // Compile a batch of payloads.
        QByteArrayList payloadChunk;
        while(!payloadsCopy.isEmpty() && (payloadChunk.count() < 20))
        {
            // Take a payload from the list.
            const QByteArray payload = payloadsCopy.takeFirst();

            // Discard large payloads.
            if(payload.size() > 8*1024)
            {
                qWarning() << "payload is too big" << payload.size() << payload;
                continue;
            }

            // Calculate size of the batch after current payload will be added to it.
            const int futureSize = payload.size()
                                   + std::accumulate(payloadChunk.constBegin(),
                                                     payloadChunk.constEnd(),
                                                     0,
                                                     [](const int a, const QByteArray &b) { return a + b.size(); });
            // If size of current batch plus new payload is acceptable - add payload.
            if(futureSize <= 16*1024)
            {
                payloadChunk.append(payload);
            }
            // If new payload will make batch larger than allowed - add payload back to initial list and exit the loop.
            else
            {
                payloadsCopy.prepend(payload);
                break;
            }
        }

        if(payloadChunk.isEmpty())
            break;

        QNetworkRequest request(payloadChunk.count() == 1 ? kGaEndpointCollect : kGaEndpointBatch);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        QNetworkReply *reply = mNet.post(request, payloadChunk.join('\n'));
        mReplies.append(reply);
        connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &Analytics::onNetworkReplyError);
        connect(reply, &QNetworkReply::finished, this, &Analytics::onNetworkReplyFinished);
    }
}

void Analytics::onNetworkReplyError(const QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply)
        return;

    qWarning() << "analytics error" << error << reply->errorString();
}

void Analytics::onNetworkReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply)
        return;

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
    mReplies.removeOne(reply);
}
