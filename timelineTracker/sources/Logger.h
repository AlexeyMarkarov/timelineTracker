#ifndef LOGGER_H
#define LOGGER_H

#include <QtCore>

class Logger
{
public:
    static void init();
    static void release();

    static QString getLogsDir();

private:
    Logger();
    ~Logger();

    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    static QFile sLogFile;
};

#endif // LOGGER_H
