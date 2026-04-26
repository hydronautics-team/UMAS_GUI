#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QKeyEvent>
#include <optional>

#include "input/i_input_source.h"

class KeyBoard : public umas::input::IInputSource
{
public:
    KeyBoard();
    ~KeyBoard() override;

    std::optional<umas::input::ControlCommand> poll() override;
    bool isAvailable() const override;

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void reset();

private:
    umas::input::ControlCommand command_;
    double pending_depth_delta_ = 0.0;
    double pending_roll_delta_ = 0.0;
    double pending_pitch_delta_ = 0.0;
    double pending_yaw_delta_ = 0.0;
    bool dirty_ = true;
};

#endif // KEYBOARD_H
