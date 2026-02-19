#include "gamepad.h"
#include <QDebug>
#include <QCoreApplication>
#include <errno.h>
#include <string.h>

Gamepad::Gamepad(int id, QObject *parent)
    : QObject(parent)
    , m_fd(-1)
    , m_notifier(nullptr)
    , m_connected(false)
    , m_triggerLeftZero(0.0f)
    , m_triggerRightZero(0.0f)
    , m_triggersCalibrated(false)
    , m_calibrationFrames(0)
    , m_prevDPadUp(false)
    , m_prevDPadDown(false)
    , m_prevDPadLeft(false)
    , m_prevDPadRight(false)
{
    // Обнуляем массивы состояний
    for (int i = 0; i < 8; ++i)
        m_axisValues[i] = 0.0f;
    for (int i = 0; i < 11; ++i)
        m_buttonStates[i] = false;

    // Открываем устройство /dev/input/js<id>
    QString devicePath = QString("/dev/input/js%1").arg(id);
    m_fd = open(devicePath.toLocal8Bit().constData(), O_RDONLY | O_NONBLOCK);
    if (m_fd < 0) {
        qWarning() << "Не удалось открыть устройство" << devicePath << ":" << strerror(errno);
        m_connected = false;
        return;
    }

    m_connected = true;
    qDebug() << "Геймпад подключён:" << devicePath;

    // Создаём сокетный уведомитель для асинхронного чтения
    m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, &Gamepad::onJoystickEvent);
}

Gamepad::~Gamepad()
{
    if (m_notifier) {
        m_notifier->setEnabled(false);
        delete m_notifier;
    }
    if (m_fd >= 0) {
        close(m_fd);
    }
}

bool Gamepad::isConnected() const
{
    return m_connected && m_fd >= 0;
}

