#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QObject>
#include "i_control_data.h"
#include <QTimer>
#include <QDebug>

#include "SFML/Window.hpp"
//#include "joy_stick.h"
//#include "key_board.h"

class RemoteControl : public IControlData
{
    Q_OBJECT
public:
    explicit RemoteControl();
    ~RemoteControl();

    int     id;
    QTimer *updateTimer;

protected:
    sf::Joystick::Axis impactAxisMarch;
    sf::Joystick::Axis impactAxisDepth;
    sf::Joystick::Axis impactAxisRoll;
    sf::Joystick::Axis impactAxisPitch;
    sf::Joystick::Axis impactAxisYaw;

};

#endif // REMOTECONTROL_H
