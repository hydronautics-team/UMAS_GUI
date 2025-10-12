#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProcess>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class Videowidget; }
QT_END_NAMESPACE

class Videowidget : public QWidget
{
    Q_OBJECT

public:
    explicit Videowidget(QWidget *parent = nullptr);
    ~Videowidget();

private slots:
    void togglePipeline();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void startPipelineNow(); // реальное создание/запуск пайплайна

    Ui::Videowidget *ui;
    GstElement *pipeline;
    bool pendingStart;   // ждем winId
};

#endif // VIDEOWIDGET_H
