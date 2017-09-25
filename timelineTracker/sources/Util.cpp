#include "Util.h"

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
