#include "gamepad_input_source.h"

#include "axis_response.h"
#include "gamepad.h"
#include <chrono>
#include <cmath>

namespace
{
constexpr float STICK_DEADZONE = 15.0f;
constexpr double STICK_EXPO = 0.65;
std::uint64_t now_ms()
{
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                                          std::chrono::steady_clock::now().time_since_epoch())
                                          .count());
}
} // namespace

GamepadInputSource::GamepadInputSource(Gamepad *gamepad, QObject *parent)
    : QObject(parent), gamepad_(gamepad)
{
    // Стики: -100..100 -> -1..1
    connect(gamepad_, &Gamepad::leftStickYMoved, this,
            [this](float v)
            {
                march_ = static_cast<float>(
                    -umas::input::shapeStickAxis(v, 100.0, STICK_DEADZONE, STICK_EXPO) / 100.0);
                markDirty();
            });
    connect(gamepad_, &Gamepad::leftStickXMoved, this,
            [this](float v)
            {
                yaw_ = static_cast<float>(
                    umas::input::shapeStickAxis(v, 100.0, STICK_DEADZONE, STICK_EXPO) / 100.0);
                markDirty();
            });
    connect(gamepad_, &Gamepad::rightStickYMoved, this,
            [this](float v)
            {
                pitch_ = static_cast<float>(
                    umas::input::shapeStickAxis(v, 100.0, STICK_DEADZONE, STICK_EXPO) / 100.0);
                markDirty();
            });
    connect(gamepad_, &Gamepad::rightStickXMoved, this,
            [this](float v)
            {
                lag_ = static_cast<float>(
                    umas::input::shapeStickAxis(v, 100.0, STICK_DEADZONE, STICK_EXPO) / 100.0);
                markDirty();
            });

    // Крен: X/B
    connect(gamepad_, &Gamepad::buttonXPressed, this,
            [this]()
            {
                roll_ = -10.f;
                markDirty();
            });
    connect(gamepad_, &Gamepad::buttonXReleased, this,
            [this]()
            {
                roll_ = 0.f;
                markDirty();
            });
    connect(gamepad_, &Gamepad::buttonBPressed, this,
            [this]()
            {
                roll_ = 10.f;
                markDirty();
            });
    connect(gamepad_, &Gamepad::buttonBReleased, this,
            [this]()
            {
                roll_ = 0.f;
                markDirty();
            });

    // Глубина: L2/R2 (триггеры) — пропорционально нажатию
    connect(gamepad_, &Gamepad::leftTriggerMoved, this,
            [this](float v)
            {
                // v = 0..100, преобразуем в 0..10 для depth
                // L2 нажата → погружение (+)
                depth_ = (v / 100.0f) * 10.0f;
                markDirty();
            });
    connect(gamepad_, &Gamepad::rightTriggerMoved, this,
            [this](float v)
            {
                // v = 0..100, преобразуем в 0..-10 для depth
                // R2 нажата → всплытие (-)
                depth_ = -(v / 100.0f) * 10.0f;
                markDirty();
            });

    // Глубина: L2/R2 (триггеры)
    // connect(gamepad_, &Gamepad::leftTriggerMoved, this, [this](float v) {
    //     if (v > 50.0f) {
    //         depth_ = 10.f;   // L2 нажата — вниз
    //     } else {
    //         depth_ = 0.f;    // отпущена — стоп
    //     }
    //     markDirty();
    // });
    // connect(gamepad_, &Gamepad::rightTriggerMoved, this, [this](float v) {
    //     if (v > 50.0f) {
    //         depth_ = -10.f;  // R2 нажата — вверх
    //     } else {
    //         depth_ = 0.f;    // отпущена — стоп
    //     }
    //     markDirty();
    // });
}

void GamepadInputSource::markDirty() { dirty_ = true; }

std::optional<umas::input::ControlCommand> GamepadInputSource::poll()
{
    if (!dirty_)
    {
        return std::nullopt;
    }
    dirty_ = false;

    umas::input::ControlCommand cmd;
    cmd.march = march_;
    cmd.lag = lag_;
    cmd.depth = depth_;
    cmd.roll = roll_;
    cmd.pitch = pitch_;
    cmd.yaw = yaw_;
    cmd.valid = true;
    cmd.timestamp_ms = now_ms();
    return cmd;
}

bool GamepadInputSource::isAvailable() const { return gamepad_ && gamepad_->isConnected(); }
