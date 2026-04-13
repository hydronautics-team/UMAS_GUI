#include "ros2_bridge.h"
#include <QDebug>

RosBridge::RosBridge(QObject* parent)
    : QThread(parent)
{}

RosBridge::~RosBridge()
{
    if (rclcpp::ok()) {
        rclcpp::shutdown();
    }
    wait();
}

bool RosBridge::isReady() const
{
    return is_ready_.load();
}

sensor_msgs::msg::Image::ConstSharedPtr RosBridge::takeLatestCameraFrame()
{
    std::lock_guard<std::mutex> lock(camera_mutex_);
    auto frame = latest_camera_frame_;
    latest_camera_frame_.reset();
    return frame;
}

void RosBridge::run()
{
    if (!rclcpp::ok()) {
        rclcpp::init(0, nullptr);
    }

    // Namespace "qt_controller" — все относительные топики получат префикс /qt_controller/
    node_ = rclcpp::Node::make_shared("qt_controller_node", "qt_controller");

    twist_pub_ = node_->create_publisher<geometry_msgs::msg::Twist>("/control/data", 10);

    pose_sub_ = node_->create_subscription<geometry_msgs::msg::Pose>(
        "pose_topic", 10,
        [this](const geometry_msgs::msg::Pose::SharedPtr msg) {
            emit poseUpdated(msg->position.x, msg->position.y, msg->position.z);

            UVState::Pose pose;
            pose.x = msg->position.x;
            pose.y = msg->position.y;
            pose.z = msg->position.z;
            emit poseReceived(pose);
        });
camera_sub_ = node_->create_subscription<sensor_msgs::msg::Image>(
    "/stingray_core/topics/camera_1",
    rclcpp::QoS(100).reliable(),   // было 10, стало 100
    [this](sensor_msgs::msg::Image::ConstSharedPtr msg) {
        static int frame_counter = 0;
        RCLCPP_INFO(node_->get_logger(), "Camera frame #%d", ++frame_counter);
        std::lock_guard<std::mutex> lock(camera_mutex_);
        latest_camera_frame_ = std::move(msg);
    });

    control_flags_pub_ =
        node_->create_publisher<std_msgs::msg::UInt8>("control/loop_flags", 10);

    zero_yaw_pub_ =
        node_->create_publisher<std_msgs::msg::Bool>("imu/zero_yaw", 10);

    is_ready_ = true;

    // spin_some позволяет Qt обрабатывать очередь сигналов (QueuedConnection)
    while (rclcpp::ok() && !isInterruptionRequested()) {
        rclcpp::spin_some(node_);
        QThread::msleep(1);
    }

    rclcpp::shutdown();
}

void RosBridge::publishTwistInternal(double x, double y, double z,
                                     double angular_x, double angular_y, double angular_z)
{
    if (!is_ready_ || !twist_pub_) return;

    geometry_msgs::msg::Twist msg;
    msg.linear.x  = x;
    msg.linear.y  = y;
    msg.linear.z  = z;
    msg.angular.x = angular_x;
    msg.angular.y = angular_y;
    msg.angular.z = angular_z;
    twist_pub_->publish(msg);
}

void RosBridge::zeroYawInternal()
{
    if (!is_ready_ || !zero_yaw_pub_) return;

    std_msgs::msg::Bool msg;
    msg.data = true;
    zero_yaw_pub_->publish(msg);

    qDebug() << "Published yaw zero signal";
}

void RosBridge::setControlFlagInternal(uint8_t bit, bool value)
{
    if (!is_ready_ || !control_flags_pub_) return;

    if (value)
        control_flags_ |=  (1u << bit);
    else
        control_flags_ &= ~(1u << bit);

    std_msgs::msg::UInt8 msg;
    msg.data = control_flags_;
    control_flags_pub_->publish(msg);

    emit controlFlagsPublished(control_flags_);

    qDebug() << "Published control mode flags:" << static_cast<int>(control_flags_);
}
