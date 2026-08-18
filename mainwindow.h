#pragma once


#include <QMainWindow>
#include <QTimer>
#include <QDebug>
#include <QProcess>
#include <QTime>
#include <QButtonGroup>
#include <QString>
#include <QKeyEvent>
#include <memory>
#include <QSettings>
#include <QSpinBox>

#include "joy_stick.h"
#include "key_board.h"
#include "input/i_input_source.h"
#include "input/gamepad_input_source.h"
#include "control/control_service.h"
#include "diagnostic_board.h"
#include "ros2_bridge.h"
#include "uv_state.h"
#include "video/video_player_widget.h"
#include "video/fullscreen_video_window.h"
#include "Gamepad/gamepad.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class VideoPlayerWidget;
class FullscreenVideoWindow; 

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
  
    FullscreenVideoWindow* fullscreenWindow_ = nullptr;

    enum class SpeedMode : uint8_t { Slow = 0, Medium = 1, Fast = 2 };
    SpeedMode currentMode;

    QMap<int, QMap<QString, double>> speedModeGains;
    QVector<QSpinBox*> gainSpinBoxes;
    QStringList gainNames = {"surge", "sway", "depth", "yaw", "pitch", "roll"};

    void saveCurrentModeGains();
    void setSpinBoxValuesForCurrentMode();
    void saveSettings();
    void loadSettings();
    void useGamepad();
    void setTimer_updateImpact(int periodUpdateMsec);
    void setBottom();
    void setupButtonStyles();
    void setBottom_mode();
    void setTab();
    void setUpdateUI();
    void setWidget();
    void setInterface();

    void resetTelemetryToDefault();
    void updateTelemetryFromState();
    bool isConnected = false;


    bool status_keyboard = false;

    Diagnostic_board    *diagnostic_board;
    RosBridge           *rosBridge;
    UVState             *uvState;
    VideoPlayerWidget   *videoPlayer_ = nullptr;

private slots:
    void setSpeedMode(SpeedMode mode);
    void displayText(QString str);
    void updateUi_fromControl();
    void updateUi_Compass(float yaw);
    void useKeyBoard();
    void useJoyStick();

signals:
    void updateCompass(float yaw);
    void publishTwistRequested(double x, double y, double z,
                               double angular_x, double angular_y, double angular_z);
    void controlFlagRequested(uint8_t bit, bool value);

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
