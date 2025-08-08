// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pipeline = nullptr; // Инициализируем пайплайн как неактивный

    // Инициализация GStreamer
    gst_init(nullptr, nullptr);

    // Получаем "внутренний" идентификатор нашего виджета для видео
    videoWindowId = ui->videoWidget->winId();

    ui->pushButton_Play_Pause->setText("Начать прием");
}

MainWindow::~MainWindow()
{
    // Корректно останавливаем и освобождать ресурсы при закрытии
    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        pipeline = nullptr;
    }
    delete ui;
}

void MainWindow::on_pushButton_Play_Pause_clicked()
{
    // Отправка видео с Raspberry Pi
    QString command = "ssh -X agent1@192.168.1.11 "
                      "\"gst-launch-1.0 v4l2src device=/dev/video0 ! "
                      "image/jpeg,width=100,height=100,framerate=30/1 ! "
                      "jpegparse ! rtpjpegpay ! udpsink host=192.168.1.6 port=5000\"";

    QProcess *sshProcess = new QProcess(this);
    sshProcess->start(command);

    if (!pipeline) { // Если пайплайн не запущен, запускаем его
        qDebug() << "Starting pipeline...";

        // Приём видео на ПК через xvimagesink
        const char *pipeline_str =
            "udpsrc port=5000 ! application/x-rtp,encoding-name=JPEG,payload=96 ! "
            "rtpjpegdepay ! jpegdec ! videoconvert ! xvimagesink name=sink";

        GError *error = nullptr;
        pipeline = gst_parse_launch(pipeline_str, &error);

        if (error) {
            qWarning() << "Failed to create pipeline:" << error->message;
            g_error_free(error);
            return;
        }

        // Находим элемент sink и привязываем к нашему QWidget
        GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
        if (!sink) {
            qWarning() << "Could not find sink element";
            gst_object_unref(pipeline);
            pipeline = nullptr;
            return;
        }

        if (GST_IS_VIDEO_OVERLAY(sink)) {
            gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(sink), videoWindowId);
        } else {
            qWarning() << "Sink is not video overlay capable!";
        }
        gst_object_unref(sink);

        // Запускаем пайплайн
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        ui->pushButton_Play_Pause->setText("Остановить прием");

    } else { // Если пайплайн уже работает, останавливаем его
        qDebug() << "Stopping pipeline...";
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        pipeline = nullptr;
        ui->pushButton_Play_Pause->setText("Начать прием");
    }
}
