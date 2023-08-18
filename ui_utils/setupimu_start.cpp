#include "setupimu_start.h"
#include "ui_setupimu_start.h"

SetupIMU_start::SetupIMU_start(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupIMU_start)
{
    ui->setupUi(this);
    ui->pushButton_setupIMU_end->setEnabled(false);


    connect(
        ui->pushButton_setupIMU_startTimer, SIGNAL(clicked()),
        this, SLOT(pushButton_startTimer1()));

    connect(
        this, SIGNAL(on_t2()),
        this, SLOT(pushButton_startTimer2()));
    connect(
        ui->pushButton_setupIMU_end, SIGNAL(clicked()),
        this, SLOT(close()));
}

SetupIMU_start::~SetupIMU_start()
{
    delete ui;
}

void SetupIMU_start::pushButton_startTimer1()
{
    flagAH127C_pult.initCalibration = true;
    uv_interface.setFlagAH127C_pult(flagAH127C_pult);

    timer_checkFlag = new QTimer(this);
    connect(
        timer_checkFlag, SIGNAL(timeout()),
        this, SLOT(isCheckedFlagBort_start()));
    timer_checkFlag->start(1000);
}

void SetupIMU_start::isCheckedFlagBort_start()
{
    flagAH127C_bort = uv_interface.getFlagAH127C_bort();
    if(flagAH127C_bort.startCalibration)
    {
        timer_checkFlag->stop();
        flagAH127C_pult.initCalibration = false;
        uv_interface.setFlagAH127C_pult(flagAH127C_pult);

        ui->pushButton_setupIMU_startTimer->setEnabled(false);
        ui->SetupIMU_start_statusBar->setStyleSheet("color: rgb(0, 153, 76)");
        ui->SetupIMU_start_statusBar->setText("Равномерно поворачивайте аппарат вокруг вертикали 2 раза за 30 секунд");

        updateTimerSec = new QTimer(this);
        connect(
            updateTimerSec, SIGNAL(timeout()),
            this, SLOT(updateUi_sec_t1())
            );
        updateTimerSec->start(1000);
    }
    else
    {
        ui->SetupIMU_start_statusBar->setStyleSheet("color: rgb(255, 0, 0)");
        ui->SetupIMU_start_statusBar->setText("Ожидаение подтверждения настройки от БСО");
    }
}

void SetupIMU_start::updateUi_sec_t1()
{
    ui->label_setupIMU_timer1->setText(QString::number(--sec));
    if (sec == 0) {
        sec = 30;
        updateTimerSec->stop();
        emit on_t2();
    }
}

void SetupIMU_start::pushButton_startTimer2()
{
    ui->SetupIMU_start_statusBar->setStyleSheet("color: rgb(0, 153, 76)");
    ui->SetupIMU_start_statusBar->setText("Теперь вокруг оси вращения 2 раза за 30 секунд");

    updateTimerSec = new QTimer(this);
    connect(
        updateTimerSec, SIGNAL(timeout()),
        this, SLOT(updateUi_sec_t2())
        );
    updateTimerSec->start(1000);

}

void SetupIMU_start::updateUi_sec_t2()
{
    ui->label_setupIMU_timer2->setText(QString::number(--sec));
    if (sec == 0)
    {
        updateTimerSec->stop();
        flagAH127C_pult.saveCalibration = true;
        uv_interface.setFlagAH127C_pult(flagAH127C_pult);

        timer_checkFlag = new QTimer(this);
        connect(
            timer_checkFlag, SIGNAL(timeout()),
            this, SLOT(isCheckedFlagBort_end()));
        timer_checkFlag->start(1000);
    }
}

void SetupIMU_start::isCheckedFlagBort_end()
{
    flagAH127C_bort = uv_interface.getFlagAH127C_bort();
    if(flagAH127C_bort.endCalibration)
    {
        timer_checkFlag->stop();
        flagAH127C_pult.saveCalibration = false;
        uv_interface.setFlagAH127C_pult(flagAH127C_pult);

        ui->pushButton_setupIMU_end->setEnabled(true);
        ui->SetupIMU_start_statusBar->setStyleSheet("color: rgb(0, 153, 76)");
        ui->SetupIMU_start_statusBar->setText("Данные сохранились, можно завершать настройку ");
    }
    else
    {
        ui->SetupIMU_start_statusBar->setStyleSheet("color: rgb(255, 0, 0)");
        ui->SetupIMU_start_statusBar->setText("Данные сохраняются");
    }
}

