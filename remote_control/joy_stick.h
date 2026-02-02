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

signals:
    // Сигналы для осей джойстика
    void leftStickXMoved(float value);   // Левый стик: влево/вправо
    void leftStickYMoved(float value);   // Левый стик: вперед/назад
    void rightStickXMoved(float value);  // Правый стик: горизонталь
    void rightStickYMoved(float value);  // Правый стик: вертикаль
    
    // Сигналы для кнопок
    void buttonAPressed();      // Кнопка A
    void buttonBPressed();      // Кнопка B
    void buttonXPressed();      // Кнопка X
    void buttonYPressed();      // Кнопка Y
    void startButtonPressed();  // Кнопка Start
    void backButtonPressed();   // Кнопка Back



    void buttonXReleased();
    void buttonBReleased();
    
    // Сигналы для триггеров
    void leftTriggerMoved(float value);  // L2 (аналоговый триггер)
    void rightTriggerMoved(float value); // R2 (аналоговый триггер)
    void L1Pressed();                    // L1 (кнопка)
    void R1Pressed();                    // R1 (кнопка)
    
    // Сигналы для крестика (D-Pad)
    void dPadUpPressed();      // Крестик: вверх
    void dPadDownPressed();    // Крестик: вниз
    void dPadLeftPressed();    // Крестик: влево
    void dPadRightPressed();   // Крестик: вправо

private slots:
    void updateImpact();

public:
    QPair<float, float> getLeftStickValues() const;
    QPair<float, float> getRightStickValues() const;
    QPair<float, float> getTriggerValues() const;
    bool isConnected() const;

private:
    // Идентификаторы кнопок для DEXP G-15
    const unsigned int BUTTON_A = 0;     // Кнопка A
    const unsigned int BUTTON_B = 1;     // Кнопка B
    const unsigned int BUTTON_X = 2;     // Кнопка X
    const unsigned int BUTTON_Y = 3;     // Кнопка Y
    const unsigned int START_BUTTON = 7; // Кнопка Start
    const unsigned int BACK_BUTTON = 6;  // Кнопка Back

    // Состояние предыдущих нажатий кнопок
    bool prevButtonAState;
    bool prevButtonBState;
    bool prevButtonXState;
    bool prevButtonYState;
    bool prevStartButtonState;
    bool prevBackButtonState;
    
    // Состояние кнопок L1/R1
    bool prevL1State;
    bool prevR1State;
    
    // Состояние крестика (D-Pad)
    bool prevDPadUp;
    bool prevDPadDown;
    bool prevDPadLeft;
    bool prevDPadRight;

    // Предыдущие значения осей джойстиков
    float prevLeftStickY;
    float prevLeftStickX;
    float prevRightStickY;
    float prevRightStickX;
    
    // Предыдущие значения триггеров (оси)
    float prevLeftTrigger;
    float prevRightTrigger;
    
    // Оси для триггеров
    sf::Joystick::Axis impactAxisLeftTrigger;
    sf::Joystick::Axis impactAxisRightTrigger;
};

#endif // JOYSTICK_H