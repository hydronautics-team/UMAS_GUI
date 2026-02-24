#pragma once

#include "input/i_input_source.h"

namespace umas::control {

class ControlService {
public:
    void apply(const umas::input::ControlCommand& command) {
        if (!command.valid) {
            return;
        }
        current_ = command;
    }

    const umas::input::ControlCommand& snapshot() const {
        return current_;
    }

private:
    umas::input::ControlCommand current_{};
};

} // namespace umas::control
