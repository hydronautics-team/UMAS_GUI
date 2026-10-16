#pragma once
#include "uv_state.h"
#include <QObject>
#include <QThread>
#include <atomic>
#include <chrono>
#include <geometry_msgs/msg/twist.hpp>
#include <mutex>
#include <nav_msgs/msg/odometry.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/battery_state.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/float32.hpp>
#include <std_msgs/msg/int32.hpp>
#include <std_msgs/msg/u_int8.hpp>
#include <std_srvs/srv/trigger.hpp>

class RosBridge : public QThread
{
    Q_OBJECT

public:
    explicit RosBridge(QObject *parent = nullptr);
    ~RosBridge() override;

    bool isReady() const;
    void run() override;
    void publishPwmInternal(int pwm_value);

    void setTurnState(const QString &state);
    void setGripState(const QString &state);
    void updateManipulatorCommand();

public:
    void setTurnSpeed(float speed);

private:
    float turn_speed_ = 0.0f;
signals:
    void readyChanged(bool ready);
    void poseUpdated(double x, double y, double z);
    void poseReceived(const UVState::Pose &pose);
    void imuReceived(const UVState::ImuData &imu);
    void batteryReceived(const UVState::BatteryData &battery);
    void temperatureReceived(float temperatureCelsius);
    void bottomDistanceReceived(float distanceMeters);
    void leakStateReceived(bool leakDetected);
    void killSwitchStateReceived(bool engaged);
    void controlFlagsPublished(std::uint8_t flags);
    void dvlOriginResetFinished(bool success, const QString &message);

public slots:
    void publishTwistInternal(double x, double y, double z, double angular_x, double angular_y,
                              double angular_z);
    void zeroYawInternal();
    void resetDvlOriginInternal();
    void setControlFlagInternal(uint8_t bit, bool value);
    void publishLightsMode(unsigned mode);
    void publishLightsBrightness(unsigned value);

private:
    rclcpp::Node::SharedPtr node_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr twist_pub_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odometry_sub_;
    rclcpp::Subscription<sensor_msgs::msg::BatteryState>::SharedPtr battery_sub_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr temperature_sub_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr bottom_distance_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr leak_state_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr kill_switch_state_sub_;
    UVState::BatteryData battery_data_{};
    rclcpp::Publisher<std_msgs::msg::UInt8>::SharedPtr control_flags_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr zero_yaw_pub_;
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr reset_dvl_origin_client_;
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr lights_mode_pub_;
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr lights_brightness_pub_;

    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr manipulator_pwm_pub_;
    rclcpp::TimerBase::SharedPtr manipulator_timer_;

    std::atomic<bool> is_ready_{false};
    uint8_t control_flags_ = 0;
    int current_pwm_ = 0;

    int last_sent_command_ = -1;
    QString target_state_ = "stop";
    QString turn_state_ = "none";
    QString grip_state_ = "stop";
};
