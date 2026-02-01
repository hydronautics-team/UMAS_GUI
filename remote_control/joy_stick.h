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

signals:  // Добавляем сигналы
    void buttonAPressed();
    void buttonBPressed();
    void buttonXPressed();
    void buttonYPressed();
    void startButtonPressed();
    void backButtonPressed();

private slots:
    void updateImpact();

private:  // Добавляем состояние кнопок для отслеживания нажатий
    bool prevButtonAState;
    bool prevButtonBState;
    bool prevButtonXState;
    bool prevButtonYState;
    bool prevStartButtonState;
    bool prevBackButtonState;
    
    // Идентификаторы кнопок (могут варьироваться в зависимости от геймпада)
    const unsigned int BUTTON_A = 0;     // Кнопка A (обычно кнопка 0)
    const unsigned int BUTTON_B = 1;     // Кнопка B (обычно кнопка 1)
    const unsigned int BUTTON_X = 2;     // Кнопка X (обычно кнопка 2)
    const unsigned int BUTTON_Y = 3;     // Кнопка Y (обычно кнопка 3)
    const unsigned int START_BUTTON = 7; // Кнопка Start (обычно кнопка 7)
    const unsigned int BACK_BUTTON = 6;  // Кнопка Back (обычно кнопка 6)
};

#endif // JOYSTICK_H