#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    RemoteControl joystick;
    MainWindow::timerUpdateImpact(joystick.periodUpdateMsec);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerUpdateImpact(int periodUpdateMsec){
    QTimer *updateTimer = new QTimer(this);
    connect(
        updateTimer, SIGNAL(timeout()),
        this, SLOT(UpdateImpact())
        );
    updateTimer->start(periodUpdateMsec);
}

void MainWindow::UpdateImpact(){
    RemoteControl joystick;
    sf::Joystick::update();
    ui->label_impactDataMarch->setText(QString::number(joystick.getMarch()));
    ui->label_impactDataDepth->setText(QString::number(joystick.getDepth()));
    ui->label_impactDataRoll->setText(QString::number(joystick.getRoll()));
    ui->label_impactDataPitch->setText(QString::number(joystick.getPitch()));
    ui->label_impactDataYaw->setText(QString::number(joystick.getYaw()));
}
