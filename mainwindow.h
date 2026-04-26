#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include <QMainWindow>
#include <QTimer>
#include <QDebug>
#include <QProcess>
#include <QTime>
#include <QButtonGroup>

#include <QString>

// Legacy include'ы удалены.

#include <QKeyEvent>
#include <cstdint>
#include <memory>

#include "joy_stick.h"
#include "key_board.h"
#include "input/i_input_source.h"
#include "input/gamepad_input_source.h"
#include "control/control_service.h"
// #include "power_system.h"
// #include "mode_automatic.h"
// #include "map_widget.h"
#include "diagnostic_board.h"
#include "ros2_bridge.h"
#include "uv_state.h"

#include <QSettings>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include "Gamepad/gamepad.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setConsole();

private:
    Gamepad *gamepad = nullptr;
    std::unique_ptr<GamepadInputSource> gamepadInput;
    // Используем enum class с фиксированным типом uint8_t
    enum class SpeedMode : uint8_t { Slow = 0, Medium = 1, Fast = 2 };
    SpeedMode currentMode; // инициализируется в конструкторе

    QMap<int, QMap<QString, double>> speedModeGains; // [режим][имя]
    QVector<QSpinBox*> gainSpinBoxes;
    QStringList gainNames = {"surge", "sway", "depth", "yaw", "pitch", "roll"};

    void saveCurrentModeGains();
    void setSpinBoxValuesForCurrentMode();
    void saveSettings();
    void loadSettings();
    void useGamepad();

    void setTimer_updateImpact(int periodUpdateMsec);
    void setBottom();
    void setBottom_mode();
    void setupLightControls();


    /*!
     * \brief setTab устанавливает названия вкладкам.
     */
    void setTab();
    void setUpdateUI();
    void setWidget();

    void setInterface();


    bool status_keyboard = false;


    // legacy widgets (сейчас не используются в сборке)
    // PowerSystem         *powerSystem;
    // CheckMsg            *checkMsg;
    // ModeAutomatic       *modeAutomatic;
    Diagnostic_board    *diagnostic_board;
    RosBridge           *rosBridge;
    UVState             *uvState;

private slots:
    // Единый слот для переключения режима скорости
    void setSpeedMode(SpeedMode mode);

    void displayText(QString str);
    void updateUi_fromControl();
    void resetControlImpact();

    /*!
     * \brief updateUi_Compass слот обновления компаса на UI форме.
     * \param yaw новое значение курса.
     */
    void updateUi_Compass(float yaw);
    void useKeyBoard();
    void useJoyStick();


signals:
    void updateCompass(float yaw);

    void publishTwistRequested(double x, double y, double z,
                               double angular_x, double angular_y, double angular_z);
    void publishLightsRequested(uint8_t left, uint8_t right);
    void controlFlagRequested(uint8_t bit, bool value);
    // legacy signals (пока не используются)
    // void updateIMU(DataAH127C imuData);
    // void updateSetupMsg();
    // void updateDataMission();
    // void updateStatePushButton();
    // void updateMap();
    // void pointAdded(qreal x, qreal y);
    // void toggleMissionPlanning_cppPointsEnabled();
    // void signal_sendCurrentPos(double latitude, double longitude);

protected:
    Ui::MainWindow *ui;
    QTimer *updateTimer = nullptr;

    std::unique_ptr<JoyStick> joyStick;
    std::unique_ptr<KeyBoard> keyBoard;
    umas::input::IInputSource *activeInput = nullptr;
    umas::control::ControlService controlService;

    umas::input::ControlCommand applyGains(const umas::input::ControlCommand& raw) const;
    void updateControlLabels(const umas::input::ControlCommand& scaled);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // MAINWINDOW_H
