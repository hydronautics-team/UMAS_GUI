#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDebug>
#include <QTime>
#include <QButtonGroup>
#include <QPushButton>
#include <QProcess>

#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <gst/app/gstappsink.h>

#include "remote_control.h"
#include "uv_state.h"
#include "i_user_interface_data.h"
#include "pc_protocol.h"
#include "i_server_data.h"

#include "setup_imu.h"
#include "setupimu_check.h"
#include "map.h"

#include "i_basic_data.h"

#include "joy_stick.h"
#include "key_board.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/*!
 * \brief MainWindow - класс формы главного окна, в котором
 *  реализованы основные методы для работы с ним.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*!
     * \brief MainWindow конструктор, в котором создается главная UI форма.
     */
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /*!
     * \brief setConsole устанавливает настройки для консоли.
     */
    void setConsole();

private:
    /*!
     * \brief setTimer_updateImpact устанавливает таймер
     *  обработки пульта управления.
     * \param periodUpdateMsec период обновления таймера
     *  обработки значений пульта управления.
     */
    void setTimer_updateImpact(int periodUpdateMsec);

    /*!
     * \brief setBottom устанавливает все используемые кнопки.
     */
    void setBottom();
    void setBottom_mode();
    void setBottom_modeAutomated();
    void setBottom_modeAutomatic();
    void setBottom_powerMode();
    void setBottom_connection();
    void setBottom_modeSelection();
    void setBottom_setupIMU();
    void setBottom_setupIMU_check();
    void setBottom_selectAgent();

    void setTab();
    void setMap();
    void setUpdateUI();

private slots:
    void displayText(QString str);
    void test_automatic_after();
    void setLocationUWB(double *x, double *y);
    void updateUi_fromControl();

    void e_CSModeManualToggled();
    void e_CSModeAutomatedToggled();
    void e_CSModeAutomaticToggled();

    void stabilizeYawToggled(bool state);
    void stabilizePitchToggled(bool state);
    void stabilizeRollToggled(bool state);
    void stabilizeMarchToggled(bool state);
    void stabilizeLagToggled(bool state);
    void stabilizeDepthToggled(bool state);

    void pushButton_on_powerMode_2();
    void pushButton_on_powerMode_3();
    void pushButton_on_powerMode_4();
    void pushButton_on_powerMode_5();
    void off_powerMode_5();

    void setConnection();
    void updateUi_fromAgent1();
    void updateUi_fromAgent2();
    void breakConnection();

    void setModeSelection(int index);

    void getWindow_setupIMU();
    void getWindow_setupIMU_check();

    void updateUi_Compass(float yaw);
    void updateUi_IMU(DataAH127C imuData);
    void updateUi_SetupMsg();

    void useKeyBoard();
    void useJoyStick();

    void pushButton_selectAgent1(bool stateBottom);
    void pushButton_selectAgent2(bool stateBottom);

    void slot_pushButton_missionControl_modeIdle();
    void slot_pushButton_missionControl_modeStart();
    void slot_pushButton_missionControl_modeCancel();
    void slot_pushButton_missionControl_modeStop();
    void slot_pushButton_missionControl_modeComplete();

    void slot_pushButton_missionPlanning_goto();
    void slot_pushButton_missionPlanning_goto_update();
    void slot_pushButton_missionPlanning_goto_back();

    void slot_pushButton_missionPlanning_go_trajectory_update();
    void slot_pushButton_missionPlanning_go_trajectory_back();

    void slot_pushButton_missionPlanning_following();
    void slot_pushButton_missionPlanning_go_trajectory();

    void updateUi_DataMission();
    void updateUi_statePushButton();

    void on_pushButton_Play_Pause_clicked();

signals:
    void updateCompass(float yaw);
    void updateIMU(DataAH127C imuData);
    void updateSetupMsg();
    void updateDataMission();
    void updateMap(DataUWB dataUWB);
    void updateMapForAgent2(DataUWB dataUWB_agent2);
    void signal_pushButton_missionPlanning_goto_updateMap(double x, double y, double r, int flag_clear);
    void signal_pushButton_missionPlanning_go_trajectory_updateMap(double x, double y, double r, int flag_clear);
    void updateStatePushButton();

protected:
    Ui::MainWindow *ui;

    QTimer *updateTimer = nullptr;

    JoyStick *joyStick = nullptr;
    KeyBoard *keyBoard = nullptr;
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    QTimer *timer_off_powerMode_5;
    power_Mode before_powerMode;

    IUserInterfaceData uv_interface;
    Pult::PC_Protocol   *communicationAgent1;
    Pult::PC_Protocol   *communicationAgent2;

    RemoteControl pult;

    // === Добавлено из второго файла ===
    GstElement *pipeline = nullptr;
    WId videoWindowId = 0;
    // ===================================
};

#endif // MAINWINDOW_H
