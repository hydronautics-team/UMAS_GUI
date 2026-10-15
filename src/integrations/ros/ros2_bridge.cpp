#include "ros2_bridge.h"
#include <QDebug>
#include <algorithm>
#include <cmath>

RosBridge::RosBridge(QObject *parent) : QThread(parent) {}

RosBridge::~RosBridge()
{
    if (rclcpp::ok())
    {
        rclcpp::shutdown();
    }
    wait();
}

bool RosBridge::isReady() const { return is_ready_.load(); }

void RosBridge::run()
{
    if (!rclcpp::ok())
    {
        rclcpp::init(0, nullptr);
    }

    node_ = rclcpp::Node::make_shared("qt_controller_node", "qt_controller");

    twist_pub_ = node_->create_publisher<geometry_msgs::msg::Twist>("/control/data", 10);

    odometry_sub_ = node_->create_subscription<nav_msgs::msg::Odometry>(
        "/core/state/odometry", rclcpp::QoS(10).reliable(),
        [this](const nav_msgs::msg::Odometry::SharedPtr msg)
        {
            const auto nowMs =
                static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                                               std::chrono::system_clock::now().time_since_epoch())
                                               .count());
            emit poseUpdated(msg->pose.pose.position.x, msg->pose.pose.position.y,
                             msg->pose.pose.position.z);

            UVState::Pose pose;
            pose.x = msg->pose.pose.position.x;
            pose.y = msg->pose.pose.position.y;
            pose.z = msg->pose.pose.position.z;
            pose.velocity_x = msg->twist.twist.linear.x;
            pose.velocity_y = msg->twist.twist.linear.y;
            pose.velocity_z = msg->twist.twist.linear.z;
            pose.timestamp_ms = nowMs;
            emit poseReceived(pose);

            const double x = msg->pose.pose.orientation.x;
            const double y = msg->pose.pose.orientation.y;
            const double z = msg->pose.pose.orientation.z;
            const double w = msg->pose.pose.orientation.w;

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
            imu.timestamp_ms = nowMs;
            emit imuReceived(imu);
        });

    battery_sub_ = node_->create_subscription<sensor_msgs::msg::BatteryState>(
        "/stingray_core/battery/state", rclcpp::SensorDataQoS(),
        [this](const sensor_msgs::msg::BatteryState::SharedPtr msg)
        {
            const auto nowMs =
                static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                                               std::chrono::system_clock::now().time_since_epoch())
                                               .count());
            if (msg->location == "1")
            {
                battery_data_.voltage_1 = msg->voltage;
                battery_data_.percentage_1 = msg->percentage;
                battery_data_.timestamp_1_ms = nowMs;
            }
            else if (msg->location == "2")
            {
                battery_data_.voltage_2 = msg->voltage;
                battery_data_.percentage_2 = msg->percentage;
                battery_data_.timestamp_2_ms = nowMs;
            }
            else
            {
                return;
            }
            emit batteryReceived(battery_data_);
        });

    control_flags_pub_ = node_->create_publisher<std_msgs::msg::UInt8>("/control/loop_flags", 10);

    zero_yaw_pub_ = node_->create_publisher<std_msgs::msg::Bool>("/imu/zero_yaw", 10);
    reset_dvl_origin_client_ =
        node_->create_client<std_srvs::srv::Trigger>("/core/localization/reset_origin");

    lights_mode_pub_ = node_->create_publisher<std_msgs::msg::Int32>("/lights/mode", 10);
    lights_brightness_pub_ =
        node_->create_publisher<std_msgs::msg::Int32>("/lights/brightness", 10);

    manipulator_pwm_pub_ =
        node_->create_publisher<std_msgs::msg::Int32>("/stingray_core/devices/manipulator/cmd", 10);

    manipulator_timer_ = node_->create_wall_timer(std::chrono::milliseconds(50),
                                                  [this]() { updateManipulatorCommand(); });

    is_ready_ = true;
    emit readyChanged(true);

    while (rclcpp::ok() && !isInterruptionRequested())
    {
        rclcpp::spin_some(node_);
        QThread::msleep(1);
    }

    is_ready_ = false;
    emit readyChanged(false);
    rclcpp::shutdown();
}

