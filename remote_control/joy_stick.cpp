#include "joy_stick.h"

#include <chrono>
#include <QDebug>

JoyStick::JoyStick()
{
    id = -1;

    impactAxisMarch = sf::Joystick::Y;
    impactAxisDepth = sf::Joystick::Z;
    impactAxisRoll = sf::Joystick::PovX;
    impactAxisPitch = sf::Joystick::PovY;
    impactAxisYaw = sf::Joystick::R;
    impactAxisLag = sf::Joystick::X;

    sf::Joystick::update();
    selectAvailableJoystick();
}

JoyStick::~JoyStick()
{
}

std::optional<umas::input::ControlCommand> JoyStick::poll()
{
    sf::Joystick::update();
    if (id < 0 || !sf::Joystick::isConnected(id)) {
        selectAvailableJoystick();
    }

    if (id < 0 || !sf::Joystick::isConnected(id)) {
        return std::nullopt;
    }

    umas::input::ControlCommand command;
    command.march = -sf::Joystick::getAxisPosition(id, impactAxisMarch) / 1.0;
    command.lag = sf::Joystick::getAxisPosition(id, impactAxisLag) / 1.0;
    command.depth = sf::Joystick::getAxisPosition(id, impactAxisDepth) / 1.0;
    command.roll = sf::Joystick::getAxisPosition(id, impactAxisRoll) / 1.0;
    command.pitch = sf::Joystick::getAxisPosition(id, impactAxisPitch) / 1.0;
    command.yaw = sf::Joystick::getAxisPosition(id, impactAxisYaw) / 1.0;
    command.valid = true;
    command.timestamp_ms = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count());

    return command;
}

bool JoyStick::isAvailable() const
{
    return id >= 0 && sf::Joystick::isConnected(id);
}

void JoyStick::selectAvailableJoystick()
{
    for (unsigned int i = 0; i < sf::Joystick::Count; ++i) {
        if (sf::Joystick::isConnected(i)) {
            if (id != static_cast<int>(i)) {
                qInfo() << "JoyStick: подключен контроллер с id" << i;
            }
            id = static_cast<int>(i);
            return;
        }
    }

    if (id != -1) {
        qWarning() << "JoyStick: контроллер не обнаружен";
    }
    id = -1;
}

