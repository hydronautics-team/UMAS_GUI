#pragma once

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageLogContext>
#include <QMessageLogger>
#include <QMutex>
#include <QMutexLocker>
#include <QTextStream>
#include <QThread>
#include <QUuid>

#include <cstdio>

#define UMAS_LOG_DEBUG() QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "umas").debug()
#define UMAS_LOG_INFO() QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "umas").info()
#define UMAS_LOG_WARNING() QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "umas").warning()
#define UMAS_LOG_ERROR() QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "umas").critical()

class ApplicationLogger final
{
public:
    static bool initialize(const QString &directory = QStringLiteral("logs"))
    {
        QMutexLocker locker(&mutex_);
        if (initialized_)
            return true;

        logDirectory_ = QDir(directory).absolutePath();
        QDir logDirectory(logDirectory_);
        if (!logDirectory.mkpath(QStringLiteral(".")))
        {
            std::fprintf(stderr, "UMAS GUI: cannot create log directory: %s\n",
                         qPrintable(logDirectory_));
            return false;
        }

        removeExpiredLogs(logDirectory);
        sessionId_ = QUuid::createUuid().toString(QUuid::WithoutBraces);
        const QDateTime startedAt = QDateTime::currentDateTime();
        logFileBaseName_ =
            QStringLiteral("umas-gui_%1_%2")
                .arg(startedAt.toString(QStringLiteral("yyyy-MM-dd_HH-mm-ss-zzz")),
                     sessionId_.left(8));
        logFilePart_ = 1;
        if (!openLogFile(QStringLiteral("SESSION_START")))
            return false;

        initialized_ = true;
        previousHandler_ = qInstallMessageHandler(messageHandler);
        return true;
    }

    static void shutdown()
    {
        qInstallMessageHandler(previousHandler_);
        QMutexLocker locker(&mutex_);
        if (!initialized_)
            return;

        writeSessionBoundary(QStringLiteral("SESSION_END"));
        logFile_.flush();
        logFile_.close();
        initialized_ = false;
    }

    static QString logDirectory()
    {
        QMutexLocker locker(&mutex_);
        return logDirectory_;
    }

private:
    static constexpr qint64 MaxLogFileSize = 25LL * 1024LL * 1024LL;
    static constexpr int RetentionDays = 30;

    static const char *levelName(QtMsgType type)
    {
        switch (type)
        {
        case QtDebugMsg:
            return "DEBUG";
        case QtInfoMsg:
            return "INFO";
        case QtWarningMsg:
            return "WARN";
        case QtCriticalMsg:
            return "ERROR";
        case QtFatalMsg:
            return "FATAL";
        }
        return "UNKNOWN";
    }

    static QString sanitize(QString value)
    {
        value.replace(QLatin1Char('\\'), QStringLiteral("\\\\"));
        value.replace(QLatin1Char('\n'), QStringLiteral("\\n"));
        value.replace(QLatin1Char('\r'), QStringLiteral("\\r"));
        value.replace(QLatin1Char('\t'), QStringLiteral("\\t"));
        return value;
    }

    static bool openLogFile(const QString &boundaryEvent)
    {
        if (logFile_.isOpen())
            logFile_.close();

        const QString partSuffix =
            logFilePart_ == 1 ? QString() : QStringLiteral("_part-%1").arg(logFilePart_);
        const QString filePath = QDir(logDirectory_)
                                     .filePath(logFileBaseName_ + partSuffix + QStringLiteral(".log"));

        logFile_.setFileName(filePath);
        if (!logFile_.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            std::fprintf(stderr, "UMAS GUI: cannot open log file: %s\n", qPrintable(filePath));
            return false;
        }

        writeSessionBoundary(boundaryEvent);
        return true;
    }

    static void writeSessionBoundary(const QString &event)
    {
        if (!logFile_.isOpen())
            return;

        QTextStream stream(&logFile_);
        stream << QDateTime::currentDateTime().toString(Qt::ISODateWithMs) << " | " << event
               << " | session=" << sessionId_ << " | pid=" << QCoreApplication::applicationPid()
               << " | app=" << QCoreApplication::applicationName()
               << " | version=" << QCoreApplication::applicationVersion() << '\n';
        stream.flush();
    }

    static void rotateIfNeeded()
    {
        if (logFile_.size() >= MaxLogFileSize)
        {
            ++logFilePart_;
            openLogFile(QStringLiteral("SESSION_CONTINUE"));
        }
    }

    static void removeExpiredLogs(const QDir &directory)
    {
        const QDateTime cutoff = QDateTime::currentDateTime().addDays(-RetentionDays);
        const QFileInfoList files = directory.entryInfoList(
            {QStringLiteral("umas-gui_*.log")}, QDir::Files | QDir::NoSymLinks, QDir::Time);
        for (const QFileInfo &file : files)
        {
            if (file.lastModified() < cutoff)
                QFile::remove(file.absoluteFilePath());
        }
    }

    static void messageHandler(QtMsgType type, const QMessageLogContext &context,
                               const QString &message)
    {
        const QDateTime now = QDateTime::currentDateTime();
        const QString timestamp = now.toString(Qt::ISODateWithMs);
        const QString category = context.category ? QString::fromUtf8(context.category)
                                                   : QStringLiteral("default");
        const QString sourceFile = context.file ? QString::fromUtf8(context.file)
                                                : QStringLiteral("unknown");
        const QString function = context.function ? QString::fromUtf8(context.function)
                                                   : QStringLiteral("unknown");
        const quintptr threadId = reinterpret_cast<quintptr>(QThread::currentThreadId());
        const QString line =
            QStringLiteral("%1 | %2 | session=%3 | pid=%4 | thread=0x%5 | category=%6 | "
                           "source=%7:%8 | function=%9 | message=%10")
                .arg(timestamp)
                .arg(QString::fromLatin1(levelName(type)), -5)
                .arg(sessionId_)
                .arg(QCoreApplication::applicationPid())
                .arg(static_cast<qulonglong>(threadId), 0, 16)
                .arg(sanitize(category), sanitize(sourceFile))
                .arg(context.line)
                .arg(sanitize(function), sanitize(message));

        {
            QMutexLocker locker(&mutex_);
            if (initialized_)
            {
                rotateIfNeeded();
                if (logFile_.isOpen())
                {
                    QTextStream stream(&logFile_);
                    stream << line << '\n';
                    stream.flush();
                }
            }
        }

        std::fprintf(stderr, "%s\n", qPrintable(line));
        std::fflush(stderr);
    }

    inline static QMutex mutex_;
    inline static QFile logFile_;
    inline static QString logDirectory_;
    inline static QString sessionId_;
    inline static QString logFileBaseName_;
    inline static int logFilePart_ = 1;
    inline static QtMessageHandler previousHandler_ = nullptr;
    inline static bool initialized_ = false;
};

