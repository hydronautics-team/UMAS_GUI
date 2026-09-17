#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <QObject>
#include <QTimer>
#include <QPair>
#include <QSocketNotifier>
#include <linux/joystick.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmath>
#include <algorithm>

class Gamepad : public QObject
{
    Q_OBJECT

public:
    explicit Gamepad(int id = 0, QObject *parent = nullptr);
    ~Gamepad();

    bool isConnected() const;
    QPair<float, float> getLeftStickValues() const;
    QPair<float, float> getRightStickValues() const;

    QPair<float, float> getTriggerValues() const;

signals:
    // Левый стик (оси X и Y)
    void leftStickXMoved(float value);   // курс
    void leftStickYMoved(float value);   // марш

    // Правый стик
    void rightStickXMoved(float value);
    void rightStickYMoved(float value);

    // Триггеры (значения 0..100)
    void leftTriggerMoved(float value);
    void rightTriggerMoved(float value);

    // Существующие сигналы нажатия
    void buttonAPressed();
    void buttonBPressed();
    void buttonXPressed();
    void buttonYPressed();
    void startButtonPressed();
    void backButtonPressed();
    void L1Pressed();
    void R1Pressed();
    void dPadUpPressed();
    void dPadDownPressed();
    void dPadLeftPressed();
    void dPadRightPressed();

    // Новые сигналы отпускания
    void buttonAReleased();
    void buttonBReleased();
    void buttonXReleased();
    void buttonYReleased();
    void startButtonReleased();
    void backButtonReleased();
    void L1Released();
    void R1Released();
    void dPadUpReleased();
    void dPadDownReleased();
    void dPadLeftReleased();
    void dPadRightReleased();

    // Если нужны L3/R3
    void L3Pressed();
    void L3Released();
    void R3Pressed();
    void R3Released();




private slots:
    void onJoystickEvent();   // слот для чтения событий

private:
    void calibrateTriggers();
    float normalizeAxis(int value) const;   // -32767..32767 -> -100..100
    int m_fd;                                // файловый дескриптор устройства
    QSocketNotifier *m_notifier;             // уведомитель о появлении данных
    bool m_connected;

    // Индексы осей (предполагаем стандартный Xbox-подобный геймпад)
    // Возможно, потребуется настройка под конкретный геймпад.
    enum AxisIndex {
        AxisLeftStickX = 0,
        AxisLeftStickY = 1,
        AxisLeftTrigger = 2,      // иногда L2
        AxisRightStickX = 3,
        AxisRightStickY = 4,
        AxisRightTrigger = 5,     // иногда R2
        AxisDPadX = 6,             // крестовина по горизонтали
        AxisDPadY = 7              // крестовина по вертикали
    };

    // Индексы кнопок (стандартный маппинг Xbox)
    enum ButtonIndex {
        ButtonA = 0,
        ButtonB = 1,
        ButtonX = 2,
        ButtonY = 3,
        ButtonL1 = 4,
        ButtonR1 = 5,
        ButtonBack = 6,
        ButtonStart = 7,
        ButtonL3 = 9,              // нажатие левого стика
        ButtonR3 = 10               // нажатие правого стика
    };

    // Текущие состояния
    float m_axisValues[8];          // значения осей в диапазоне -100..100 (для триггеров 0..100)
    bool m_buttonStates[11];         // состояния кнопок

    // Для калибровки триггеров
    float m_triggerLeftZero;
    float m_triggerRightZero;
    bool m_triggersCalibrated;
    int m_calibrationFrames;        // счётчик пропущенных кадров при калибровке
    static constexpr int CALIBRATION_SKIP = 3;   // сколько событий пропустить перед калибровкой

    bool m_prevDPadUp;
bool m_prevDPadDown;
bool m_prevDPadLeft;
bool m_prevDPadRight;
};

#endif // GAMEPAD_H