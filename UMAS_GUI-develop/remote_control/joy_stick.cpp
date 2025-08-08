#include "joy_stick.h"
#include <QTimer>
#include <QElapsedTimer>
#include <QRandomGenerator>
#include <QDebug>
#include "SFML/Window/Joystick.hpp"

namespace {
    double baseDepth = 0.0; // базовое значение глубины
    bool fluctuationsActivated = false;
}

JoyStick::JoyStick(QObject *parent) : RemoteControl()
{
    Q_UNUSED(parent);

    id = 0;
    int periodUpdateMsec = 80;
    int updperde = 150;

    updateTimer = new QTimer(this);
    updateTimer2 = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &JoyStick::updateImpact);
    connect(updateTimer2, &QTimer::timeout, this, &JoyStick::Naebalovo);
    updateTimer->start(periodUpdateMsec);
    updateTimer2->start(updperde);

    // Назначение осей и кнопок
    impactAxisMarch = sf::Joystick::Y;
    impactAxisDepth = sf::Joystick::Z;
    impactAxisRoll = sf::Joystick::PovX;
    impactAxisPitch = sf::Joystick::PovY;
    impactAxisYaw = sf::Joystick::R;
    impactAxisLag = sf::Joystick::X;
    impactButtonGripping = 0;
    impactButtonRotman = 1;
    falseDepthplus = 8;
    falseDepthminus = 9;
    offPower = 10;

    baseDepth = currentDepth;

    fluctuationTimer.start();
}

JoyStick::~JoyStick()
{
    // Освобождение ресурсов, если потребуется
}

void JoyStick::Naebalovo()
{
    // При нажатии кнопок глубина изменяется, и базовое значение обновляется
    if (sf::Joystick::isButtonPressed(id, falseDepthplus)) {
        currentDepth += 0.1;
        baseDepth = currentDepth;
        fluctuationsActivated = true;
        qDebug() << "Глубина:" << QString::number(currentDepth, 'f', 3);
    }
    if (sf::Joystick::isButtonPressed(id, falseDepthminus)) {
        currentDepth -= 0.1;
        baseDepth = currentDepth;
        fluctuationsActivated = true;
        qDebug() << "Глубина:" << QString::number(currentDepth, 'f', 3);
    }
}

void JoyStick::updateImpact()
{
    sf::Joystick::update();

    if (!fluctuationsActivated) {
            return;
        }


    double fluctuationValue = QRandomGenerator::global()->generateDouble() * 0.20 - 0.10;
    currentDepth = baseDepth + fluctuationValue;
    fluctuationTimer.restart();
    qDebug() << "Глубина:" << QString::number(currentDepth, 'f', 3);

    if (sf::Joystick::isConnected((id))) {
        setOffPower(sf::Joystick::isButtonPressed(id, offPower));

    }

}
