#include "video_player_widget.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QIcon>
#include <QPainter>
#include <QPushButton>
#include <QResizeEvent>
#include <QVBoxLayout>

namespace
{
QIcon makeCameraActionIcon(int kind)
{
    QPixmap pixmap(24, 24);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(
        QPen(QColor(QStringLiteral("#55e9ed")), 1.7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);

    if (kind == 0 || kind == 2)
    {
        painter.drawRoundedRect(QRectF(3.0, 6.5, 18.0, 13.5), 2.0, 2.0);
        painter.drawEllipse(QPointF(12.0, 13.2), 4.0, 4.0);
        painter.drawLine(QPointF(7.0, 6.5), QPointF(9.0, 3.8));
        painter.drawLine(QPointF(9.0, 3.8), QPointF(14.5, 3.8));
        if (kind == 2)
        {
            painter.drawLine(QPointF(18.0, 4.0), QPointF(21.0, 4.0));
            painter.drawLine(QPointF(19.5, 2.5), QPointF(19.5, 5.5));
        }
    }
    else if (kind == 1)
    {
        painter.drawEllipse(QPointF(12.0, 12.0), 7.2, 7.2);
        painter.setBrush(QColor(QStringLiteral("#55e9ed")));
        painter.drawEllipse(QPointF(12.0, 12.0), 3.2, 3.2);
    }
    else
    {
        painter.drawLine(QPointF(3.5, 9.0), QPointF(3.5, 3.5));
        painter.drawLine(QPointF(3.5, 3.5), QPointF(9.0, 3.5));
        painter.drawLine(QPointF(15.0, 3.5), QPointF(20.5, 3.5));
        painter.drawLine(QPointF(20.5, 3.5), QPointF(20.5, 9.0));
        painter.drawLine(QPointF(20.5, 15.0), QPointF(20.5, 20.5));
        painter.drawLine(QPointF(20.5, 20.5), QPointF(15.0, 20.5));
        painter.drawLine(QPointF(9.0, 20.5), QPointF(3.5, 20.5));
        painter.drawLine(QPointF(3.5, 20.5), QPointF(3.5, 15.0));
    }
    return QIcon(pixmap);
}
} // namespace

class VideoHudOverlay final : public QWidget
{
public:
    explicit VideoHudOverlay(QWidget *parent = nullptr) : QWidget(parent)
    {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_TranslucentBackground);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        const QColor hud(202, 255, 255, 205);
        const QPointF center = rect().center();
        painter.setPen(QPen(hud, 1.2));
        painter.drawLine(center + QPointF(-17, 0), center + QPointF(17, 0));
        painter.drawLine(center + QPointF(0, -17), center + QPointF(0, 17));

        const qreal leftX = width() * 0.055;
        const qreal top = height() * 0.25;
        const qreal bottom = height() * 0.74;
        painter.setFont(QFont(QStringLiteral("IBM Plex Mono"), 8));
        painter.drawLine(QPointF(leftX, top), QPointF(leftX, bottom));
        const QStringList depths = {QStringLiteral("-110"), QStringLiteral("-120"),
                                    QStringLiteral("-130"), QStringLiteral("-140")};
        for (int index = 0; index < depths.size(); ++index)
        {
            const qreal y = top + (bottom - top) * index / (depths.size() - 1.0);
            painter.drawLine(QPointF(leftX, y), QPointF(leftX + 8, y));
            painter.drawText(QRectF(leftX + 12, y - 8, 45, 16), Qt::AlignLeft | Qt::AlignVCenter,
                             depths.at(index));
        }

        const qreal rightX = width() * 0.94;
        painter.drawLine(QPointF(rightX, top), QPointF(rightX, bottom));
        const QStringList angles = {QStringLiteral("+30"), QStringLiteral("0"),
                                    QStringLiteral("-30")};
        for (int index = 0; index < angles.size(); ++index)
        {
            const qreal y = top + (bottom - top) * index / (angles.size() - 1.0);
            painter.drawLine(QPointF(rightX - 8, y), QPointF(rightX, y));
            painter.drawText(QRectF(rightX - 42, y - 8, 30, 16), Qt::AlignRight | Qt::AlignVCenter,
                             angles.at(index));
        }
    }
};

