#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore>

class Settings
{
    Q_GADGET

public:
    enum class Type
    {
        FirstRun
    };
    Q_ENUM(Type)

    static QVariant get(const Type type, const QVariant &defaultValue = QVariant());
    static void set(const Type type, const QVariant &value);

private:
    Settings();
    virtual ~Settings();
};

#endif // SETTINGS_H
