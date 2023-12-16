#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDebug>

#include <QTime>

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
    /*!
     * \brief setBottom_mode устанавливает кнопки и слоты,
     *  связанные с режимами управления.
     */
    void setBottom_mode();
    /*!
     * \brief setBottom_modeAutomated устанавливает кнопки и слоты,
     *  связанные с автоматизированным режимом управления.
     */
    void setBottom_modeAutomated();
    /*!
     * \brief setBottom_modeAutomatic устанавливает кнопки и слоты,
     *  связанные с автоматическим режимом управления.
     */
    void setBottom_modeAutomatic();


    /*!
     * \brief setBottom_powerMode устанавливает кнопки и слоты,
     *  связанные с режимами питания.
     */
    void setBottom_powerMode();
    /*!
     * \brief setBottom_connection устанавливает кнопки и слоты,
     *  связанные с установкой соединения к аппарата.
     */
    void setBottom_connection();
    /*!
     * \brief setBottom_modeSelection устанавливает кнопки и слоты,
     *  связанные с режимом вывода данных.
     */
    void setBottom_modeSelection();
    /*!
     * \brief setBottom_setupIMU устанавливает на нажатие кнопки слот
     *  вызова окна настройки БСО.
     */
    void setBottom_setupIMU();
    /*!
     * \brief setBottom_setupIMU_check устанавливает на нажатие кнопки
     *  слот вызова окна настройки БСО.
     */
    void setBottom_setupIMU_check();

    void setBottom_selectAgent();

    /*!
     * \brief setTab устанавливает названия вкладкам.
     */
    void setTab();

    /*!
     * \brief setMap устанавливает настройки для карты.
     */
    void setMap();

    /*!
     * \brief setUpdateUI устанавливает слоты обновления для UI формы.
     */
    void setUpdateUI();


private slots:
    /*!
     * \brief displayText слот для вывода сообщений в консоль.
     * \param str является выводимой строкой.
     */
    void displayText(QString str);

    void test_automatic_after();

    /*!
     * \brief setLocationUWB слот записи данных о месторасположении
     *  3 uwb-модулей.
     * \param x координаты по оси X.
     * \param y координаты по оси Y.
     */
    void setLocationUWB(double *x, double *y);

    /*!
     * \brief updateUi_fromControl слот для обновления на UI форме
     *  данных управляющих воздействий.
     */
    void updateUi_fromControl();

    /*!
     * \brief e_CSModeManualToggled слот для установки ручного режима
     *  управления.
     */
    void e_CSModeManualToggled();
    /*!
     * \brief e_CSModeAutomatedToggled слот для установки автоматизированного
     *  режима управления.
     */
    void e_CSModeAutomatedToggled();
    /*!
     * \brief e_CSModeAutomaticToggled слот для установки автоматического
     *  режима управления.
     */
    void e_CSModeAutomaticToggled();

    /*!
     * \brief stabilizeYawToggled слот для переключения канала курса.
     * \param state состояние нажатия кнопки.
     */
    void stabilizeYawToggled(bool state);
    /*!
     * \brief stabilizePitchToggled слот для переключения канала дифферента.
     * \param state состояние нажатия кнопки.
     */
    void stabilizePitchToggled(bool state);
    /*!
     * \brief stabilizeRollToggled слот для переключения канала крена.
     * \param state состояние нажатия кнопки.
     */
    void stabilizeRollToggled(bool state);
    /*!
     * \brief stabilizeMarchToggled слот для переключения канала марша.
     * \param state состояние нажатия кнопки.
     */
    void stabilizeMarchToggled(bool state);
    /*!
     * \brief stabilizeLagToggled слот для переключения канала лага.
     * \param state состояние нажатия кнопки.
     */
    void stabilizeLagToggled(bool state);

    /*!
     * \brief stabilizeDepthToggled слот для переключения канала глубины.
     * \param state состояние нажатия кнопки.
     */
    void stabilizeDepthToggled(bool state);

    /*!
     * \brief pushButton_on_powerMode_2 слот установки 2 режима питания.
     */
    void pushButton_on_powerMode_2();
    /*!
     * \brief pushButton_on_powerMode_3 слот установки 3 режима питания.
     */
    void pushButton_on_powerMode_3();
    /*!
     * \brief pushButton_on_powerMode_4 слот установки 4 режима питания.
     */
    void pushButton_on_powerMode_4();
    /*!
     * \brief pushButton_on_powerMode_5 слот установки 5 режима питания
     *  и запуск таймера переключения режима питания на предыдущий.
     */
    void pushButton_on_powerMode_5();
    /*!
     * \brief off_powerMode_5 слот выключения таймера 5 режима питания
     *  и переключение режима питания на предыдущий.
     */
    void off_powerMode_5();

    /*!
     * \brief setConnection слот установления соединения.
     */
    void setConnection();
    /*!
     * \brief updateUi_fromAgent1 слот вызова сигналов обновления UI формы.
     */
    void updateUi_fromAgent1();
    void updateUi_fromAgent2();

    /*!
     * \brief breakConnection слот разрыва соединения.
     */
    void breakConnection();

    /*!
     * \brief setModeSelection слот установления вывода данных.
     * \param index выбор вывода данных.
     */
    void setModeSelection(int index);

    /*!
     * \brief getWindow_setupIMU слот вызова окна настройки БСО.
     */
    void getWindow_setupIMU();
    /*!
     * \brief getWindow_setupIMU_check слот вызова окна настройки БСО.
     */
    void getWindow_setupIMU_check();

    /*!
     * \brief updateUi_Compass слот обновления компаса на UI форме.
     * \param yaw новое значение курса.
     */
    void updateUi_Compass(float yaw);
    /*!
     * \brief updateUi_IMU слот обновления данных с БСО на UI форме.
     * \param imuData структура данных с обновленными значениями с БСО.
     */
    void updateUi_IMU(DataAH127C imuData);
    /*!
     * \brief updateUi_SetupMsg слот обновления отправленных и полученных
     *  данных на UI форме.
     */
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

    void slot_pushButton_missionPlanning_following();
    void slot_pushButton_missionPlanning_go_trajectory();

    void updateUi_DataMission();

