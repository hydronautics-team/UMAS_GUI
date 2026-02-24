#pragma once

#include <QWidget>
#include <cstdint>

#include <sensor_msgs/msg/image.hpp>

class QLabel;

class VideoPlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPlayerWidget(QWidget* parent = nullptr);

public slots:
    void onFrame(sensor_msgs::msg::Image::ConstSharedPtr frame);

private:
    QLabel* titleLabel_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    std::uint64_t frameCounter_ = 0;
};
