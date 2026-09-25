#include "input/gamepad_input_source.h"

#include <chrono>
#include <cmath>  
#include "Gamepad/gamepad.h"

namespace {
    constexpr float STICK_DEADZONE = 15.0f;
std::uint64_t now_ms()
{
    return static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count());
}
}

GamepadInputSource::GamepadInputSource(Gamepad* gamepad, QObject* parent)
    : QObject(parent)
    , gamepad_(gamepad)
{
    // Стики: -100..100 -> -1..1
    connect(gamepad_, &Gamepad::leftStickYMoved, this, [this](float v) {
        if (std::abs(v) < STICK_DEADZONE) v = 0.0f;
        march_ = -v / 100.f;
        markDirty();
    });
    connect(gamepad_, &Gamepad::leftStickXMoved, this, [this](float v) {
        if (std::abs(v) < STICK_DEADZONE) v = 0.0f;
        yaw_ = v / 100.f;
        markDirty();
    });
    connect(gamepad_, &Gamepad::rightStickYMoved, this, [this](float v) {
        if (std::abs(v) < STICK_DEADZONE) v = 0.0f;
        pitch_ = v / 100.f;
        markDirty();
    });
    connect(gamepad_, &Gamepad::rightStickXMoved, this, [this](float v) {
        if (std::abs(v) < STICK_DEADZONE) v = 0.0f;
        lag_ = v / 100.f;
        markDirty();
    });

    // Крен: X/B
    connect(gamepad_, &Gamepad::buttonXPressed,  this, [this]() { roll_ = -10.f; markDirty(); });
    connect(gamepad_, &Gamepad::buttonXReleased, this, [this]() { roll_ =  0.f;  markDirty(); });
    connect(gamepad_, &Gamepad::buttonBPressed,  this, [this]() { roll_ =  10.f; markDirty(); });
    connect(gamepad_, &Gamepad::buttonBReleased, this, [this]() { roll_ =  0.f;  markDirty(); });



    // Глубина: L2/R2 (триггеры)
connect(gamepad_, &Gamepad::leftTriggerMoved, this, [this](float v) {
    if (v > 50.0f) {
        depth_ = 10.f;   // L2 нажата — вниз
    } else {
        depth_ = 0.f;    // отпущена — стоп
    }
    markDirty();
});
connect(gamepad_, &Gamepad::rightTriggerMoved, this, [this](float v) {
    if (v > 50.0f) {
        depth_ = -10.f;  // R2 нажата — вверх
    } else {
        depth_ = 0.f;    // отпущена — стоп
    }
    markDirty();
});
 
}

void GamepadInputSource::markDirty()
{
    dirty_ = true;
}

std::optional<umas::input::ControlCommand> GamepadInputSource::poll()
{
    if (!dirty_) {
        return std::nullopt;
    }
    dirty_ = false;

    umas::input::ControlCommand cmd;
    cmd.march = march_;
    cmd.lag   = lag_;
    cmd.depth = depth_;
    cmd.roll  = roll_;
    cmd.pitch = pitch_;
    cmd.yaw   = yaw_;
    cmd.valid = true;
    cmd.timestamp_ms = now_ms();
    return cmd;
}

bool GamepadInputSource::isAvailable() const
{
    return gamepad_ && gamepad_->isConnected();
}
