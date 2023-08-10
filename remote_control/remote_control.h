#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QObject>
#include "i_control_data.h"
#include <QTimer>
#include <QDebug>


#include "SFML/Window.hpp"

class RemoteControl : public QObject
{
    Q_OBJECT
public:
    explicit RemoteControl(int joy_id = 0, int update_time = 0, QObject *parent = 0);
    ~RemoteControl();

    int id;
    int periodUpdateMsec;

    void setMarch();
    void setDepth();
    void setRoll(float roll);
    void setPitch(float pitch);
    void setYaw(float yaw);

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

    IControlData interface;
    QTimer *updateTimer;

};

#endif // REMOTECONTROL_H
