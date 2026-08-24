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

void RosBridge::run()
{
    if (!rclcpp::ok()) {
        rclcpp::init(0, nullptr);
    }

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

    depth_sub_ = node_->create_subscription<std_msgs::msg::Float32>(
        "/stingray_core/pressure_sensor/depth", rclcpp::SensorDataQoS(),
        [this](std_msgs::msg::Float32::SharedPtr msg) { emit depthReceived(msg->data); });

    bottom_sub_ = node_->create_subscription<std_msgs::msg::Float32>(
        "/sensors/altitude", 10,
        [this](std_msgs::msg::Float32::SharedPtr msg) { emit bottomReceived(msg->data); });

    // temp_sub_ = node_->create_subscription<std_msgs::msg::Float32>(
    //     "/sensors/temperature", 10,
    //     [this](std_msgs::msg::Float32::SharedPtr msg) { emit temperatureReceived(msg->data); });

    // leak_sub_ = node_->create_subscription<std_msgs::msg::Bool>(
    //     "/sensors/leak", 10,
    //     [this](std_msgs::msg::Bool::SharedPtr msg) { emit leakReceived(msg->data); });

    battery_sub_ = node_->create_subscription<sensor_msgs::msg::BatteryState>(
    "/power/battery/state",
    rclcpp::SensorDataQoS(),
    [this](const sensor_msgs::msg::BatteryState::SharedPtr msg) {
        // Counting ping
        auto now = node_->now();
        auto publish_time = msg->header.stamp;
        
        rclcpp::Duration latency = now - publish_time;
        int64_t latency_ns = latency.nanoseconds();
        int pingMs = static_cast<int>(latency_ns / 1000000);
        
        emit pingReceived(pingMs);
        
        const double percentage = msg->percentage;
        if (msg->location == "1") {
            emit battery1Received(percentage);
        }
        else if (msg->location == "2") {
            emit battery2Received(percentage);
        }
    });

    kill_switch_sub_ = node_->create_subscription<std_msgs::msg::Bool>(
        "/safety/kill_switch", 10,
        [this](std_msgs::msg::Bool::SharedPtr msg) { emit killswitchReceived(msg->data); });

    control_flags_pub_ =
        node_->create_publisher<std_msgs::msg::UInt8>("/control/loop_flags", 10);

    zero_yaw_pub_ =
        node_->create_publisher<std_msgs::msg::Bool>("/imu/zero_yaw", 10);

    is_ready_ = true;

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
        control_flags_ |= (1u << bit);
    else
        control_flags_ &= ~(1u << bit);

    std_msgs::msg::UInt8 msg;
    msg.data = control_flags_;
    control_flags_pub_->publish(msg);

    emit controlFlagsPublished(control_flags_);

    qDebug() << "Published control mode flags:" << static_cast<int>(control_flags_);
}