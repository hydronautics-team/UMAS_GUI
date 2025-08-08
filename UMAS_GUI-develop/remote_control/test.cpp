#include "joy_stick.h"
#include <QDebug>

JoyStick::JoyStick(QObject *parent) : QObject(parent) {
    id = 0; // ID джойстика
    int periodUpdateMsec = 20; // Интервал обновления (20 мс)

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &JoyStick::updateImpact);
    updateTimer->start(periodUpdateMsec);
}

void JoyStick::updateImpact() {
    sf::Joystick::update(); // Обновление состояния джойстика

    // Проверка всех осей
    for (int axis = 0; axis < sf::Joystick::AxisCount; ++axis) {
        float value = sf::Joystick::getAxisPosition(id, static_cast<sf::Joystick::Axis>(axis));
        qDebug() << "Axis" << axis << ":" << value;
    }

    // Проверка всех кнопок
    for (int btn = 0; btn < sf::Joystick::getButtonCount(id); ++btn) {
        if (sf::Joystick::isButtonPressed(id, btn)) {
            qDebug() << "Кнопка" << btn << "нажата!";
        }
    }
}