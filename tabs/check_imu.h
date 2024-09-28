#ifndef CHECK_IMU_H
#define CHECK_IMU_H

#include <QWidget>
#include <QButtonGroup>
#include <QTimer>

#include "i_user_interface_data.h"
#include "setup_imu.h"
#include "setupimu_check.h"

namespace Ui {
class CheckImu;
}

class CheckImu : public QWidget
{
    Q_OBJECT

public:
    explicit CheckImu(QWidget *parent = nullptr);
    ~CheckImu();

public slots:
    void updateUi_imu(DataAH127C imuData);

private:
    Ui::CheckImu *ui;

    void setBottom_setupIMU();
    void setBottom_setupIMU_check();

private slots:
    void getWindow_setupIMU();
    void getWindow_setupIMU_check();
};

#endif // CHECK_IMU_H
