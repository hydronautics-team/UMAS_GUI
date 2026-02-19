#include "joy_stick.h"

#include <chrono>

JoyStick::JoyStick()
{
    id = 0;

    impactAxisMarch = sf::Joystick::Y;
    impactAxisDepth = sf::Joystick::Z;
    impactAxisRoll = sf::Joystick::PovX;
    impactAxisPitch = sf::Joystick::PovY;
    impactAxisYaw = sf::Joystick::R;
    impactAxisLag = sf::Joystick::X;
}

JoyStick::~JoyStick()
{
}

std::optional<umas::input::ControlCommand> JoyStick::poll()
{
    sf::Joystick::update();
    if (!sf::Joystick::isConnected(id)) {
        return std::nullopt;
    }

    umas::input::ControlCommand command;
    command.march = -sf::Joystick::getAxisPosition(id, impactAxisMarch) / 2.0;
    command.lag = sf::Joystick::getAxisPosition(id, impactAxisLag) / 2.0;
    command.depth = 0.0;
    command.roll = sf::Joystick::getAxisPosition(id, impactAxisRoll) / 4.0;
    command.pitch = sf::Joystick::getAxisPosition(id, impactAxisPitch) / 10.0;
    command.yaw = sf::Joystick::getAxisPosition(id, impactAxisYaw) / 4.0;
    command.valid = true;
    command.timestamp_ms = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count());

    return command;
}

bool JoyStick::isAvailable() const
{
    return sf::Joystick::isConnected(id);
}

