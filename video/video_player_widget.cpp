#include "video_player_widget.h"
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QPixmap>
#include <QDebug>
#include <sensor_msgs/msg/compressed_image.hpp>

VideoPlayerWidget::VideoPlayerWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    titleLabel_ = new QLabel("Video Player", this);
    titleLabel_->setAlignment(Qt::AlignCenter);

    imageLabel_ = new QLabel(this);
    imageLabel_->setAlignment(Qt::AlignCenter);
    imageLabel_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imageLabel_->setMinimumSize(160, 120);
    imageLabel_->setStyleSheet("background-color: black;");

    statusLabel_ = new QLabel("Ожидание кадров...", this);
    statusLabel_->setAlignment(Qt::AlignCenter);

    layout->addWidget(titleLabel_);
    layout->addWidget(imageLabel_, 1);
    layout->addWidget(statusLabel_);
}

// Слот для сырых изображений (если понадобятся)
void VideoPlayerWidget::onFrame(sensor_msgs::msg::Image::ConstSharedPtr frame)
{
    if (!frame) return;
    qDebug() << "onFrame called, encoding:" << QString::fromStdString(frame->encoding)
             << "size:" << frame->width << "x" << frame->height;
    ++frameCounter_;

    QImage img = rosImageToQImage(frame).copy(); 
    if (img.isNull()) {
        statusLabel_->setText("Ошибка конвертации кадра");
        return;
    }

    current_frame_ = img;
    updateDisplay();

    statusLabel_->setText(
        QString("Кадр #%1 | %2x%3 | encoding: %4")
            .arg(frameCounter_)
            .arg(frame->width)
            .arg(frame->height)
            .arg(QString::fromStdString(frame->encoding)));
}

void VideoPlayerWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateDisplay();
}

void VideoPlayerWidget::updateDisplay()
{
    if (current_frame_.isNull() || !imageLabel_) return;

    QSize labelSize = imageLabel_->size();
    if (labelSize.isEmpty()) return;

    QPixmap pix = QPixmap::fromImage(
        current_frame_.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)
    );
    imageLabel_->setPixmap(pix);
}

void VideoPlayerWidget::onFrameQImage(const QImage &img)
{
    if (img.isNull()) return;

    ++frameCounter_;
    current_frame_ = img;
    updateDisplay();

    statusLabel_->setText(
        QString("Кадр #%1 | %2x%3 | compressed")
            .arg(frameCounter_)
            .arg(img.width())
            .arg(img.height()));

    qDebug() << "onFrameQImage received, size:" << img.width() << "x" << img.height();
}

QImage VideoPlayerWidget::compressedImageToQImage(const sensor_msgs::msg::CompressedImage::ConstSharedPtr &msg)
{
    if (!msg || msg->data.empty()) {
        qWarning() << "Empty compressed image data";
        return QImage();
    }

    qDebug() << "compressedImageToQImage: data size =" << msg->data.size();

    QByteArray data(reinterpret_cast<const char*>(msg->data.data()), msg->data.size());
    QImage img;
    if (img.loadFromData(data)) {
        qDebug() << "Decoded compressed image, size:" << img.width() << "x" << img.height();
        return img;
    } else {
        qWarning() << "Failed to decode compressed image, data size:" << msg->data.size();
        return QImage();
    }
}

void VideoPlayerWidget::onFrameCompressed(const sensor_msgs::msg::CompressedImage::ConstSharedPtr &msg)
{
    qDebug() << "onFrameCompressed called";
    if (!msg) {
        qDebug() << "msg is null";
        return;
    }
    QImage img = compressedImageToQImage(msg);
    if (!img.isNull()) {
        onFrameQImage(img);
    } else {
        statusLabel_->setText("Ошибка декодирования сжатого кадра");
    }
}

QImage VideoPlayerWidget::rosImageToQImage(const sensor_msgs::msg::Image::ConstSharedPtr& msg)
{
    if (msg->data.empty() || msg->width == 0 || msg->height == 0) {
        return QImage();
    }

    const std::string& encoding = msg->encoding;

    if (encoding == "rgb8") {
        return QImage(msg->data.data(), msg->width, msg->height,
                      msg->step, QImage::Format_RGB888).copy();
    }
    else if (encoding == "bgr8") {
        QImage img(msg->data.data(), msg->width, msg->height,
                   msg->step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    else if (encoding == "mono8" || encoding == "8UC1") {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        return QImage(msg->data.data(), msg->width, msg->height,
                      msg->step, QImage::Format_Grayscale8).copy();
#else
        QImage rgbImage(msg->width, msg->height, QImage::Format_RGB888);
        const uint8_t* src = msg->data.data();
        for (int y = 0; y < msg->height; ++y) {
            uchar* dst = rgbImage.scanLine(y);
            for (int x = 0; x < msg->width; ++x) {
                uint8_t gray = src[y * msg->step + x];
                *dst++ = gray;
                *dst++ = gray;
                *dst++ = gray;
            }
        }
        return rgbImage;
#endif
    }
    else if (encoding == "rgba8") {
        return QImage(msg->data.data(), msg->width, msg->height,
                      msg->step, QImage::Format_RGBA8888).copy();
    }
    else if (encoding == "bgra8") {
        QImage img(msg->data.data(), msg->width, msg->height,
                   msg->step, QImage::Format_RGBA8888);
        return img.rgbSwapped().copy();
    }
    else if (encoding == "yuv422_yuy2" || encoding == "yuyv") {
        QImage rgbImage(msg->width, msg->height, QImage::Format_RGB888);
        const uint8_t* src = msg->data.data();
        size_t step = msg->step;

        for (int y = 0; y < msg->height; ++y) {
            const uint8_t* src_row = src + y * step;
            uchar* dst_row = rgbImage.scanLine(y);

            for (int x = 0; x < msg->width; x += 2) {
                uint8_t y0 = src_row[0];
                uint8_t u  = src_row[1];
                uint8_t y1 = src_row[2];
                uint8_t v  = src_row[3];
                src_row += 4;

                int c = y0 - 16;
                int d = u - 128;
                int e = v - 128;
                int r = (298 * c + 409 * e + 128) >> 8;
                int g = (298 * c - 100 * d - 208 * e + 128) >> 8;
                int b = (298 * c + 516 * d + 128) >> 8;
                dst_row[0] = qBound(0, r, 255);
                dst_row[1] = qBound(0, g, 255);
                dst_row[2] = qBound(0, b, 255);
                dst_row += 3;

                c = y1 - 16;
                r = (298 * c + 409 * e + 128) >> 8;
                g = (298 * c - 100 * d - 208 * e + 128) >> 8;
                b = (298 * c + 516 * d + 128) >> 8;
                dst_row[0] = qBound(0, r, 255);
                dst_row[1] = qBound(0, g, 255);
                dst_row[2] = qBound(0, b, 255);
                dst_row += 3;
            }
        }
        return rgbImage;
    }
    else {
        qWarning() << "Неподдерживаемый формат изображения:" << QString::fromStdString(encoding);
        return QImage();
    }
}