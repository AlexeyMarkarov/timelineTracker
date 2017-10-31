#include "Settings.h"
#include <QtGui>

struct SettingsData
{
    QString key;
    QVariant defaultValue;

    SettingsData(const QString &k = QString(), const QVariant &d = QVariant())
        : key(k), defaultValue(d)
    {}

    bool isEmpty() const { return key.isEmpty() || !defaultValue.isValid(); }
};

static const QMap<Settings::Type, SettingsData> kSettingsTypeMap = {
    { Settings::Type::FirstRun,         { "FirstRun",           true } },
    { Settings::Type::WindowVisibility, { "WindowVisibility",   QWindow::AutomaticVisibility } },
    { Settings::Type::WindowPosition,   { "WindowPosition",     QPoint(0, 0) } },
    { Settings::Type::WindowSize,       { "WindowSize",         QSize() } },
    { Settings::Type::AnalyticsEnabled, { "AnalyticsEnabled",   true } },
    { Settings::Type::AnalyticsClientId,{ "AnalyticsClientId",  QString() } }
};

QVariant Settings::get(const Settings::Type type, const QVariant &defaultValue)
{
    const SettingsData &data = kSettingsTypeMap.value(type);
    if(data.isEmpty())
    {
        qDebug() << "data unavailable for" << type << data.key << data.defaultValue;
        return defaultValue;
    }
    return QSettings().value(data.key, defaultValue.isNull() ? data.defaultValue : defaultValue);
}

void Settings::set(const Settings::Type type, const QVariant &value)
{
    const SettingsData &data = kSettingsTypeMap.value(type);
    if(data.isEmpty())
    {
        qDebug() << "data unavailable for" << type << data.key << data.defaultValue;
        return;
    }
    QSettings().setValue(data.key, value);
}

QObject *Settings::qmlSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new Settings;
}
