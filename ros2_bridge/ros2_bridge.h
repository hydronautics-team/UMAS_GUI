
// // rosworker.hpp
// #include <rclcpp/rclcpp.hpp>
// #include <geometry_msgs/msg/twist.hpp>
// #include <QThread>

// class RosBridge : public QThread {
//     Q_OBJECT
// public:
//     void run() override {
//         rclcpp::init(0, nullptr);
//         node = rclcpp::Node::make_shared("qt_controller_node");
//         pub = node->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
//         rclcpp::spin(node);
//         rclcpp::shutdown();
//     }

//     void publishCommand(double linear, double angular) {
//         if (!pub) return;
//         geometry_msgs::msg::Twist msg;
//         msg.linear.x = linear;
//         msg.angular.z = angular;
//         pub->publish(msg);
//     }

// private:
//     rclcpp::Node::SharedPtr node;
//     rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub;
// };с
#pragma once
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <QThread>
#include <QObject>

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

signals:
    void poseUpdated(double x, double y, double z);

private:
    rclcpp::Node::SharedPtr node_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr sub_;
    bool ready_ = false;
};
