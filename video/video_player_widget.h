#pragma once

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <cstdint>
#include <sensor_msgs/msg/image.hpp>

class VideoPlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPlayerWidget(QWidget* parent = nullptr);

public slots:
    void onFrame(sensor_msgs::msg::Image::ConstSharedPtr frame);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateDisplay();
    QImage rosImageToQImage(const sensor_msgs::msg::Image::ConstSharedPtr& msg);

    QLabel* titleLabel_ = nullptr;
    QLabel* imageLabel_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    std::uint64_t frameCounter_ = 0;
    QImage current_frame_;
};