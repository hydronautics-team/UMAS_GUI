
#pragma once
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <std_msgs/msg/u_int8.hpp>
#include <QThread>
#include <QObject>
#include <QDebug>
#include <std_msgs/msg/bool.hpp>

// using namespace std::chrono_literals;

class RosBridge : public QThread {
    Q_OBJECT

public:
    RosBridge(QObject* parent = nullptr) : QThread(parent) {}

    void run() override {
        if (!rclcpp::ok()) {
            rclcpp::init(0, nullptr); // инициализация ROS2 один раз
        }

        node_ = rclcpp::Node::make_shared("qt_controller_node");
        pub_ = node_->create_publisher<geometry_msgs::msg::Twist>("/control/data", 10);

        // подписка на pose (пример)
        sub_ = node_->create_subscription<geometry_msgs::msg::Pose>(
            "pose_topic", 10,
            [this](const geometry_msgs::msg::Pose::SharedPtr msg){
                emit poseUpdated(msg->position.x, msg->position.y, msg->position.z);
            });

        control_mode_pub_ =
            node_->create_publisher<std_msgs::msg::UInt8>(
                "/control/loop_flags", 10);

        zero_yaw_pub_ =
            node_->create_publisher<std_msgs::msg::Bool>(
                "/imu/zero_yaw", 10);


        ready_ = true; // publisher готов

        // rclcpp::Rate rate(node_, 100ms);
        // while (rclcpp::ok()) {
        rclcpp::spin(node_);
            // rate.sleep();
        // }

        rclcpp::shutdown();
    }

    bool isReady() const { return ready_; }

public slots:
    void publishCommand(double x, double y, double z, double angular_x, double angular_y, double angular_z) {
        if (!pub_ || !ready_) return;
        geometry_msgs::msg::Twist msg;
        msg.linear.x = x;
        msg.linear.y = y;
        msg.linear.z = z;
        msg.angular.x = angular_x;
        msg.angular.y = angular_y;
        msg.angular.z = angular_z;
        pub_->publish(msg);
    }

    void zeroYaw() {
        if (!ready_ || !zero_yaw_pub_) return;

        std_msgs::msg::Bool msg;
        msg.data = true;
        zero_yaw_pub_->publish(msg);

        qDebug() << "Published yaw zero signal";
}


    void setModeSurge(bool checked)  { qDebug() << "Setting surge mode to:" << checked; setFlag(0, checked); }
    void setModeSway(bool checked)   { qDebug() << "Setting sway mode to:" << checked; setFlag(1, checked); }
    void setModeHeave(bool checked)  { qDebug() << "Setting heave mode to:" << checked; setFlag(2, checked); }
    void setModeYaw(bool checked)    { qDebug() << "Setting yaw mode to:" << checked; setFlag(3, checked); }
    void setModePitch(bool checked)  { qDebug() << "Setting pitch mode to:" << checked; setFlag(4, checked); }
    void setModeRoll(bool checked)   { qDebug() << "Setting roll mode to:" << checked; setFlag(5, checked); }


signals:
    void poseUpdated(double x, double y, double z);

private:
    rclcpp::Node::SharedPtr node_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr sub_;
    bool ready_ = false;
    uint8_t control_mode_flags_ = 0;
    rclcpp::Publisher<std_msgs::msg::UInt8>::SharedPtr control_mode_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr zero_yaw_pub_;

    void setFlag(uint8_t bit, bool value) {
        if (!ready_ || !control_mode_pub_) return;

        if (value)
            control_mode_flags_ |=  (1 << bit);
        else
            control_mode_flags_ &= ~(1 << bit);

        std_msgs::msg::UInt8 msg;
        msg.data = control_mode_flags_;
        control_mode_pub_->publish(msg);
        qDebug() << "Published control mode flags:" << static_cast<int>(control_mode_flags_);
    }

};
