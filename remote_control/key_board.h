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

private:
    umas::input::ControlCommand command_;
    bool dirty_ = true;
};

#endif // KEYBOARD_H
