#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MainWindow::timerUpdateImpact(joystick.periodUpdateMsec);

    ui->pushButton_modeManual->setCheckable(true);
    ui->pushButton_modeAutomated->setCheckable(true);
    mode = new QButtonGroup(this);
    mode->addButton(ui->pushButton_modeManual);
    mode->addButton(ui->pushButton_modeAutomated);
    mode->setExclusive(true);

    ui->pushButton_modeAutomated_march->setCheckable(true);
    ui->pushButton_modeAutomated_lag->setCheckable(true);
    ui->pushButton_modeAutomated_psi->setCheckable(true);
    ui->pushButton_modeAutomated_tetta->setCheckable(true);
    ui->pushButton_modeAutomated_gamma->setCheckable(true);
    modeAutomated = new QButtonGroup(this);
    modeAutomated->addButton(ui->pushButton_modeAutomated_march);
    modeAutomated->addButton(ui->pushButton_modeAutomated_lag);
    modeAutomated->addButton(ui->pushButton_modeAutomated_psi);
    modeAutomated->addButton(ui->pushButton_modeAutomated_tetta);
    modeAutomated->addButton(ui->pushButton_modeAutomated_gamma);
    modeAutomated->setExclusive(false);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerUpdateImpact(int periodUpdateMsec){
    QTimer *updateTimer = new QTimer(this);
    connect(
        updateTimer, SIGNAL(timeout()),
        this, SLOT(updateUi_fromControl())
        );
    updateTimer->start(periodUpdateMsec);
}

void MainWindow::updateUi_fromControl(){
    ControlData control = uv_interface.getControlData();

    ui->label_impactDataDepth->setText(QString::number(control.march));
    ui->label_impactDataRoll->setText(QString::number(control.roll));
    ui->label_impactDataPitch->setText(QString::number(control.pitch));
    ui->label_impactDataYaw->setText(QString::number(control.yaw));
}
