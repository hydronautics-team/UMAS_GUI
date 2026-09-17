#include "check_imu.h"
#include "ui_check_imu.h"

CheckImu::CheckImu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CheckImu)
{
    ui->setupUi(this);

    setBottom_setupIMU();
    setBottom_setupIMU_check();
}

void CheckImu::setBottom_setupIMU()
{
    connect(
        ui->pushButton_setupIMU, SIGNAL(clicked()),
        this, SLOT(getWindow_setupIMU()));
}

void CheckImu::setBottom_setupIMU_check()
{
    connect(
        ui->pushButton_setupIMU_check, SIGNAL(clicked()),
        this, SLOT(getWindow_setupIMU_check()));
}

void CheckImu::updateUi_imu(DataAH127C imuData){
    ui->label_IMUdata_accel_X->setText(QString::number(imuData.X_accel, 'f', 2));
    ui->label_IMUdata_accel_Y->setText(QString::number(imuData.Y_accel, 'f', 2));
    ui->label_IMUdata_accel_Z->setText(QString::number(imuData.Z_accel, 'f', 2));

    ui->label_IMUdata_rate_X->setText(QString::number(imuData.X_rate, 'f', 2));
    ui->label_IMUdata_rate_Y->setText(QString::number(imuData.Y_rate, 'f', 2));
    ui->label_IMUdata_rate_Z->setText(QString::number(imuData.Z_rate, 'f', 2));

    ui->label_IMUdata_magn_X->setText(QString::number(imuData.X_magn, 'f', 2));
    ui->label_IMUdata_magn_Y->setText(QString::number(imuData.Y_magn, 'f', 2));
    ui->label_IMUdata_magn_Z->setText(QString::number(imuData.Z_magn, 'f', 2));

    ui->label_IMUdata_q0->setText(QString::number(imuData.quat[0], 'f', 2));
    ui->label_IMUdata_q1->setText(QString::number(imuData.quat[1], 'f', 2));
    ui->label_IMUdata_q2->setText(QString::number(imuData.quat[2], 'f', 2));
    ui->label_IMUdata_q3->setText(QString::number(imuData.quat[3], 'f', 2));

}

void CheckImu::getWindow_setupIMU()
{
    SetupIMU window_setupIMU;
    window_setupIMU.setModal(false);
    window_setupIMU.exec();
}


void CheckImu::getWindow_setupIMU_check()
{
    SetupIMU_check window_setupIMU_check;
    window_setupIMU_check.setModal(false);
    window_setupIMU_check.exec();
}

CheckImu::~CheckImu()
{
    delete ui;
}
