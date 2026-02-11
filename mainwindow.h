#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDebug>
#include <QProcess>
#include <QTime>
#include <QButtonGroup>

#include "remote_control.h"
#include "uv_state.h"
#include "i_basic_data.h"
#include "joy_stick.h"
#include "key_board.h"
// #include "power_system.h"
// #include "mode_automatic.h"
// #include "map_widget.h"
#include "diagnostic_board.h"
#include "ros2_bridge.h"


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
     * \brief setTab устанавливает названия вкладкам.
     */
    void setTab();

    /*!
     * \brief setUpdateUI устанавливает слоты обновления для UI формы.
     */
    void setUpdateUI();

    void setModeAutomatic_mission_cpp();

    void setWidget();

    void setInterface();
    // PowerSystem         *powerSystem;
    // CheckMsg            *checkMsg;
    // CheckImu            *checkImu;
    // ModeAutomatic       *modeAutomatic;
    // MapWidget           *mapWidget;
    Diagnostic_board    *diagnostic_board;
    RosBridge           *rosBridge;

private slots:

    /*!
     * \brief displayText слот для вывода сообщений в консоль.
     * \param str является выводимой строкой.
     */
    void displayText(QString str);

    /*!
     * \brief updateUi_fromControl слот для обновления на UI форме
     *  данных управляющих воздействий.
     */
    void updateUi_fromControl();

    /*!
     * \brief updateUi_Compass слот обновления компаса на UI форме.
     * \param yaw новое значение курса.
     */
    void updateUi_Compass(float yaw);

    void useKeyBoard();
    void useJoyStick();


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

    void updateStatePushButton();

    void updateMap();

    void pointAdded(qreal x, qreal y);

    void toggleMissionPlanning_cppPointsEnabled();

    // void signal_setMarker(const QGeoCoordinate &coordinate);
    void signal_sendCurrentPos(double latitude, double longitude);

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
};
#endif // MAINWINDOW_H
