#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDebug>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>

#include <QButtonGroup>

#include "remote_control.h"
#include "uv_state.h"
#include "i_user_interface_data.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void timerUpdateImpact(int periodUpdateMsec);

private slots:
    void updateUi_fromControl();
    void stabilizeYawToggled(bool state);
    void stabilizePitchToggled(bool state);
    void stabilizeRollToggled(bool state);
    void stabilizeMarchToggled(bool state);
    void stabilizeLagToggled(bool state);

    void e_CSModeManualToggled();
    void e_CSModeAutomatedToggled();

private:
    Ui::MainWindow *ui;

    QButtonGroup *mode;
    QButtonGroup *modeAutomated;

    IUserInterfaceData uv_interface;
    RemoteControl joystick;


};
#endif // MAINWINDOW_H
