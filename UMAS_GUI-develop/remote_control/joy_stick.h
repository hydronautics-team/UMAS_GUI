#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <QObject>
#include <QDebug>
#include <QElapsedTimer>

#include "remote_control.h"
#include "SFML/Window.hpp"



class JoyStick : public RemoteControl
{
    Q_OBJECT
public:
    explicit JoyStick(QObject *parent = nullptr);  // Добавлен explicit
    ~JoyStick();

private:
    double currentDepth = 0.00;
    QElapsedTimer fluctuationTimer;
    double fluctuationValue = 0.0;



private slots:
    void updateImpact();
    void Naebalovo();
};

#endif // JOYSTICK_H