VideoPlayerWidget::VideoPlayerWidget(QWidget *parent) : QWidget(parent)
{
    static bool gstInit = false;
    if (!gstInit)
    {
        gst_init(nullptr, nullptr);
        gstInit = true;
    }

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    toolbar_ = new QWidget(this);
    toolbar_->setObjectName(QStringLiteral("cameraToolbar"));
    toolbar_->setFixedHeight(39);
    auto *toolbarLayout = new QHBoxLayout(toolbar_);
    toolbarLayout->setContentsMargins(10, 4, 6, 4);
    toolbarLayout->setSpacing(8);

    titleLabel_ = new QLabel(QStringLiteral("◉  КАМЕРА 1"), toolbar_);
    titleLabel_->setObjectName(QStringLiteral("cameraTitle"));
    auto *cameraType = new QLabel(QStringLiteral("Основная"), toolbar_);
    cameraType->setObjectName(QStringLiteral("cameraType"));
    liveDot_ = new QLabel(QStringLiteral("●"), toolbar_);
    liveDot_->setObjectName(QStringLiteral("cameraLiveDot"));
    liveLabel_ = new QLabel(QStringLiteral("LIVE"), toolbar_);
    liveLabel_->setObjectName(QStringLiteral("cameraLiveLabel"));
    resolutionLabel_ = new QLabel(QStringLiteral("—"), toolbar_);
    resolutionLabel_->setProperty("streamBadge", true);
    fpsLabel_ = new QLabel(QStringLiteral("— FPS"), toolbar_);
    fpsLabel_->setProperty("streamBadge", true);
    auto *bitrate = new QLabel(QStringLiteral("Поток: —"), toolbar_);
    bitrate->setObjectName(QStringLiteral("cameraBitrate"));

    liveDot_->hide();
    liveLabel_->hide();
    toolbarLayout->addWidget(titleLabel_);
    toolbarLayout->addWidget(cameraType);
    toolbarLayout->addSpacing(10);
    toolbarLayout->addWidget(liveDot_);
    toolbarLayout->addWidget(liveLabel_);
    toolbarLayout->addStretch();
    toolbarLayout->addWidget(resolutionLabel_);
    toolbarLayout->addWidget(fpsLabel_);
    toolbarLayout->addWidget(bitrate);

    auto makeAction =
        [this, toolbarLayout](int iconKind, const QString &tooltip, bool enabled, int width = 40)
    {
        auto *button = new QPushButton(toolbar_);
        button->setProperty("cameraAction", true);
        button->setIcon(makeCameraActionIcon(iconKind));
        button->setIconSize(QSize(22, 22));
        button->setToolTip(tooltip);
        button->setEnabled(enabled);
        button->setFixedSize(width, 31);
        toolbarLayout->addWidget(button);
        return button;
    };
    makeAction(0, QStringLiteral("Снимок пока не подключён"), false);
    makeAction(1, QStringLiteral("Запись пока не подключена"), false);
    makeAction(2, QStringLiteral("Переключение камеры пока не подключено"), false);
    auto *fullscreen = makeAction(3, QStringLiteral("Полноэкранный режим"), true, 35);
    connect(fullscreen, &QPushButton::clicked, this, &VideoPlayerWidget::fullscreenRequested);

    imageLabel_ = new QLabel(this);
    imageLabel_->setAlignment(Qt::AlignCenter);
    imageLabel_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imageLabel_->setMinimumSize(160, 120);
    imageLabel_->setStyleSheet("background-color: #00111d; border: 1px solid #07516b;");

    statusLabel_ = new QLabel("Ожидание потока GStreamer...", this);
    statusLabel_->setAlignment(Qt::AlignCenter);
    statusLabel_->setStyleSheet(
        "color: #668a9b; font-size: 11px; background: rgba(0, 12, 23, 180); padding: 5px;");

    hudOverlay_ = new VideoHudOverlay(imageLabel_);
    thumbnailStrip_ = new QWidget(imageLabel_);
    thumbnailStrip_->setObjectName(QStringLiteral("cameraThumbnailStrip"));
    auto *thumbnailLayout = new QHBoxLayout(thumbnailStrip_);
    thumbnailLayout->setContentsMargins(0, 0, 0, 0);
    thumbnailLayout->setSpacing(10);
    for (int camera = 1; camera <= 4; ++camera)
    {
        auto *thumbnail = new QPushButton(QStringLiteral("Камера %1").arg(camera), thumbnailStrip_);
        thumbnail->setProperty("cameraThumbnail", true);
        thumbnail->setProperty("activeCamera", camera == 1);
        thumbnail->setEnabled(camera == 1);
        thumbnail->setToolTip(camera == 1 ? QStringLiteral("Активная камера")
                                          : QStringLiteral("Источник не настроен"));
        thumbnail->setFixedSize(138, 78);
        thumbnailLayout->addWidget(thumbnail);
    }

    layout->addWidget(toolbar_);
    layout->addWidget(imageLabel_, 1);
    statusLabel_->setParent(imageLabel_);

    connect(this, &VideoPlayerWidget::frameReceived, this,
            [this](const QPixmap &pix)
            {
                if (!pix.isNull())
                {
                    imageLabel_->setPixmap(pix.scaled(imageLabel_->size(),
                                                      Qt::KeepAspectRatioByExpanding,
                                                      Qt::SmoothTransformation));
                    currentPixmap_ = pix;
                    statusLabel_->hide();
                    liveDot_->show();
                    liveLabel_->show();
                    resolutionLabel_->setText(
                        QStringLiteral("%1×%2").arg(pix.width()).arg(pix.height()));
                    ++frameCount_;
                    if (!frameTimer_.isValid())
                        frameTimer_.start();
                    else if (frameTimer_.elapsed() >= 1000)
                    {
                        const int fps = qRound(frameCount_ * 1000.0 / frameTimer_.elapsed());
                        fpsLabel_->setText(QStringLiteral("%1 FPS").arg(fps));
                        frameCount_ = 0;
                        frameTimer_.restart();
                    }
                }
            });

    qDebug() << "VideoPlayerWidget (GStreamer) создан";
}

