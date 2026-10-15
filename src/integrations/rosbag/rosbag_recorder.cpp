#include "rosbag_recorder.h"

#include <QDateTime>
#include <QDir>
#include <QTimer>

#ifdef Q_OS_UNIX
#include <csignal>
#include <sys/types.h>
#endif

RosbagRecorder::RosbagRecorder(QObject *parent) : QObject(parent)
{
    process_.setProcessChannelMode(QProcess::SeparateChannels);

    connect(&process_, &QProcess::started, this, [this]() { emit recordingStarted(outputPath_); });

    connect(&process_, &QProcess::errorOccurred, this,
            [this](QProcess::ProcessError error)
            {
                if (error == QProcess::FailedToStart)
                {
                    emit recordingError(QStringLiteral("Не удалось запустить ros2 bag: %1")
                                            .arg(process_.errorString()));
                }
            });

    connect(&process_, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [this](int exitCode, QProcess::ExitStatus exitStatus)
            {
                const QString completedPath = outputPath_;
                const bool expectedStop = stopRequested_;
                stopRequested_ = false;

                if (!expectedStop && (exitStatus == QProcess::CrashExit || exitCode != 0))
                {
                    emit recordingError(QStringLiteral("ros2 bag завершился с кодом %1: %2")
                                            .arg(exitCode)
                                            .arg(processErrorText()));
                }

                emit recordingStopped(completedPath);
            });
}

RosbagRecorder::~RosbagRecorder()
{
    if (process_.state() == QProcess::NotRunning)
        return;

    stopRecording();
    if (!process_.waitForFinished(3000))
    {
        process_.kill();
        process_.waitForFinished(1000);
    }
}

bool RosbagRecorder::isRecording() const { return process_.state() != QProcess::NotRunning; }

QString RosbagRecorder::outputPath() const { return outputPath_; }

void RosbagRecorder::startRecording(const QString &outputRoot, const QStringList &topics)
{
    if (isRecording())
    {
        emit recordingError(QStringLiteral("Запись rosbag уже запущена"));
        return;
    }
    if (topics.isEmpty())
    {
        emit recordingError(QStringLiteral("Список топиков для записи пуст"));
        return;
    }

    QDir root(outputRoot);
    if (!root.mkpath(QStringLiteral(".")))
    {
        emit recordingError(
            QStringLiteral("Не удалось создать каталог для rosbag: %1").arg(outputRoot));
        return;
    }

    const QString sessionName =
        QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd_HH-mm-ss-zzz"));
    outputPath_ = root.filePath(sessionName);
    stopRequested_ = false;

    QStringList arguments = {QStringLiteral("bag"), QStringLiteral("record"),
                             QStringLiteral("--output"), outputPath_};
    arguments.append(topics);

    process_.setProgram(QStringLiteral("ros2"));
    process_.setArguments(arguments);
    process_.start();
}

void RosbagRecorder::stopRecording()
{
    if (process_.state() == QProcess::NotRunning)
        return;

    stopRequested_ = true;

#ifdef Q_OS_UNIX
    const qint64 pid = process_.processId();
    if (pid > 0)
        ::kill(static_cast<pid_t>(pid), SIGINT);
    else
        process_.terminate();
#else
    process_.terminate();
#endif

    QTimer::singleShot(5000, this, &RosbagRecorder::forceStopIfNeeded);
}

void RosbagRecorder::forceStopIfNeeded()
{
    if (process_.state() == QProcess::NotRunning)
        return;

    process_.terminate();
    QTimer::singleShot(2000, this,
                       [this]()
                       {
                           if (process_.state() != QProcess::NotRunning)
                               process_.kill();
                       });
}

QString RosbagRecorder::processErrorText()
{
    const QString standardError = QString::fromLocal8Bit(process_.readAllStandardError()).trimmed();
    return standardError.isEmpty() ? process_.errorString() : standardError;
}
