#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDebug>

#include <QButtonGroup>

#include "remote_control.h"
#include "uv_state.h"
#include "i_user_interface_data.h"
#include "pc_protocol.h"
#include "i_server_data.h"

#include "setup_imu.h"
#include "setupimu_check.h"
#include "map.h"

#include "i_basic_data.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setBottom(Ui::MainWindow *ui, QObject *ts);
    void setBottom_mode(Ui::MainWindow *ui, QObject *ts);
    void setBottom_modeAutomated(Ui::MainWindow *ui, QObject *ts);
    void setBottom_powerMode(Ui::MainWindow *ui, QObject *ts);
    void setBottom_connection(Ui::MainWindow *ui, QObject *ts);
    void setBottom_modeSelection(Ui::MainWindow *ui, QObject *ts);
    void setBottom_setupIMU(Ui::MainWindow *ui, QObject *ts);
    void setBottom_setupIMU_check(Ui::MainWindow *ui, QObject *ts);

    void setTab(Ui::MainWindow *ui);

    void setTimer_updateImpact(int periodUpdateMsec);

private slots:
    void updateUi_fromControl();
    void stabilizeYawToggled(bool state);
    void stabilizePitchToggled(bool state);
    void stabilizeRollToggled(bool state);
    void stabilizeMarchToggled(bool state);
    void stabilizeLagToggled(bool state);

    void e_CSModeManualToggled();
    void e_CSModeAutomatedToggled();

    void pushButton_on_powerMode_2();
    void pushButton_on_powerMode_3();
    void pushButton_on_powerMode_4();
    void pushButton_on_powerMode_5();
    void off_powerMode_5();

    void setModeSelection(int index);

    void updateUi_fromAgent();
    void updateUi_Compass(float yaw);
    void updateUi_IMU(DataAH127C imuData);
    void updateUi_SetupMsg();

    void setConnection();
    void breakConnection();

    void getWindow_setupIMU();
    void getWindow_setupIMU_check();

signals:
    void updateCompass(float yaw);
    void updateIMU(DataAH127C imuData);
    void updateSetupMsg();

private:
    Ui::MainWindow *ui;
    QTimer *updateTimer = nullptr;

    QTimer *timer_off_powerMode_5;
    power_Mode before_powerMode;

    IUserInterfaceData uv_interface;
    Pult::PC_Protocol* pultProtocol;

    RemoteControl joystick;
    IServerData data;

};
#endif // MAINWINDOW_H
