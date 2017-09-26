#include "Logger.h"
#include <iostream>

QFile Logger::sLogFile;

void Logger::init()
{
    const QString logDir = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    if(!QDir(logDir).exists())
    {
        if(!QDir().mkpath(logDir))
        {
            qWarning() << Q_FUNC_INFO << "unable to create directory" << logDir;
        }
    }
    const QString logPath = logDir
                            + (logDir.endsWith(QDir::separator()) ? QString() : QDir::separator())
                            + "log.txt";
    sLogFile.setFileName(logPath);
    sLogFile.open(QFile::WriteOnly | QFile::Truncate);
    if(!sLogFile.isOpen())
    {
        qWarning() << Q_FUNC_INFO << "unable to open log file for writing" << logPath;
    }
    qInstallMessageHandler(&messageHandler);
}

void Logger::release()
{
    if(sLogFile.isOpen())
    {
        sLogFile.close();
    }
}

void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static const QMap<QtMsgType, QString> typeNames = {
        { QtDebugMsg,       "Debug" },
        { QtWarningMsg,     "Warning" },
        { QtCriticalMsg,    "Critical" },
        { QtFatalMsg,       "Fatal" },
        { QtInfoMsg,        "Info" }
    };
    static QMutex mutex;

    const QDateTime dt = QDateTime::currentDateTime();
    const QString finalMsg = QString("[%1-%2-%3 %4:%5:%6.%7 %8 %9 func:%10 line:%11]: %12\n")
                             .arg(dt.date().year())
                             .arg(dt.date().month(), 2, 10, QChar('0'))
                             .arg(dt.date().day(), 2, 10, QChar('0'))
                             .arg(dt.time().hour(), 2, 10, QChar('0'))
                             .arg(dt.time().minute(), 2, 10, QChar('0'))
                             .arg(dt.time().second(), 2, 10, QChar('0'))
                             .arg(dt.time().msec(), 3, 10, QChar('0'))
                             .arg(dt.timeZone().displayName(dt, QTimeZone::OffsetName))
                             .arg(typeNames.value(type, QStringLiteral("Debug")))
                             .arg(context.function)
                             .arg(context.line)
                             .arg(msg)
                             ;

    const QMutexLocker locker(&mutex);
    std::cerr << finalMsg.toStdString();
    if(sLogFile.isOpen())
    {
        sLogFile.write(finalMsg.toUtf8());
        sLogFile.flush();
    }
}
