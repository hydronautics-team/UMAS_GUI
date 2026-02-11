#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "SFML/Window.hpp"
#include "input/i_input_source.h"

class JoyStick : public umas::input::IInputSource
{
public:
    JoyStick();
    ~JoyStick() override;

    std::optional<umas::input::ControlCommand> poll() override;
    bool isAvailable() const override;

private:
    int id;
    sf::Joystick::Axis impactAxisMarch;
    sf::Joystick::Axis impactAxisDepth;
    sf::Joystick::Axis impactAxisRoll;
    sf::Joystick::Axis impactAxisPitch;
    sf::Joystick::Axis impactAxisYaw;
    sf::Joystick::Axis impactAxisLag;
};

#endif // JOYSTICK_H
