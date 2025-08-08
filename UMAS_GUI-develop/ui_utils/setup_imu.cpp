#include "setup_imu.h"
#include "ui_setup_imu.h"

SetupIMU::SetupIMU(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupIMU)
{
    ui->setupUi(this);

    connect(
        ui->pushButton_setupIMU_nextStep1, SIGNAL(clicked()),
        this, SLOT(setupIMU_nextStep1()));
}

SetupIMU::~SetupIMU()
{
    delete ui;
}

void SetupIMU::setupIMU_nextStep1()
{
    hide();
    SetupIMU_start window_setupIMU_start;
    window_setupIMU_start.setModal(true);
    window_setupIMU_start.exec();
}
