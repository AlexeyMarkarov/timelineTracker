#ifndef UTIL_H
#define UTIL_H

#include <QtCore>
#include <QtWidgets>
#include <QtQml>

class Util : public QObject
{
    Q_OBJECT

public:
    static QObject *qmlSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

    Util();
    virtual ~Util();

    // QStyle interface
    Q_INVOKABLE static int pixelMetric(const QStyle::PixelMetric metric);

    Q_INVOKABLE static void copyToClipboard(const QString &string);
    static QString getUserName();
};

#endif // UTIL_H
