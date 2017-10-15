#include "Util.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <lmcons.h>
#endif

QObject *Util::qmlSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new Util;
}

Util::Util()
{
}

Util::~Util()
{
}

int Util::pixelMetric(const QStyle::PixelMetric metric)
{
    return qApp->style()->pixelMetric(metric);
}

void Util::copyToClipboard(const QString &string)
{
    qApp->clipboard()->setText(string);
}

QString Util::getUserName()
{
    QString username;
#ifdef Q_OS_WIN
    {
        TCHAR buf[UNLEN + 1];
        DWORD size = UNLEN + 1;
        const auto result = GetUserName(buf, &size);
        if(result != 0)
        {
#ifdef UNICODE
            username = QString::fromStdWString(buf);
#else
            username = QString(buf);
#endif
        }
        else
        {
            qWarning() << "Windows: unable to get user name:" << result << GetLastError();
        }
    }
#else
#error Util::getUserName() not implemented
#endif
    return username;
}
