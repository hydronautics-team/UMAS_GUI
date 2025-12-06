#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <QObject>
#include <QDebug>
#include "remote_control.h"
#include "SFML/Window.hpp"

class GamePad : public RemoteControl
{
    Q_OBJECT
public:
    GamePad(QObject *parent = nullptr);
    ~GamePad();

private slots:
    void updateImpact();

private:
    unsigned int impactButtonGripping;
    unsigned int impactButtonOpening;
    unsigned int impactButtonRotmanrt;
    unsigned int impactButtonRotmanlf;
    unsigned int offPower;

    sf::Joystick::Axis impactAxisR2; // триггер вперед
    sf::Joystick::Axis impactAxisL2; // триггер назад
};

#endif // GAMEPAD_H