#pragma once

#include "input/i_input_source.h"

namespace umas::control {

class ControlService {
public:
    void apply(const umas::input::ControlCommand& command) {
        if (!command.valid) {
            return;
        }
        current_.march = command.march;
        current_.lag = command.lag;
        current_.depth += command.depth * axisScale(command.depth_is_velocity, command.timestamp_ms,
                                                    depth_velocity_timestamp_ms_);

        current_.roll += command.roll * axisScale(command.roll_is_velocity, command.timestamp_ms,
                                                  roll_velocity_timestamp_ms_);
        current_.pitch += command.pitch * axisScale(command.pitch_is_velocity, command.timestamp_ms,
                                                    pitch_velocity_timestamp_ms_);
        current_.yaw += command.yaw * axisScale(command.yaw_is_velocity, command.timestamp_ms,
                                                yaw_velocity_timestamp_ms_);
        current_.valid = true;
        current_.timestamp_ms = command.timestamp_ms;
    }

    const umas::input::ControlCommand& snapshot() const {
        return current_;
    }

    void reset() {
        current_ = {};
        current_.valid = true;
        depth_velocity_timestamp_ms_ = 0;
        roll_velocity_timestamp_ms_ = 0;
        pitch_velocity_timestamp_ms_ = 0;
        yaw_velocity_timestamp_ms_ = 0;
    }

private:
    double axisScale(bool is_velocity, std::uint64_t timestamp_ms, std::uint64_t& previous_timestamp_ms) {
        if (!is_velocity) {
            previous_timestamp_ms = 0;
            return 1.0;
        }

        if (previous_timestamp_ms == 0 || timestamp_ms <= previous_timestamp_ms) {
            previous_timestamp_ms = timestamp_ms;
            return 0.0;
        }
        const double elapsed = static_cast<double>(timestamp_ms - previous_timestamp_ms) / 1000.0;
        previous_timestamp_ms = timestamp_ms;
        return elapsed;
    }

    umas::input::ControlCommand current_{};
    std::uint64_t depth_velocity_timestamp_ms_ = 0;
    std::uint64_t roll_velocity_timestamp_ms_ = 0;
    std::uint64_t pitch_velocity_timestamp_ms_ = 0;
    std::uint64_t yaw_velocity_timestamp_ms_ = 0;
};

} // namespace umas::control
