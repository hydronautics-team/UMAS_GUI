#include "setupimu_start.h"
#include "ui_setupimu_start.h"

SetupIMU_start::SetupIMU_start(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupIMU_start)
{
    ui->setupUi(this);


    connect(
        ui->pushButton_setupIMU_startTimer, SIGNAL(clicked()),
        this, SLOT(pushButton_startTimer1()));

    connect(this, SIGNAL(on_t2()),
            this, SLOT(pushButton_startTimer2()));
    connect(ui->pushButton_setupIMU_end, SIGNAL(clicked()),
            this, SLOT(close()));

}

SetupIMU_start::~SetupIMU_start()
{
    delete ui;
}

void SetupIMU_start::pushButton_startTimer1()
{
    updateTimerSec = new QTimer(this);
    connect(
        updateTimerSec, SIGNAL(timeout()),
        this, SLOT(updateUi_sec_t1())
        );
    updateTimerSec->start(1000);

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
        updateTimerSec->stop();

}
