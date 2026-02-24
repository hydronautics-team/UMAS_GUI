#pragma once
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <std_msgs/msg/u_int8.hpp>
#include <std_msgs/msg/bool.hpp>
#include <QThread>
#include <QObject>
#include <atomic>

#include "uv_state.h"

class RosBridge : public QThread {
    Q_OBJECT

public:
    explicit RosBridge(QObject* parent = nullptr);
    ~RosBridge() override;

    bool isReady() const;

    void run() override;

signals:
    // "Сырой" сигнал (может быть полезен для отладки)
    void poseUpdated(double x, double y, double z);

    // Сигналы для обновления UVState-модели.
    void poseReceived(const UVState::Pose& pose);
    void controlFlagsPublished(std::uint8_t flags);

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

    std::atomic<bool> is_ready_{false};
    uint8_t control_flags_ = 0;
};