VideoPlayerWidget::~VideoPlayerWidget() { stopStream(); }

void VideoPlayerWidget::setFullscreenPresentation(bool fullscreen)
{
    toolbar_->setVisible(!fullscreen);
    thumbnailStrip_->setVisible(!fullscreen);
    statusLabel_->setVisible(!fullscreen && currentPixmap_.isNull());
    imageLabel_->setStyleSheet(fullscreen ? "background-color: black; border: none;"
                                          : "background-color: #1a1a1a; border: 1px solid #444;");
}

void VideoPlayerWidget::startStream(const QString &portStr)
{
    if (gstPipeline_)
    {
        qDebug() << "Поток уже запущен";
        return;
    }

    QString pipeline = QString("udpsrc port=%1 caps=\"application/x-rtp, media=(string)video, "
                               "clock-rate=(int)90000, encoding-name=(string)JPEG\" ! "
                               "rtpjpegdepay ! jpegdec ! videoconvert ! video/x-raw, format=RGB ! "
                               "appsink name=my_sink emit-signals=true sync=false")
                           .arg(portStr);

    GError *error = nullptr;
    gstPipeline_ = gst_parse_launch(pipeline.toUtf8().constData(), &error);

    if (error)
    {
        qCritical() << "GStreamer error:" << error->message;
        g_error_free(error);
        statusLabel_->setText("Ошибка запуска GStreamer");
        return;
    }

    GstElement *sink = gst_bin_get_by_name(GST_BIN(gstPipeline_), "my_sink");
    if (sink)
    {
        GstAppSinkCallbacks callbacks = {nullptr, nullptr, &VideoPlayerWidget::newSampleCallback,
                                         nullptr};
        gst_app_sink_set_callbacks(GST_APP_SINK(sink), &callbacks, this, nullptr);
        gst_object_unref(sink);
    }
    else
    {
        statusLabel_->setText("Ошибка: appsink не найден");
        return;
    }

    gst_element_set_state(gstPipeline_, GST_STATE_PLAYING);
    statusLabel_->setText(QString("Ожидание кадров (порт %1)").arg(portStr));
    qDebug() << "Видео-поток запущен на порту" << portStr;
}

void VideoPlayerWidget::stopStream()
{
    if (gstPipeline_)
    {
        gst_element_set_state(gstPipeline_, GST_STATE_NULL);
        gst_object_unref(gstPipeline_);
        gstPipeline_ = nullptr;
        statusLabel_->setText("Поток остановлен");
        statusLabel_->show();
        liveDot_->hide();
        liveLabel_->hide();
        resolutionLabel_->setText(QStringLiteral("—"));
        fpsLabel_->setText(QStringLiteral("— FPS"));
        frameTimer_.invalidate();
        frameCount_ = 0;
        qDebug() << "Видео-поток остановлен";
    }
}

GstFlowReturn VideoPlayerWidget::newSampleCallback(GstAppSink *appsink, gpointer userData)
{
    GstSample *sample = gst_app_sink_pull_sample(appsink);
    if (!sample)
        return GST_FLOW_OK;

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstCaps *caps = gst_sample_get_caps(sample);
    GstStructure *structure = gst_caps_get_structure(caps, 0);

    int width = 0, height = 0;
    gst_structure_get_int(structure, "width", &width);
    gst_structure_get_int(structure, "height", &height);

    GstMapInfo map;
    if (gst_buffer_map(buffer, &map, GST_MAP_READ))
    {
        QImage img(map.data, width, height, width * 3, QImage::Format_RGB888);
        QPixmap pix = QPixmap::fromImage(img.copy());

        auto *self = static_cast<VideoPlayerWidget *>(userData);
        QMetaObject::invokeMethod(self, "frameReceived", Qt::QueuedConnection, Q_ARG(QPixmap, pix));

        gst_buffer_unmap(buffer, &map);
    }

    gst_sample_unref(sample);
    return GST_FLOW_OK;
}

void VideoPlayerWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!currentPixmap_.isNull())
    {
        imageLabel_->setPixmap(currentPixmap_.scaled(
            imageLabel_->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }
    hudOverlay_->setGeometry(imageLabel_->rect());
    hudOverlay_->raise();
    statusLabel_->adjustSize();
    statusLabel_->move((imageLabel_->width() - statusLabel_->width()) / 2,
                       (imageLabel_->height() - statusLabel_->height()) / 2);
    constexpr int thumbnailWidth = 138;
    constexpr int thumbnailHeight = 78;
    constexpr int thumbnailSpacing = 10;
    const int stripWidth =
        qMin(imageLabel_->width() - 24, 4 * thumbnailWidth + 3 * thumbnailSpacing);
    thumbnailStrip_->setGeometry(12, imageLabel_->height() - thumbnailHeight - 8, stripWidth,
                                 thumbnailHeight);
    thumbnailStrip_->raise();
}
