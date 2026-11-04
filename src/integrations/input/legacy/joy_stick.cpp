#include "joy_stick.h"

#include "axis_response.h"
#include <QDebug>
#include <chrono>

namespace
{
constexpr double STICK_RANGE = 100.0;
constexpr double STICK_DEADZONE = 5.0;
constexpr double STICK_EXPO = 0.65;
}

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

JoyStick::~JoyStick() {}

std::optional<umas::input::ControlCommand> JoyStick::poll()
{
    sf::Joystick::update();
    if (id < 0 || !sf::Joystick::isConnected(id))
    {
        selectAvailableJoystick();
    }

    if (id < 0 || !sf::Joystick::isConnected(id))
    {
        return std::nullopt;
    }

    umas::input::ControlCommand command;
    command.march = -umas::input::shapeStickAxis(
        sf::Joystick::getAxisPosition(id, impactAxisMarch), STICK_RANGE, STICK_DEADZONE,
        STICK_EXPO);
    command.lag = umas::input::shapeStickAxis(sf::Joystick::getAxisPosition(id, impactAxisLag),
                                               STICK_RANGE, STICK_DEADZONE, STICK_EXPO);
    command.depth = umas::input::shapeStickAxis(sf::Joystick::getAxisPosition(id, impactAxisDepth),
                                                 STICK_RANGE, STICK_DEADZONE, STICK_EXPO);
    command.roll = umas::input::shapeStickAxis(sf::Joystick::getAxisPosition(id, impactAxisRoll),
                                                STICK_RANGE, STICK_DEADZONE, STICK_EXPO);
    command.pitch = umas::input::shapeStickAxis(sf::Joystick::getAxisPosition(id, impactAxisPitch),
                                                 STICK_RANGE, STICK_DEADZONE, STICK_EXPO);
    command.yaw = umas::input::shapeStickAxis(sf::Joystick::getAxisPosition(id, impactAxisYaw),
                                               STICK_RANGE, STICK_DEADZONE, STICK_EXPO);
    command.valid = true;
    command.timestamp_ms =
        static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                                       std::chrono::steady_clock::now().time_since_epoch())
                                       .count());

    return command;
}

bool JoyStick::isAvailable() const { return id >= 0 && sf::Joystick::isConnected(id); }

void JoyStick::selectAvailableJoystick()
{
    for (unsigned int i = 0; i < sf::Joystick::Count; ++i)
    {
        if (sf::Joystick::isConnected(i))
        {
            if (id != static_cast<int>(i))
            {
                qInfo() << "JoyStick: подключен контроллер с id" << i;
            }
            id = static_cast<int>(i);
            return;
        }
    }

    if (id != -1)
    {
        qWarning() << "JoyStick: контроллер не обнаружен";
    }
    id = -1;
}
