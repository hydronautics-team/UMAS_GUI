#include "video_player_widget.h"

#include <QLabel>
#include <QVBoxLayout>

VideoPlayerWidget::VideoPlayerWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);

    titleLabel_ = new QLabel("VideoPlayerWidget (placeholder)", this);
    statusLabel_ = new QLabel("Ожидание кадров...", this);

    titleLabel_->setAlignment(Qt::AlignCenter);
    statusLabel_->setAlignment(Qt::AlignCenter);

    layout->addWidget(titleLabel_);
    layout->addWidget(statusLabel_);
    layout->addStretch(1);
}

void VideoPlayerWidget::onFrame(sensor_msgs::msg::Image::ConstSharedPtr frame)
{
    if (!frame) {
        return;
    }

    ++frameCounter_;
    statusLabel_->setText(
        QString("Кадр #%1 | %2x%3 | encoding: %4 | step: %5")
            .arg(frameCounter_)
            .arg(frame->width)
            .arg(frame->height)
            .arg(QString::fromStdString(frame->encoding))
            .arg(frame->step));
}
