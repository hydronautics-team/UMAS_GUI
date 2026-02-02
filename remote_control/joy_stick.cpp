#include "joy_stick.h"

JoyStick::JoyStick(QObject *parent) {
    id = 0;
    int periodUpdateMsec = 20;

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &JoyStick::updateImpact);
    updateTimer->start(periodUpdateMsec);

    // Для DEXP G-15 настраиваем оси
    impactAxisMarch = sf::Joystick::Y; // Левый стик Y (вперед/назад)
    impactAxisLag = sf::Joystick::X; // Левый стик X (влево/вправо)
    impactAxisDepth = sf::Joystick::Z; // Правый стик Y
    impactAxisYaw = sf::Joystick::R; // Правый стик X
    impactAxisRoll = sf::Joystick::PovX; // Крестовина X
    impactAxisPitch = sf::Joystick::PovY; // Крестовина Y

    // Для триггеров L2/R2 используем оси U и V
    impactAxisLeftTrigger = sf::Joystick::U;   // Триггер L2 (ось U)
    impactAxisRightTrigger = sf::Joystick::V;  // Триггер R2 (ось V)

    // Инициализация предыдущих значений осей
    prevLeftStickY = 0.0f;
    prevLeftStickX = 0.0f;
    prevRightStickY = 0.0f;
    prevRightStickX = 0.0f;
    prevLeftTrigger = 0.0f;
    prevRightTrigger = 0.0f;

    // Инициализация состояния кнопок
    prevButtonAState = false;
    prevButtonBState = false;
    prevButtonXState = false;
    prevButtonYState = false;
    prevStartButtonState = false;
    prevBackButtonState = false;

    // Для кнопок L1/R1 (обычно кнопки 4 и 5)
    prevL1State = false;
    prevR1State = false;

    // состояние крестика
    prevDPadUp = false;
    prevDPadDown = false;
    prevDPadLeft = false;
    prevDPadRight = false;
}

JoyStick::~JoyStick() {}

