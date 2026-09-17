#pragma once
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <std_msgs/msg/u_int8.hpp>
#include <std_msgs/msg/bool.hpp>
#include <QThread>
#include <QObject>
#include <atomic>
#include <mutex>
#include <chrono>
#include "uv_state.h"
#include <std_msgs/msg/int32.hpp>

class RosBridge : public QThread {
    Q_OBJECT

public:
    explicit RosBridge(QObject* parent = nullptr);
    ~RosBridge() override;

    bool isReady() const;
    void run() override;
     void publishPwmInternal(int pwm_value);

         void setTurnState(const QString& state);
    void setGripState(const QString& state);

signals:
    void poseUpdated(double x, double y, double z);
    void poseReceived(const UVState::Pose& pose);
    void controlFlagsPublished(std::uint8_t flags);

public slots:
    void publishTwistInternal(double x, double y, double z,
                              double angular_x, double angular_y, double angular_z);
    void zeroYawInternal();
    void setControlFlagInternal(uint8_t bit, bool value);
       
void updateManipulatorPwm();

private:
    // Базовые элементы ROS 2 ноды

    rclcpp::Node::SharedPtr node_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr twist_pub_;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr pose_sub_;
    rclcpp::Publisher<std_msgs::msg::UInt8>::SharedPtr control_flags_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr zero_yaw_pub_;
    
    // Элементы управления манипулятором
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr manipulator_pwm_pub_;
    rclcpp::TimerBase::SharedPtr manipulator_timer_;
    
    // Переменные состояний и флагов
    std::atomic<bool> is_ready_{false};  // Одна общая переменная готовности (Thread-safe)
    uint8_t control_flags_ = 0;
    int current_pwm_ = 0;
    
         int last_sent_command_ = -1; 
    QString target_state_ = "stop";      
    QString turn_state_ = "none";
    QString grip_state_ = "stop";
    

};