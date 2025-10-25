#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <QObject>
#include <QDebug>

#include "remote_control.h"
#include "SFML/Window.hpp"



class JoyStick : public RemoteControl
{
    Q_OBJECT
public:
    JoyStick(QObject *parent = nullptr);
    ~JoyStick();

private slots:
    void updateImpact();
};

#endif // JOYSTICK_H
