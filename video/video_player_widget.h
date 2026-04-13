/*
Подключение камеры на нулине на момент 08.04.26
0) СОздаем второй контейнер нулины
nulina@nulina:~$ docker exec -it stingray_core /bin/bash
1) source /opt/ros/humble/setup.bash
2) colcon build --packages-select camera
3) apt update /// apt-get update -o Acquire::ForceIPv4=true
4) apt install ros-humble-usb-cam ////   apt install ros-humble-v4l2-camera -y
5) source install/setup.bash
6) ros2 launch camera video_display.launch.py

source /opt/ros/humble/setup.bash
colcon build --packages-select camera
apt-get update -o Acquire::ForceIPv4=true
apt install ros-humble-usb-cam
source install/setup.bash
ros2 launch camera video_display.launch.py


*/


#include <QLabel>
#include <QImage>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/compressed_image.hpp>

class VideoPlayerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoPlayerWidget(QWidget *parent = nullptr);
    static QImage rosImageToQImage(const sensor_msgs::msg::Image::ConstSharedPtr& msg);
    static QImage compressedImageToQImage(const sensor_msgs::msg::CompressedImage::ConstSharedPtr &msg);

public slots:
    void onFrame(sensor_msgs::msg::Image::ConstSharedPtr frame);
    void onFrameQImage(const QImage &img);
       void onFrameCompressed(const sensor_msgs::msg::CompressedImage::ConstSharedPtr &msg);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateDisplay();
    QLabel *titleLabel_;
    QLabel *imageLabel_;
    QLabel *statusLabel_;
    QImage current_frame_;
    int frameCounter_ = 0;
};