signals:
    /*!
     * \brief updateCompass сигнал запуска обновления компаса на UI форме.
     * \param yaw новое значение курса.
     */
    void updateCompass(float yaw);
    /*!
     * \brief updateIMU сигнал запуска обновления данных с БСО на UI форме.
     * \param imuData структура данных с обновленными значениями с БСО.
     */
    void updateIMU(DataAH127C imuData);
    /*!
     * \brief updateSetupMsg сигнал запуска обновления отправленных и полученных
     *  данных на UI форме.
     */
    void updateSetupMsg();

    void updateDataMission();

    void updateMap(DataUWB dataUWB);
    void updateMapForAgent2(DataUWB dataUWB_agent2);
    void signal_pushButton_missionPlanning_goto_updateMap(double x, double y, double r);

protected:
    /*!
     * \brief ui указатель на главную форму.
     */
    Ui::MainWindow *ui;

    /*!
     * \brief updateTimer таймер обновления данных от пульта управления.
     */
    QTimer *updateTimer = nullptr;

    JoyStick *joyStick = nullptr;
    KeyBoard *keyBoard = nullptr;
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    /*!
     * \brief timer_off_powerMode_5 таймер переключения 5 режима питания.
     */
    QTimer *timer_off_powerMode_5;
    /*!
     * \brief before_powerMode режим питания, используемый перед 5.
     */
    power_Mode before_powerMode;

    /*!
     * \brief uv_interface интерфейс взаимодействия с глобальной переменной,
     * с принимаемыми и отправляемыми сообщениями.
     */
    IUserInterfaceData uv_interface;
    /*!
     * \brief communicationAgent1 указатель на объект класса для UDP соединения.
     */
    Pult::PC_Protocol   *communicationAgent1;

    Pult::PC_Protocol   *communicationAgent2;


    /*!
     * \brief pult объект класса для обновления задающих воздействий
     */
    RemoteControl pult;

};
#endif // MAINWINDOW_H
