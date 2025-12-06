#include "joy_stick.h"
#include <QTimer>
#include <QElapsedTimer>
#include <QRandomGenerator>
#include <QDebug>
#include "SFML/Window/Joystick.hpp"




JoyStick::JoyStick(QObject *parent) : RemoteControl()
{
    Q_UNUSED(parent);

    id = 0;
    int periodUpdateMsec = 80;
    int updperde = 150;

    updateTimer = new QTimer(this);
    updateTimer2 = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &JoyStick::updateImpact);
    updateTimer->start(periodUpdateMsec);
    updateTimer2->start(updperde);

    // Назначение осей и кнопок
    impactAxisMarch = sf::Joystick::Y;
    impactAxisMarch = sf::Joystick::Y;
    impactAxisDepth = sf::Joystick::Z;
    impactAxisRoll = sf::Joystick::PovX;
    impactAxisPitch = sf::Joystick::PovY;
    impactAxisYaw = sf::Joystick::R;
    impactAxisLag = sf::Joystick::X;
    impactButtonGripping = 0;
    impactButtonOpening = 5;
    impactButtonRotmanrt = 1;
    impactButtonRotmanlf = 2;
    falseDepthplus = 8;
    falseDepthminus = 9;
    powoff = 10;


}

JoyStick::~JoyStick()
{
    // Освобождение ресурсов, если потребуется
}


void JoyStick::updateImpact()
{

}
