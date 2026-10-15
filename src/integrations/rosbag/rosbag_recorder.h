#pragma once

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>

class RosbagRecorder : public QObject
{
    Q_OBJECT

public:
    explicit RosbagRecorder(QObject *parent = nullptr);
    ~RosbagRecorder() override;

    bool isRecording() const;
    QString outputPath() const;

public slots:
    void startRecording(const QString &outputRoot, const QStringList &topics);
    void stopRecording();

signals:
    void recordingStarted(const QString &outputPath);
    void recordingStopped(const QString &outputPath);
    void recordingError(const QString &message);

private:
    void forceStopIfNeeded();
    QString processErrorText();

    QProcess process_;
    QString outputPath_;
    bool stopRequested_ = false;
};
