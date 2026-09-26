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

    // ============================================================
    // 1. СОЗДАЁМ НОДУ
    // ============================================================
    node_ = rclcpp::Node::make_shared("qt_controller_node", "qt_controller");

    // ============================================================
    // 2. СОЗДАЁМ ПАБЛИШЕРЫ
    // ============================================================
    twist_pub_ = node_->create_publisher<geometry_msgs::msg::Twist>("/control/data", 10);

    control_flags_pub_ =
        node_->create_publisher<std_msgs::msg::UInt8>("/control/loop_flags", 10);

    zero_yaw_pub_ =
        node_->create_publisher<std_msgs::msg::Bool>("/imu/zero_yaw", 10);

    lights_mode_pub_      = node_->create_publisher<std_msgs::msg::Int32>("/lights/mode", 10);
    lights_brightness_pub_ = node_->create_publisher<std_msgs::msg::Int32>("/lights/brightness", 10);

    // Единственный паблишер для манипулятора
    //manipulator_pwm_pub_ = node_->create_publisher<std_msgs::msg::Int32>("/manipulator/pwm", 10);
    manipulator_pwm_pub_ = node_->create_publisher<std_msgs::msg::Int32>("/stingray_core/device/manipulator/cmd", 10);

    // ============================================================
    // 3. ТАЙМЕР ДЛЯ ОБНОВЛЕНИЯ ПОТОКА ДАННЫХ (КАЖДЫЕ 50 МС)
    // ============================================================
    manipulator_timer_ = node_->create_wall_timer(
        std::chrono::milliseconds(50),
        [this]() {
            updateManipulatorPwm();
        }
    );

    // ============================================================
    // 4. ПОДПИСКИ
    // ============================================================
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

    // Дефолтные состояния перед стартом
    target_state_ = "stop";
    turn_state_ = "stop";

    // Атомарно взводим готовность — теперь всё начнет слаться в топики
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

// ============================================================
// ЛОГИКА РАБОТЫ МАНИПУЛЯТОРА
// ============================================================

// ============================================================
// МАНИПУЛЯТОР — ОБНОВЛЕНИЕ ПО ТАЙМЕРУ (50 МС)
// ============================================================

void RosBridge::updateManipulatorPwm()
{
    if (!is_ready_) return;

    int turn = 0;
    if (turn_state_ == "left") {
        turn = 4;
    } else if (turn_state_ == "right") {
        turn = 5;
    } else if (turn_state_ == "stop") {
        turn = 3;
    } else {
        turn = 0;
    }

    int grip = 0;
    if (grip_state_ == "close") {
        grip = 1;
    } else if (grip_state_ == "open") {
        grip = 2;
    } else {
        grip = 0;
    }

    int combined = (turn * 10) + grip;

    // ВСЕГДА ШЛЁМ В ТОПИК
    if (manipulator_pwm_pub_) {
        std_msgs::msg::Int32 msg;
        msg.data = combined;
        manipulator_pwm_pub_->publish(msg);
    }

    // ЛОГ ТОЛЬКО ПРИ СМЕНЕ КОМАНДЫ
    static int last_sent_command_ = -1;
    if (combined != last_sent_command_) {
        last_sent_command_ = combined;
        qDebug() << "Manipulator command:" << combined;
    }
}
// ============================================================
// ПРЯМАЯ ОТПРАВКА (ДЛЯ МГНОВЕННЫХ КОМАНД)
// ============================================================

void RosBridge::publishPwmInternal(int pwm_value)
{
    if (!is_ready_ || !manipulator_pwm_pub_) {
        qDebug() << "WARNING: RosBridge is not ready or publisher is null!";
        return;
    }

    std_msgs::msg::Int32 msg;
    msg.data = pwm_value;
    manipulator_pwm_pub_->publish(msg);
    
    qDebug() << "Published Manipulator Command:" << pwm_value;
}

void RosBridge::setTurnState(const QString& state)
{
    turn_state_ = state.toLower();
}

void RosBridge::setGripState(const QString& state)
{
    grip_state_ = state.toLower();
}