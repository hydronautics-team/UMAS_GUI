#include "videowidget.h"
#include "ui_videowidget.h"

#include <QTimer>
#include <QWindow>
#include <QEvent>#include <QGuiApplication>
#include <QDebug>


Videowidget::Videowidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::Videowidget),
      pipeline(nullptr),
      pendingStart(false)
{
    ui->setupUi(this);

    gst_init(nullptr, nullptr);

    qDebug() << "Platform:" << QGuiApplication::platformName();

    connect(ui->pushButton_Play_Pause, &QPushButton::clicked,
            this, &Videowidget::togglePipeline);

    // появление winId
    ui->videoWidget->installEventFilter(this);

    ui->pushButton_Play_Pause->setText("Начать прием");
}

Videowidget::~Videowidget()
{
    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        pipeline = nullptr;
    }
    delete ui;
}

bool Videowidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->videoWidget) {
        // QEvent::WinIdChange сработает, когда Qt создаст/изменит winId
        if (event->type() == QEvent::WinIdChange) {
            qDebug() << "videoWidget WinIdChange event, winId() =" << ui->videoWidget->winId();
            if (pendingStart) {
                // как только winId появился — запускаем пайплайн
                pendingStart = false;
                startPipelineNow();
            }
        }
        // также можно ловить Show если нужно:
        if (event->type() == QEvent::Show) {
            qDebug() << "videoWidget Show event, winId() =" << ui->videoWidget->winId();
            if (pendingStart) {
                pendingStart = false;
                startPipelineNow();
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void Videowidget::togglePipeline()
{
    if (!pipeline) {
        // Попробуем принудительно создать winId и windowHandle
        ui->videoWidget->createWinId(); // запрос на создание native window
        QWindow *w = ui->videoWidget->windowHandle();
        if (!w) {
            // окна ещё нет — пометим и дождёмся события WinIdChange (eventFilter)
            qDebug() << "No windowHandle yet — deferring pipeline start";
            pendingStart = true;
            return;
        }

        // если windowHandle уже есть — запускаем здесь
        startPipelineNow();
    } else {
        // Останов пайплайна
        qDebug() << "Stopping pipeline...";
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        pipeline = nullptr;
        ui->pushButton_Play_Pause->setText("Начать прием");
    }
}

void Videowidget::startPipelineNow()
{
    // Здесь гарантированно есть windowHandle / winId
    QWindow *window = ui->videoWidget->windowHandle();
    if (!window) {
        qWarning() << "startPipelineNow: still no windowHandle!";
        return;
    }

    WId wid = window->winId();
    qDebug() << "startPipelineNow: using window id =" << wid;

    // Отправка (ssh) — если нужно
    QString command =
        "ssh -X agent1@192.168.1.11 "
        "\"gst-launch-1.0 v4l2src device=/dev/video0 ! "
        "image/jpeg,framerate=30/1 ! "
        "jpegparse ! rtpjpegpay ! udpsink host=192.168.1.111 port=5000\"";

    QProcess *sshProcess = new QProcess(this);
    sshProcess->start(command);

    // Создаём пайплайн
    const char *pipeline_str =
        "udpsrc port=5000 ! application/x-rtp,encoding-name=JPEG,payload=96 ! "
        "rtpjpegdepay ! jpegdec ! videoconvert ! xvimagesink name=sink";

    GError *error = nullptr;
    pipeline = gst_parse_launch(pipeline_str, &error);
    if (error) {
        qWarning() << "Failed to create pipeline:" << error->message;
        g_error_free(error);
        pipeline = nullptr;
        return;
    }

    GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
    if (!sink) {
        qWarning() << "Could not find sink element";
        gst_object_unref(pipeline);
        pipeline = nullptr;
        return;
    }

    if (GST_IS_VIDEO_OVERLAY(sink)) {
        gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(sink), (guintptr)wid);
    } else {
        qWarning() << "Sink is not video overlay capable!";
    }
    gst_object_unref(sink);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    ui->pushButton_Play_Pause->setText("Остановить прием");
}
