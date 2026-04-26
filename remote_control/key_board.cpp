#include "key_board.h"

#include <chrono>

KeyBoard::KeyBoard()
{
    command_.valid = true;
}

KeyBoard::~KeyBoard()
{
}

std::optional<umas::input::ControlCommand> KeyBoard::poll()
{
    if (!dirty_) {
        return std::nullopt;
    }

    command_.timestamp_ms = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count());
    dirty_ = false;
    auto command = command_;
    command.depth = pending_depth_delta_;
    command.roll = pending_roll_delta_;
    command.pitch = pending_pitch_delta_;
    command.yaw = pending_yaw_delta_;
    pending_depth_delta_ = 0.0;
    pending_roll_delta_ = 0.0;
    pending_pitch_delta_ = 0.0;
    pending_yaw_delta_ = 0.0;
    return command;
}

bool KeyBoard::isAvailable() const
{
    return true;
}

void KeyBoard::reset()
{
    command_ = {};
    command_.valid = true;
    pending_depth_delta_ = 0.0;
    pending_roll_delta_ = 0.0;
    pending_pitch_delta_ = 0.0;
    pending_yaw_delta_ = 0.0;
    dirty_ = true;
}

void KeyBoard::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        return;
    }

    switch (event->key()) {
    case Qt::Key_O:
        command_.march = 10;
        break;
    case Qt::Key_L:
        command_.march = -10;
        break;
    case Qt::Key_W:
        pending_pitch_delta_ += 10;
        break;
    case Qt::Key_S:
        pending_pitch_delta_ -= 10;
        break;
    case Qt::Key_A:
        pending_yaw_delta_ -= 10;
        break;
    case Qt::Key_D:
        pending_yaw_delta_ += 10;
        break;
    case Qt::Key_C:
        pending_depth_delta_ += 10;
        break;
    case Qt::Key_V:
        pending_depth_delta_ -= 10;
        break;
    case Qt::Key_Q:
        pending_roll_delta_ += 10;
        break;
    case Qt::Key_E:
        pending_roll_delta_ -= 10;
        break;
    case Qt::Key_K:
        command_.lag = 10;
        break;
    case Qt::Key_Semicolon:
        command_.lag = -10;
        break;
    }
    dirty_ = true;
}

void KeyBoard::keyReleaseEvent(QKeyEvent *event)
{
        if (event->isAutoRepeat()) {
            return;
        }

        switch (event->key()) {
        case Qt::Key_O:
            command_.march = 0;
            break;
        case Qt::Key_L:
            command_.march = 0;
            break;
        case Qt::Key_W:
        case Qt::Key_S:
        case Qt::Key_A:
        case Qt::Key_D:
        case Qt::Key_C:
        case Qt::Key_V:
        case Qt::Key_Q:
        case Qt::Key_E:
            return;
        case Qt::Key_K:
            command_.lag = 0;
            break;
        case Qt::Key_Semicolon:
            command_.lag = 0;
            break;
        }
        dirty_ = true;
}
