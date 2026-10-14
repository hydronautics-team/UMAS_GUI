#include "ros2_bridge.h"
#include <QDebug>
#include <algorithm>
#include <cmath>

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

            // geometry_msgs/Pose stores orientation as an ENU quaternion.
            // Convert it to aerospace roll/pitch/yaw angles for the HUD.
            const double x = msg->orientation.x;
            const double y = msg->orientation.y;
            const double z = msg->orientation.z;
            const double w = msg->orientation.w;

            UVState::ImuData imu;
            const double sinRollCosPitch = 2.0 * (w * x + y * z);
            const double cosRollCosPitch = 1.0 - 2.0 * (x * x + y * y);
            const double sinPitch = std::clamp(2.0 * (w * y - z * x), -1.0, 1.0);
            const double sinYawCosPitch = 2.0 * (w * z + x * y);
            const double cosYawCosPitch = 1.0 - 2.0 * (y * y + z * z);
            constexpr double radToDeg = 180.0 / 3.14159265358979323846;

            imu.roll = static_cast<float>(std::atan2(sinRollCosPitch, cosRollCosPitch) * radToDeg);
            imu.pitch = static_cast<float>(std::asin(sinPitch) * radToDeg);
            imu.yaw = static_cast<float>(std::atan2(sinYawCosPitch, cosYawCosPitch) * radToDeg);
            imu.quat[0] = static_cast<float>(x);
            imu.quat[1] = static_cast<float>(y);
            imu.quat[2] = static_cast<float>(z);
            imu.quat[3] = static_cast<float>(w);
            emit imuReceived(imu);
        });

    control_flags_pub_ =
        node_->create_publisher<std_msgs::msg::UInt8>("/control/loop_flags", 10);

    zero_yaw_pub_ =
        node_->create_publisher<std_msgs::msg::Bool>("/imu/zero_yaw", 10);

    lights_mode_pub_      = node_->create_publisher<std_msgs::msg::Int32>("/lights/mode", 10);
    lights_brightness_pub_ = node_->create_publisher<std_msgs::msg::Int32>("/lights/brightness", 10);

        manipulator_pwm_pub_ = node_->create_publisher<std_msgs::msg::Int32>(
        "/stingray_core/devices/manipulator/cmd", 10);

    manipulator_timer_ = node_->create_wall_timer(
        std::chrono::milliseconds(50),
        [this]() { updateManipulatorCommand(); }
    );

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

void RosBridge::publishLightsMode(unsigned mode)
{
    if (!is_ready_ || !lights_mode_pub_) return;
    std_msgs::msg::Int32 msg;
    msg.data = static_cast<int32_t>(mode);
    lights_mode_pub_->publish(msg);
    qDebug() << "Published light mode:" << mode;
}

void RosBridge::publishLightsBrightness(unsigned value)
{
    if (!is_ready_ || !lights_brightness_pub_) return;
    std_msgs::msg::Int32 msg;
    msg.data = static_cast<int32_t>(value);
    lights_brightness_pub_->publish(msg);
}

void RosBridge::setTurnState(const QString& state)
{
    turn_state_ = state.toLower();
}

void RosBridge::setGripState(const QString& state)
{
    grip_state_ = state.toLower();
}

void RosBridge::updateManipulatorCommand()
{
    if (!is_ready_ || !manipulator_pwm_pub_) return;

    // Кодируем состояние в число: turn * 10 + grip
    // turn: 0=stop, 1=left, 2=right
    // grip: 0=stop, 1=close, 2=open
    int turn_code = 0;
    if (turn_state_ == "left") turn_code = 1;
    else if (turn_state_ == "right") turn_code = 2;

    int grip_code = 0;
    if (grip_state_ == "close") grip_code = 1;
    else if (grip_state_ == "open") grip_code = 2;

    int command = (turn_code * 10) + grip_code;

    std_msgs::msg::Int32 msg;
    msg.data = command;
    manipulator_pwm_pub_->publish(msg);

    // Лог только при изменении команды
    static int last_command = -1;
    if (command != last_command) {
        qDebug() << "Manipulator command:" << command 
                 << "(turn:" << turn_state_ << ", grip:" << grip_state_ << ")";
        last_command = command;
    }
}
