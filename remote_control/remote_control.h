#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QObject>
#include "i_control_data.h"
#include <QTimer>
#include <QDebug>


#include "SFML/Window.hpp"

class RemoteControl : public IControlData
{
    Q_OBJECT
public:
    explicit RemoteControl();
    ~RemoteControl();

    int id;

public slots:
    void updateImpact();

signals:

protected:
    sf::Joystick::Axis impactAxisMarch;
    sf::Joystick::Axis impactAxisDepth;
    sf::Joystick::Axis impactAxisRoll;
    sf::Joystick::Axis impactAxisPitch;
    sf::Joystick::Axis impactAxisYaw;

private:
    QTimer *updateTimer;
};

#endif // REMOTECONTROL_H