void JoyStick::updateImpact() {
    sf::Joystick::update();
    DataAH127C imuData = getImuData();

    if (sf::Joystick::isConnected(id)) {
        // --- ОБРАБОТКА ЛЕВОГО ДЖОЙСТИКА ---
        float leftStickX = sf::Joystick::getAxisPosition(id, sf::Joystick::X);
        float leftStickY = sf::Joystick::getAxisPosition(id, sf::Joystick::Y);
        
        const float leftDeadZone = 10.0f;
        
        // Ось X левого джойстика
        if (labs(leftStickX) > leftDeadZone) {
            emit leftStickXMoved(leftStickX);
        } else {
            emit leftStickXMoved(0.0f);
        }
        
        // Ось Y левого джойстика
        if (labs(leftStickY) > leftDeadZone) {
            emit leftStickYMoved(leftStickY);
        } else {
            emit leftStickYMoved(0.0f);
        }
        
        // Сохраняем текущие значения левого джойстика
        prevLeftStickX = leftStickX;
        prevLeftStickY = leftStickY;
        
        // --- ОБРАБОТКА ПРАВОГО ДЖОЙСТИКА ---
        float rightStickX = sf::Joystick::getAxisPosition(id, sf::Joystick::R);
        float rightStickY = sf::Joystick::getAxisPosition(id, sf::Joystick::Z);
        
        const float rightDeadZone = 15.0f;
        
        // Ось X правого джойстика
        if (labs(rightStickX) > rightDeadZone) {
            emit rightStickXMoved(rightStickX);
        } else {
            emit rightStickXMoved(0.0f);
        }
        
        // Ось Y правого джойстика
        if (labs(rightStickY) > rightDeadZone) {
            emit rightStickYMoved(rightStickY);
        } else {
            emit rightStickYMoved(0.0f);
        }
        
        // Сохраняем текущие значения правого джойстика
        prevRightStickX = rightStickX;
        prevRightStickY = rightStickY;
        
        // --- ОБРАБОТКА ТРИГГЕРОВ L2/R2 (ОСИ U И V) ---
        float leftTrigger = sf::Joystick::getAxisPosition(id, sf::Joystick::U);
        float rightTrigger = sf::Joystick::getAxisPosition(id, sf::Joystick::V);
        
        // Триггеры обычно имеют диапазон от -100 до 100, 
        // но в покое могут быть -100, а при нажатии +100 (или наоборот)
        // Приводим к диапазону 0-100 для удобства
        float normalizedLeftTrigger = (leftTrigger + 100.0f) / 2.0f;  // -100..100 -> 0..100
        float normalizedRightTrigger = (rightTrigger + 100.0f) / 2.0f;
        
        const float triggerThreshold = 30.0f;  // Порог срабатывания (30%)
        
        // Левый триггер (L2)
        if (normalizedLeftTrigger > triggerThreshold) {
            if (labs(normalizedLeftTrigger - prevLeftTrigger) > 5.0f) {
                emit leftTriggerMoved(normalizedLeftTrigger);
                qDebug() << "Триггер L2:" << normalizedLeftTrigger << "%";
            }
        } else {
            if (prevLeftTrigger > triggerThreshold) {
                emit leftTriggerMoved(0.0f);
            }
        }
        
        // Правый триггер (R2)
        if (normalizedRightTrigger > triggerThreshold) {
            if (labs(normalizedRightTrigger - prevRightTrigger) > 5.0f) {
                emit rightTriggerMoved(normalizedRightTrigger);
                qDebug() << "Триггер R2:" << normalizedRightTrigger << "%";
            }
        } else {
            if (prevRightTrigger > triggerThreshold) {
                emit rightTriggerMoved(0.0f);
            }
        }
        
        prevLeftTrigger = normalizedLeftTrigger;
        prevRightTrigger = normalizedRightTrigger;
        
        // --- ОБРАБОТКА КНОПОК L1/R1 (кнопки 4 и 5) ---
        // На DEXP G-15 L1 и R1 обычно это кнопки 4 и 5
        bool currentL1State = sf::Joystick::isButtonPressed(id, 4);
        bool currentR1State = sf::Joystick::isButtonPressed(id, 5);
        
        // Проверка состояния кнопок A/B/X/Y/Start/Back
        bool currentButtonAState = sf::Joystick::isButtonPressed(id, BUTTON_A);
        bool currentButtonBState = sf::Joystick::isButtonPressed(id, BUTTON_B);
        bool currentButtonXState = sf::Joystick::isButtonPressed(id, BUTTON_X);
        bool currentButtonYState = sf::Joystick::isButtonPressed(id, BUTTON_Y);
        bool currentStartButtonState = sf::Joystick::isButtonPressed(id, START_BUTTON);
        bool currentBackButtonState = sf::Joystick::isButtonPressed(id, BACK_BUTTON);

        // Обработка кнопки A
        if (currentButtonAState && !prevButtonAState) {
            emit buttonAPressed();
            qDebug() << "Кнопка A нажата на геймпаде";
        }

        // Обработка кнопки B
        if (currentButtonBState && !prevButtonBState) {
            emit buttonBPressed();
            qDebug() << "Кнопка B нажата на геймпаде";
        }

        // Обработка кнопки X
        if (currentButtonXState && !prevButtonXState) {
            emit buttonXPressed();
            qDebug() << "Кнопка X нажата на геймпаде";
        }

        // Обработка кнопки Y
        if (currentButtonYState && !prevButtonYState) {
            emit buttonYPressed();
            qDebug() << "Кнопка Y нажата на геймпаде";
        }

        // Обработка кнопки Start
        if (currentStartButtonState && !prevStartButtonState) {
            emit startButtonPressed();
            qDebug() << "Кнопка Start нажата на геймпаде";
        }

        // Обработка кнопки Back
        if (currentBackButtonState && !prevBackButtonState) {
            emit backButtonPressed();
            qDebug() << "Кнопка Back нажата на геймпаде";
        }

        // Обработка кнопки L1
        if (currentL1State && !prevL1State) {
            emit L1Pressed();
            qDebug() << "Кнопка L1 нажата";
        }

        // Обработка кнопки R1
        if (currentR1State && !prevR1State) {
            emit R1Pressed();
            qDebug() << "Кнопка R1 нажата";
        }

        // Сохраняем текущее состояние кнопок для следующего цикла
        prevButtonAState = currentButtonAState;
        prevButtonBState = currentButtonBState;
        prevButtonXState = currentButtonXState;
        prevButtonYState = currentButtonYState;
        prevStartButtonState = currentStartButtonState;
        prevBackButtonState = currentBackButtonState;
        prevL1State = currentL1State;
        prevR1State = currentR1State;

        // --- ОБРАБОТКА КРЕСТИКА (D-Pad) ---
        float dPadX = sf::Joystick::getAxisPosition(id, sf::Joystick::PovX);
        float dPadY = sf::Joystick::getAxisPosition(id, sf::Joystick::PovY);

        const float dPadThreshold = 80.0f;

        bool currentDPadUp = (dPadY <= -dPadThreshold);
        bool currentDPadDown = (dPadY >= dPadThreshold);
        bool currentDPadLeft = (dPadX <= -dPadThreshold);
        bool currentDPadRight = (dPadX >= dPadThreshold);

        if (currentDPadUp && !prevDPadUp) {
            emit dPadUpPressed();
            qDebug() << "Крестик: Вверх нажата";
        }

        if (currentDPadDown && !prevDPadDown) {
            emit dPadDownPressed();
            qDebug() << "Крестик: Вниз нажата";
        }

        if (currentDPadLeft && !prevDPadLeft) {
            emit dPadLeftPressed();
            qDebug() << "Крестик: Влево нажата";
        }

        if (currentDPadRight && !prevDPadRight) {
            emit dPadRightPressed();
            qDebug() << "Крестик: Вправо нажата";
        }

        // Сохраняем текущее состояние крестика для следующего цикла
        prevDPadUp = currentDPadUp;
        prevDPadDown = currentDPadDown;
        prevDPadLeft = currentDPadLeft;
        prevDPadRight = currentDPadRight;
    }
}

QPair<float, float> JoyStick::getLeftStickValues() const {
    if (sf::Joystick::isConnected(id)) {
        float y = sf::Joystick::getAxisPosition(id, sf::Joystick::Y);
        float x = sf::Joystick::getAxisPosition(id, sf::Joystick::X);
        return QPair<float, float>(x, y);
    }
    return QPair<float, float>(0.0f, 0.0f);
}

QPair<float, float> JoyStick::getRightStickValues() const {
    if (sf::Joystick::isConnected(id)) {
        float x = sf::Joystick::getAxisPosition(id, sf::Joystick::R);
        float y = sf::Joystick::getAxisPosition(id, sf::Joystick::Z);
        return QPair<float, float>(x, y);
    }
    return QPair<float, float>(0.0f, 0.0f);
}

// Метод для проверки подключения джойстика
bool JoyStick::isConnected() const {
    return sf::Joystick::isConnected(id);
}

// Метод для получения значений триггеров
QPair<float, float> JoyStick::getTriggerValues() const {
    if (sf::Joystick::isConnected(id)) {
        float left = (sf::Joystick::getAxisPosition(id, sf::Joystick::U) + 100.0f) / 2.0f;
        float right = (sf::Joystick::getAxisPosition(id, sf::Joystick::V) + 100.0f) / 2.0f;
        return QPair<float, float>(left, right);
    }
    return QPair<float, float>(0.0f, 0.0f);
}