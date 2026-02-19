#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include <QMainWindow>
#include <QTimer>
#include <QDebug>
#include <QProcess>
#include <QTime>
#include <QButtonGroup>

#include <QString>

#include "remote_control.h"
#include "uv_state.h"
#include "i_user_interface_data.h"
#include "pc_protocol.h"
#include "i_server_data.h"
#include "i_basic_data.h"
#include "joy_stick.h"
#include "key_board.h"
#include "power_system.h"
#include "check_msg.h"
#include "check_imu.h"
#include "mode_automatic.h"
#include "diagnostic_board.h"
#include "ros2_bridge.h"
#include <QSettings>
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
    Gamepad *gamepad; 
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
    void setBottom_connection();
    void setBottom_modeSelection();
    void setBottom_selectAgent();
    void setTab();
    void setUpdateUI();
    void setWidget();
    void setGUI_reper();
    void setInterface();


    bool m_gamepadActive = false;
    bool status_keyboard = false;
float m_gamepadMarch = 0.0f;

float m_gamepadDepth = 0.0f;
float m_gamepadYaw = 0.0f;
float m_gamepadPitch = 0.0f;
float m_gamepadCren = 0.0f; // Крен



float m_gamepadU = 0.0f;   // для оси U (правый стик X)
float m_gamepadV = 0.0f; // для оси V (правый стик Y)


float m_gamepad_right_marsh = 0.0f; //  левый стик ось У
float m_gamepad_right_course = 0.0f;


    PowerSystem         *powerSystem;
    CheckMsg            *checkMsg;
    ModeAutomatic       *modeAutomatic;
    Diagnostic_board    *diagnostic_board;
    RosBridge           *rosBridge;

private slots:
    // Единый слот для переключения режима скорости
    void setSpeedMode(SpeedMode mode);

    void displayText(QString str);
    void updateUi_fromControl();
    void updateUi_Map();
    void e_CSModeManualToggled();
    void e_CSModeAutomatedToggled();
    void e_CSModeAutomaticToggled();
    void setConnection();
    void updateUi_fromAgent1();
    void breakConnection();
    void setModeSelection(int index);
    void updateUi_Compass(float yaw);
    void useKeyBoard();
    void useJoyStick();
    void pushButton_selectAgent1(bool stateBottom);
    void updateUi_statePushButton();
    void slot_pushButton_sendReper();
    void slot_addMarker_to_gui(double x, double y);

signals:
    void updateCompass(float yaw);
    void updateIMU(DataAH127C imuData);
    void updateSetupMsg();
    void updateDataMission();
    void updateStatePushButton();
    void updateMap();
    void pointAdded(qreal x, qreal y);
    void toggleMissionPlanning_cppPointsEnabled();
    void signal_setInterface(IUserInterfaceData *uv_interface);
    void signal_sendCurrentPos(double latitude, double longitude);

protected:
    Ui::MainWindow *ui;
    QTimer *updateTimer = nullptr;
    JoyStick *joyStick = nullptr;
    KeyBoard *keyBoard = nullptr;
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    IUserInterfaceData uv_interface;
    Pult::PC_Protocol   *communicationAgent1;
    RemoteControl pult;
};

#endif // MAINWINDOW_H