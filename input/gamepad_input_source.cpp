#include "input/gamepad_input_source.h"

#include <chrono>

#include "Gamepad/gamepad.h"

namespace {
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
        march_ = -v / 100.f;
        markDirty();
    });
    connect(gamepad_, &Gamepad::leftStickXMoved, this, [this](float v) {
        yaw_ = v / 100.f;
        markDirty();
    });
    connect(gamepad_, &Gamepad::rightStickYMoved, this, [this](float v) {
        pitch_ = v / 100.f;
        markDirty();
    });
    connect(gamepad_, &Gamepad::rightStickXMoved, this, [this](float v) {
        lag_ = v / 100.f;
        markDirty();
    });

    // Крен: X/B
    connect(gamepad_, &Gamepad::buttonXPressed, this, [this]() { roll_delta_ = -10.f; markDirty(); });
    connect(gamepad_, &Gamepad::buttonBPressed, this, [this]() { roll_delta_ =  10.f; markDirty(); });

    // Глубина: L1/R1
    connect(gamepad_, &Gamepad::L1Pressed, this, [this]() { depth_delta_ =  10.f; markDirty(); });
    connect(gamepad_, &Gamepad::R1Pressed, this, [this]() { depth_delta_ = -10.f; markDirty(); });
}

void GamepadInputSource::markDirty()
{
    dirty_ = true;
}

std::optional<umas::input::ControlCommand> GamepadInputSource::poll()
{
    const bool hasAngularVelocity = yaw_ != 0.f || pitch_ != 0.f;
    if (!dirty_ && !hasAngularVelocity) {
        return std::nullopt;
    }
    dirty_ = false;

    umas::input::ControlCommand cmd;
    cmd.march = march_;
    cmd.lag   = lag_;
    cmd.depth = depth_delta_;
    cmd.roll  = roll_delta_;
    cmd.pitch = pitch_;
    cmd.yaw   = yaw_;
    cmd.pitch_is_velocity = true;
    cmd.yaw_is_velocity = true;
    cmd.valid = true;
    cmd.timestamp_ms = now_ms();
    depth_delta_ = 0.f;
    roll_delta_ = 0.f;
    return cmd;
}

bool GamepadInputSource::isAvailable() const
{
    return gamepad_ && gamepad_->isConnected();
}

void GamepadInputSource::reset()
{
    march_ = 0.f;
    yaw_ = 0.f;
    pitch_ = 0.f;
    roll_delta_ = 0.f;
    depth_delta_ = 0.f;
    lag_ = 0.f;
    markDirty();
}
