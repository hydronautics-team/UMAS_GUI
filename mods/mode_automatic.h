#ifndef MODE_AUTOMATIC_H
#define MODE_AUTOMATIC_H
#include "ui_mode_automatic.h"

#include <QWidget>
#include <QButtonGroup>
#include <QTimer>
#include <QProcess>

#include "i_user_interface_data.h"
#include "map.h"


namespace Ui {
class ModeAutomatic;
}

class ModeAutomatic : public QWidget
{
    Q_OBJECT

public:
    explicit ModeAutomatic(QWidget *parent = nullptr);
    ~ModeAutomatic();
    Ui::ModeAutomatic *ui;

signals:
    void signal_pushButton_missionPlanning_goto_updateMap(double x, double y, double r, int flag_clear);
    void signal_pushButton_missionPlanning_go_trajectory_updateMap(double x, double y, double r, int flag_clear);

    void requestUpdateChart(QLineSeries *lineSeries);
    void requestClearLines();
    void plotLineSeries(QLineSeries* series);
    void displayText_toConsole(QString str);

    void set_stackedWidget_mode(int index);

private:
    IUserInterfaceData uv_interface;

    void setBottom_modeAutomatic();

    void setMission_control();
    void setMission_goTo();
    void setMission_go_trajectory();
    void setMission_go_following();
    void setMission_cpp();

private slots:
    void slot_pushButton_missionPlanning_cpp_make();
    void slot_pushButton_missionPlanning_cpp_make_clean();
    void slot_pushButton_missionPlanning_cpp_back();
    void slot_pushButton_missionPlanning_cpp();
    void test_automatic_after();
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

public slots:
    void addPointToTable(qreal x, qreal y);

};

#endif // MODE_AUTOMATIC_H
