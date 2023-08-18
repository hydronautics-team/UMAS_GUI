#ifndef SETUPIMU_START_H
#define SETUPIMU_START_H

#include <QDialog>
#include <QTimer>
#include <QDebug>
#include <QStatusBar>

#include "i_user_interface_data.h"

namespace Ui {
class SetupIMU_start;
}

class SetupIMU_start : public QDialog
{
    Q_OBJECT

public:
    explicit SetupIMU_start(QWidget *parent = nullptr);
    ~SetupIMU_start();


private slots:
    void pushButton_startTimer1();
    void pushButton_startTimer2();
    void updateUi_sec_t1();
    void updateUi_sec_t2();

    void isCheckedFlagBort_start();
    void isCheckedFlagBort_end();

signals:
    void on_t2();

private:
    Ui::SetupIMU_start *ui;
    IUserInterfaceData uv_interface;
    FlagAH127C_pult flagAH127C_pult;
    FlagAH127C_bort flagAH127C_bort;


//    QStatusBar *statusBar;

    QTimer *updateTimerSec;
    QTimer *timer_checkFlag;

    int sec = 30;

};

#endif // SETUPIMU_START_H
