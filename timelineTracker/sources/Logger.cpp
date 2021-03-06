#include "Logger.h"
#include <iostream>

QFile Logger::sLogFile;

void Logger::init()
{
    const QString logPath = getLogFilePath();
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
        { QtDebugMsg,       QStringLiteral("Debug") },
        { QtWarningMsg,     QStringLiteral("Warning") },
        { QtCriticalMsg,    QStringLiteral("Critical") },
        { QtFatalMsg,       QStringLiteral("Fatal") },
        { QtInfoMsg,        QStringLiteral("Info") }
    };
    static QMutex mutex;

    const QDateTime dt = QDateTime::currentDateTime();
    const QString finalMsg = QStringLiteral("[%1 %2 %3] %4\r\n")
                             .arg(dt.toOffsetFromUtc(dt.offsetFromUtc()).toString(Qt::ISODateWithMs))
                             .arg(typeNames.value(type, QStringLiteral("Debug")))
                             .arg(context.function)
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

QString Logger::getLogsDir()
{
    return sLogFile.isOpen()
            ? QFileInfo(sLogFile.fileName()).absolutePath()
            : QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
}

QString Logger::getLogFilePath()
{
    if(sLogFile.isOpen())
    {
        return sLogFile.fileName();
    }
    else
    {
        const QString logDir = getLogsDir();
        if(!QDir(logDir).exists())
        {
            if(!QDir().mkpath(logDir))
            {
                qWarning() << Q_FUNC_INFO << "unable to create directory" << logDir;
            }
        }
        return QDir::cleanPath(logDir + QDir::separator() + "log.txt");
    }
}
