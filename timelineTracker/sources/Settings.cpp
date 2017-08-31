#include "Settings.h"

struct SettingsData
{
    QString key;
    QVariant defaultValue;

    SettingsData(const QString &k = QString(), const QVariant &d = QVariant())
        : key(k), defaultValue(d)
    {}

    bool isEmpty() const { return key.isEmpty() || defaultValue.isNull(); }
};

static const QMap<Settings::Type, SettingsData> kSettingsTypeMap = {
    { Settings::Type::FirstRun, { "FirstRun", true } }
};

QVariant Settings::get(const Settings::Type type, const QVariant &defaultValue)
{
    const SettingsData &data = kSettingsTypeMap.value(type);
    if(data.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "data unavailable for" << type;
        return defaultValue;
    }
    return QSettings().value(data.key, defaultValue.isNull() ? data.defaultValue : defaultValue);
}

void Settings::set(const Settings::Type type, const QVariant &value)
{
    const SettingsData &data = kSettingsTypeMap.value(type);
    if(data.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "data unavailable for" << type;
        return;
    }
    QSettings().setValue(data.key, value);
}
