#pragma once

#include <QObject>
#include <optional>

#include "input/i_input_source.h"

class Gamepad;

// Адаптер Gamepad (Qt signals) -> единый интерфейс IInputSource.
// Идея: Gamepad асинхронно обновляет оси/кнопки, а poll() отдаёт снимок как ControlCommand.
class GamepadInputSource final : public QObject, public umas::input::IInputSource
{
    Q_OBJECT

public:
    explicit GamepadInputSource(Gamepad* gamepad, QObject* parent = nullptr);
    ~GamepadInputSource() override = default;

    std::optional<umas::input::ControlCommand> poll() override;
    bool isAvailable() const override;
    void reset();

private:
    void markDirty();

    Gamepad* gamepad_ = nullptr; // не владеем

    // Текущее состояние (нормировано примерно к -1..1 для стиков и к шагам для кнопок).
    float march_ = 0.f; // surge
    float yaw_   = 0.f;
    float pitch_ = 0.f;
    float roll_delta_ = 0.f;
    float depth_delta_ = 0.f;
    float lag_   = 0.f;

    bool dirty_ = true;
};
