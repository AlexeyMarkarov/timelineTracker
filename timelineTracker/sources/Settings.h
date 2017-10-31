#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore>
#include <QtQml>

class Settings : public QObject
{
    Q_OBJECT

public:
    enum class Type
    {
        FirstRun,
        WindowVisibility,
        WindowPosition,
        WindowSize,
        AnalyticsEnabled,
        AnalyticsClientId
    };
    Q_ENUM(Type)

    static QObject *qmlSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

    Q_INVOKABLE static QVariant get(const Type type, const QVariant &defaultValue = QVariant());
    Q_INVOKABLE static void set(const Type type, const QVariant &value);

    Settings() = default;
    virtual ~Settings() = default;
};

#endif // SETTINGS_H
