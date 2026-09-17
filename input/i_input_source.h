#pragma once

#include <cstdint>
#include <optional>

namespace umas::input {

struct ControlCommand {
    double march = 0.0;
    double lag = 0.0;
    double depth = 0.0;
    double roll = 0.0;
    double pitch = 0.0;
    double yaw = 0.0;

    bool valid = false;
    std::uint64_t timestamp_ms = 0;
};

class IInputSource {
public:
    virtual ~IInputSource() = default;
    virtual std::optional<ControlCommand> poll() = 0;
    virtual bool isAvailable() const = 0;
};

} // namespace umas::input
