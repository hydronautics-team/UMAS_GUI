#include "video_player_widget.h"
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QDebug>

VideoPlayerWidget::VideoPlayerWidget(QWidget *parent)
    : QWidget(parent)
{
    static bool gstInit = false;
    if (!gstInit) {
        gst_init(nullptr, nullptr);
        gstInit = true;
    }

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    titleLabel_ = new QLabel("Video Player", this);
    titleLabel_->setAlignment(Qt::AlignCenter);
    titleLabel_->setStyleSheet("font-weight: bold; font-size: 14px; color: white;");

    imageLabel_ = new QLabel(this);
    imageLabel_->setAlignment(Qt::AlignCenter);
    imageLabel_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imageLabel_->setMinimumSize(160, 120);
    imageLabel_->setStyleSheet("background-color: #1a1a1a; border: 1px solid #444;");

    statusLabel_ = new QLabel("Ожидание потока GStreamer...", this);
    statusLabel_->setAlignment(Qt::AlignCenter);
    statusLabel_->setStyleSheet("color: #aaa; font-size: 10px;");

    layout->addWidget(titleLabel_);
    layout->addWidget(imageLabel_, 1);
    layout->addWidget(statusLabel_);

    connect(this, &VideoPlayerWidget::frameReceived,
            this, [this](const QPixmap &pix) {
                if (!pix.isNull()) {
                    imageLabel_->setPixmap(pix.scaled(
                        imageLabel_->size(),
                        Qt::KeepAspectRatio,
                        Qt::SmoothTransformation
                    ));
                    currentPixmap_ = pix;
                }
            });

    qDebug() << "VideoPlayerWidget (GStreamer) создан";
}

VideoPlayerWidget::~VideoPlayerWidget()
{
    stopStream();
}

void VideoPlayerWidget::startStream(const QString &portStr)
{
    if (gstPipeline_) {
        qDebug() << "Поток уже запущен";
        return;
    }

    QString pipeline = QString(
        "udpsrc port=%1 caps=\"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)JPEG\" ! "
        "rtpjpegdepay ! jpegdec ! videoconvert ! video/x-raw, format=RGB ! "
        "appsink name=my_sink emit-signals=true sync=false"
    ).arg(portStr);

    GError *error = nullptr;
    gstPipeline_ = gst_parse_launch(pipeline.toUtf8().constData(), &error);

    if (error) {
        qCritical() << "GStreamer error:" << error->message;
        g_error_free(error);
        statusLabel_->setText("Ошибка запуска GStreamer");
        return;
    }

    GstElement *sink = gst_bin_get_by_name(GST_BIN(gstPipeline_), "my_sink");
    if (sink) {
        GstAppSinkCallbacks callbacks = {nullptr, nullptr, &VideoPlayerWidget::newSampleCallback, nullptr};
        gst_app_sink_set_callbacks(GST_APP_SINK(sink), &callbacks, this, nullptr);
        gst_object_unref(sink);
    } else {
        statusLabel_->setText("Ошибка: appsink не найден");
        return;
    }

    gst_element_set_state(gstPipeline_, GST_STATE_PLAYING);
    statusLabel_->setText(QString("Поток активен (Порт: %1)").arg(portStr));
    qDebug() << "Видео-поток запущен на порту" << portStr;
}

void VideoPlayerWidget::stopStream()
{
    if (gstPipeline_) {
        gst_element_set_state(gstPipeline_, GST_STATE_NULL);
        gst_object_unref(gstPipeline_);
        gstPipeline_ = nullptr;
        statusLabel_->setText("Поток остановлен");
        qDebug() << "Видео-поток остановлен";
    }
}

GstFlowReturn VideoPlayerWidget::newSampleCallback(GstAppSink *appsink, gpointer userData)
{
    GstSample *sample = gst_app_sink_pull_sample(appsink);
    if (!sample) return GST_FLOW_OK;

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstCaps *caps = gst_sample_get_caps(sample);
    GstStructure *structure = gst_caps_get_structure(caps, 0);

    int width = 0, height = 0;
    gst_structure_get_int(structure, "width", &width);
    gst_structure_get_int(structure, "height", &height);

    GstMapInfo map;
    if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
        QImage img(map.data, width, height, width * 3, QImage::Format_RGB888);
        QPixmap pix = QPixmap::fromImage(img.copy());

        auto *self = static_cast<VideoPlayerWidget*>(userData);
        QMetaObject::invokeMethod(self, "frameReceived", Qt::QueuedConnection, Q_ARG(QPixmap, pix));

        gst_buffer_unmap(buffer, &map);
    }

    gst_sample_unref(sample);
    return GST_FLOW_OK;
}

void VideoPlayerWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!currentPixmap_.isNull()) {
        imageLabel_->setPixmap(currentPixmap_.scaled(
            imageLabel_->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        ));
    }
}