void RosBridge::publishTwistInternal(double x, double y, double z, double angular_x,
                                     double angular_y, double angular_z)
{
    if (!is_ready_ || !twist_pub_)
        return;

    geometry_msgs::msg::Twist msg;
    msg.linear.x = x;
    msg.linear.y = y;
    msg.linear.z = z;
    msg.angular.x = angular_x;
    msg.angular.y = angular_y;
    msg.angular.z = angular_z;
    twist_pub_->publish(msg);
}

void RosBridge::zeroYawInternal()
{
    if (!is_ready_ || !zero_yaw_pub_)
        return;

    std_msgs::msg::Bool msg;
    msg.data = true;
    zero_yaw_pub_->publish(msg);

    qDebug() << "Published yaw zero signal";
}

void RosBridge::resetDvlOriginInternal()
{
    if (!is_ready_ || !reset_dvl_origin_client_)
    {
        emit dvlOriginResetFinished(false, "ROS-мост ещё не готов");
        return;
    }

    if (!reset_dvl_origin_client_->service_is_ready())
    {
        emit dvlOriginResetFinished(false, "Сервис сброса положения DVL недоступен");
        return;
    }

    auto request = std::make_shared<std_srvs::srv::Trigger::Request>();
    reset_dvl_origin_client_->async_send_request(
        request,
        [this](rclcpp::Client<std_srvs::srv::Trigger>::SharedFuture future)
        {
            try
            {
                const auto response = future.get();
                emit dvlOriginResetFinished(response->success,
                                            QString::fromStdString(response->message));
            }
            catch (const std::exception &error)
            {
                emit dvlOriginResetFinished(false,
                                            QString("Ошибка вызова сервиса: %1").arg(error.what()));
            }
        });
}

void RosBridge::setControlFlagInternal(uint8_t bit, bool value)
{
    if (!is_ready_ || !control_flags_pub_)
        return;

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
    if (!is_ready_ || !lights_mode_pub_)
        return;
    std_msgs::msg::Int32 msg;
    msg.data = static_cast<int32_t>(mode);
    lights_mode_pub_->publish(msg);
    qDebug() << "Published light mode:" << mode;
}

void RosBridge::publishLightsBrightness(unsigned value)
{
    if (!is_ready_ || !lights_brightness_pub_)
        return;
    std_msgs::msg::Int32 msg;
    msg.data = static_cast<int32_t>(value);
    lights_brightness_pub_->publish(msg);
}

void RosBridge::setTurnState(const QString &state) { turn_state_ = state.toLower(); }

void RosBridge::setGripState(const QString &state) { grip_state_ = state.toLower(); }

void RosBridge::updateManipulatorCommand()
{
    if (!is_ready_ || !manipulator_pwm_pub_)
        return;

    // Кодируем состояние в число: turn * 10 + grip
    // turn: 0=stop, 1=left, 2=right
    // grip: 0=stop, 1=close, 2=open
    int turn_code = 0;
    if (turn_state_ == "left")
        turn_code = 1;
    else if (turn_state_ == "right")
        turn_code = 2;

    int grip_code = 0;
    if (grip_state_ == "close")
        grip_code = 1;
    else if (grip_state_ == "open")
        grip_code = 2;

    int command = (turn_code * 10) + grip_code;

    std_msgs::msg::Int32 msg;
    msg.data = command;
    manipulator_pwm_pub_->publish(msg);

    // Лог только при изменении команды
    static int last_command = -1;
    if (command != last_command)
    {
        qDebug() << "Manipulator command:" << command << "(turn:" << turn_state_
                 << ", grip:" << grip_state_ << ")";
        last_command = command;
    }
}
