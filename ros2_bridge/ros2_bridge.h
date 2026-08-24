#pragma once
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include "sensor_msgs/msg/battery_state.hpp"
#include <std_msgs/msg/u_int8.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/float32.hpp>
#include <std_msgs/msg/empty.hpp>
#include <QThread>
#include <QObject>
#include <atomic>
#include <chrono>
#include "uv_state.h"

class RosBridge : public QThread {
    Q_OBJECT

public:
    explicit RosBridge(QObject* parent = nullptr);
    ~RosBridge() override;

    bool isReady() const;
    void run() override;

signals:
    void poseUpdated(double x, double y, double z);
    void poseReceived(const UVState::Pose& pose);
    void controlFlagsPublished(std::uint8_t flags);

    // === Телеметрия ===
    void bottomReceived(double m);
    void temperatureReceived(double degC);
    void leakReceived(bool leak);
    void battery1Received(double percent);
    void battery2Received(double percent);
    void killswitchReceived(bool active);
    void heartbeatReceived(int periodMs);

public slots:
    void publishTwistInternal(double x, double y, double z,
                              double angular_x, double angular_y, double angular_z);
    void zeroYawInternal();
    void setControlFlagInternal(uint8_t bit, bool value);

private:
    rclcpp::Node::SharedPtr node_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr twist_pub_;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr pose_sub_;
    rclcpp::Publisher<std_msgs::msg::UInt8>::SharedPtr control_flags_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr zero_yaw_pub_;

    // === подписки телеметрии ===
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr bottom_sub_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr temp_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr leak_sub_;
    rclcpp::Subscription<sensor_msgs::msg::BatteryState>::SharedPtr battery_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr kill_switch_sub_;
    rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr heartbeat_sub_;

    std::chrono::steady_clock::time_point lastHeartbeat_{};

    std::atomic<bool> is_ready_{false};
    uint8_t control_flags_ = 0;
};