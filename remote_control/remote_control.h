#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QObject>

#include "SFML/Window.hpp"

class RemoteControl : public QObject
{
    Q_OBJECT
public:
    explicit RemoteControl(int joy_id = 0, int update_time = 0, QObject *parent = 0);
    ~RemoteControl();

    int id;
    int periodUpdateMsec;

    float getMarch();
    float getDepth();
    float getRoll();
    float getPitch();
    float getYaw();

    sf::Joystick::Axis impactAxisMarch;
    sf::Joystick::Axis impactAxisDepth;
    sf::Joystick::Axis impactAxisRoll;
    sf::Joystick::Axis impactAxisPitch;
    sf::Joystick::Axis impactAxisYaw;

signals:

protected:


};

#endif // REMOTECONTROL_H