void Gamepad::onJoystickEvent()
{
    struct js_event e;
    int bytesRead = read(m_fd, &e, sizeof(e));
    if (bytesRead != sizeof(e)) {
        if (bytesRead < 0 && errno != EAGAIN) {
            qWarning() << "Ошибка чтения из джойстика:" << strerror(errno);
            // Возможно, устройство отключено
            m_connected = false;
            m_notifier->setEnabled(false);
            close(m_fd);
            m_fd = -1;
        }
        return;
    }

    // Обработка события
    if (e.type & JS_EVENT_INIT) {
        // Событие инициализации – можно игнорировать или использовать для калибровки
        return;
    }

    // Калибровка триггеров (ещё не откалибровано)
    if (!m_triggersCalibrated) {
        if (m_calibrationFrames < CALIBRATION_SKIP) {
            m_calibrationFrames++;
            return;  // пропускаем первые несколько событий для стабилизации
        }
        // Запоминаем нулевые положения осей триггеров
        m_triggerLeftZero = m_axisValues[AxisLeftTrigger];
        m_triggerRightZero = m_axisValues[AxisRightTrigger];
        m_triggersCalibrated = true;
        qDebug() << "Триггеры откалиброваны. L2 zero =" << m_triggerLeftZero
                 << "R2 zero =" << m_triggerRightZero;
        return;
    }

    // Обработка оси
    if (e.type & JS_EVENT_AXIS) {
        int axis = e.number;
        float normalized = normalizeAxis(e.value);  // -100..100

        if (axis < 8) {
            m_axisValues[axis] = normalized;

            // Определяем, какая ось изменилась, и испускаем соответствующий сигнал
            switch (axis) {
            case AxisLeftStickX:
                emit leftStickXMoved(normalized);
                break;
            case AxisLeftStickY:
                emit leftStickYMoved(normalized);
                break;
            case AxisRightStickX:
                emit rightStickXMoved(normalized);
                break;
            case AxisRightStickY:
                emit rightStickYMoved(normalized);
                break;
            case AxisLeftTrigger:
                {
                    // Для триггера после калибровки преобразуем в 0..100
                    float trigger = normalized - m_triggerLeftZero;
                    trigger = std::clamp(trigger * 0.5f, 0.0f, 100.0f);
                    emit leftTriggerMoved(trigger);
                }
                break;
            case AxisRightTrigger:
                {
                    float trigger = normalized - m_triggerRightZero;
                    trigger = std::clamp(trigger * 0.5f, 0.0f, 100.0f);
                    emit rightTriggerMoved(trigger);
                }
                break;
            case AxisDPadX:
                {
                    bool left = normalized <= -80.0f;
                    bool right = normalized >= 80.0f;
                    if (left != m_prevDPadLeft) {
                        m_prevDPadLeft = left;
                        if (left)
                            emit dPadLeftPressed();
                        else
                            emit dPadLeftReleased();
                    }
                    if (right != m_prevDPadRight) {
                        m_prevDPadRight = right;
                        if (right)
                            emit dPadRightPressed();
                        else
                            emit dPadRightReleased();
                    }
                }
                break;
            case AxisDPadY:
                {
                    bool up = normalized <= -80.0f;
                    bool down = normalized >= 80.0f;
                    if (up != m_prevDPadUp) {
                        m_prevDPadUp = up;
                        if (up)
                            emit dPadUpPressed();
                        else
                            emit dPadUpReleased();
                    }
                    if (down != m_prevDPadDown) {
                        m_prevDPadDown = down;
                        if (down)
                            emit dPadDownPressed();
                        else
                            emit dPadDownReleased();
                    }
                }
                break;
            default:
                break;
            }
        }
    }
    // Обработка кнопки
    else if (e.type & JS_EVENT_BUTTON) {
        int btn = e.number;
        bool pressed = e.value;  // 1 – нажата, 0 – отпущена

        if (btn < 11) {
            bool old = m_buttonStates[btn];
            if (pressed != old) {
                m_buttonStates[btn] = pressed;
                if (pressed) {
                    switch (btn) {
                    case ButtonA: emit buttonAPressed(); break;
                    case ButtonB: emit buttonBPressed(); break;
                    case ButtonX: emit buttonXPressed(); break;
                    case ButtonY: emit buttonYPressed(); break;
                    case ButtonStart: emit startButtonPressed(); break;
                    case ButtonBack: emit backButtonPressed(); break;
                    case ButtonL1: emit L1Pressed(); break;
                    case ButtonR1: emit R1Pressed(); break;
                    case ButtonL3: emit L3Pressed(); break;
                    case ButtonR3: emit R3Pressed(); break;
                    default: break;
                    }
                } else {
                    switch (btn) {
                    case ButtonA: emit buttonAReleased(); break;
                    case ButtonB: emit buttonBReleased(); break;
                    case ButtonX: emit buttonXReleased(); break;
                    case ButtonY: emit buttonYReleased(); break;
                    case ButtonStart: emit startButtonReleased(); break;
                    case ButtonBack: emit backButtonReleased(); break;
                    case ButtonL1: emit L1Released(); break;
                    case ButtonR1: emit R1Released(); break;
                    case ButtonL3: emit L3Released(); break;
                    case ButtonR3: emit R3Released(); break;
                    default: break;
                    }
                }
            }
        }
    }
}

float Gamepad::normalizeAxis(int value) const
{
    // Значение от -32767 до 32767 преобразуем в -100..100
    return static_cast<float>(value) * 100.0f / 32767.0f;
}

void Gamepad::calibrateTriggers()
{
    // Калибровка выполняется автоматически в onJoystickEvent после пропуска событий
}

QPair<float, float> Gamepad::getLeftStickValues() const
{
    return qMakePair(m_axisValues[AxisLeftStickX], m_axisValues[AxisLeftStickY]);
}

QPair<float, float> Gamepad::getRightStickValues() const
{
    return qMakePair(m_axisValues[AxisRightStickX], m_axisValues[AxisRightStickY]);
}

QPair<float, float> Gamepad::getTriggerValues() const
{
    if (!m_triggersCalibrated)
        return qMakePair(0.0f, 0.0f);

    float left = m_axisValues[AxisLeftTrigger] - m_triggerLeftZero;
    float right = m_axisValues[AxisRightTrigger] - m_triggerRightZero;
    left = std::clamp(left * 0.5f, 0.0f, 100.0f);
    right = std::clamp(right * 0.5f, 0.0f, 100.0f);
    return qMakePair(left, right);
}