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
    return command_;
}

bool KeyBoard::isAvailable() const
{
    return true;
}

void KeyBoard::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_O:
        command_.march = 10;
        break;
    case Qt::Key_L:
        command_.march = -10;
        break;
    case Qt::Key_W:
        command_.pitch = 10;
        break;
    case Qt::Key_S:
        command_.pitch = -10;
        break;
    case Qt::Key_A:
        command_.yaw = -10;
        break;
    case Qt::Key_D:
        command_.yaw = 10;
        break;
    case Qt::Key_C:
        command_.depth = 10;
        break;
    case Qt::Key_V:
        command_.depth = -10;
        break;
    case Qt::Key_Q:
        command_.roll = 10;
        break;
    case Qt::Key_E:
        command_.roll = -10;
        break;
    case Qt::Key_K:
        command_.lag = -10;
        break;
    case Qt::Key_Semicolon:
        command_.lag = 10;
        break;
    }
    dirty_ = true;
}

void KeyBoard::keyReleaseEvent(QKeyEvent *event)
{
        switch (event->key()) {
        case Qt::Key_O:
            command_.march = 0;
            break;
        case Qt::Key_L:
            command_.march = 0;
            break;
        case Qt::Key_W:
            command_.pitch = 0;
            break;
        case Qt::Key_S:
            command_.pitch = 0;
            break;
        case Qt::Key_A:
            command_.yaw = 0;
            break;
        case Qt::Key_D:
            command_.yaw = 0;
            break;
        case Qt::Key_C:
            command_.depth = 0;
            break;
        case Qt::Key_V:
            command_.depth = 0;
            break;
        case Qt::Key_Q:
            command_.roll = 0;
            break;
        case Qt::Key_E:
            command_.roll = 0;
            break;
        case Qt::Key_K:
            command_.lag = 0;
            break;
        case Qt::Key_Semicolon:
            command_.lag = 0;
            break;
        }
        dirty_ = true;
}
