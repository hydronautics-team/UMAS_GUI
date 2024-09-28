#include "power_system.h"
#include "ui_power_system.h"

PowerSystem::PowerSystem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PowerSystem)
{
    ui->setupUi(this);
    // режим питания
    switch (uv_interface->getPowerMode()) {
    case power_Mode::MODE_2:
        ui->pushButton_powerMode_2->setChecked(true);
        break;
    case power_Mode::MODE_3:
        ui->pushButton_powerMode_3->setChecked(true);
        break;
    case power_Mode::MODE_4:
        ui->pushButton_powerMode_4->setChecked(true);
        break;
    case power_Mode::MODE_5:
        ui->pushButton_powerMode_5->setChecked(true);
        break;
    }
    setBottom_powerMode();
}

void PowerSystem::setBottom_powerMode()
{
    QButtonGroup *powerMode = new QButtonGroup(this);
    powerMode->addButton(ui->pushButton_powerMode_2);
    powerMode->addButton(ui->pushButton_powerMode_3);
    powerMode->addButton(ui->pushButton_powerMode_4);
    powerMode->addButton(ui->pushButton_powerMode_5);

    ui->pushButton_powerMode_2->setCheckable(true);
    ui->pushButton_powerMode_3->setCheckable(true);
    ui->pushButton_powerMode_4->setCheckable(true);
    ui->pushButton_powerMode_5->setCheckable(true);

    uv_interface->setPowerMode(power_Mode::MODE_2);
    ui->pushButton_powerMode_2->setChecked(true);

    connect(
        ui->pushButton_powerMode_2, SIGNAL(clicked()),
        this, SLOT(pushButton_on_powerMode_2()));

    connect(
        ui->pushButton_powerMode_3, SIGNAL(clicked()),
        this, SLOT(pushButton_on_powerMode_3()));

    connect(
        ui->pushButton_powerMode_4, SIGNAL(clicked()),
        this, SLOT(pushButton_on_powerMode_4()));

    connect(
        ui->pushButton_powerMode_5, SIGNAL(clicked()),
        this, SLOT(pushButton_on_powerMode_5()));
}

void PowerSystem::pushButton_on_powerMode_2()
{
    uv_interface->setPowerMode(power_Mode::MODE_2);
}

void PowerSystem::pushButton_on_powerMode_3()
{
    uv_interface->setPowerMode(power_Mode::MODE_3);
}

void PowerSystem::pushButton_on_powerMode_4()
{
    uv_interface->setPowerMode(power_Mode::MODE_4);
}

void PowerSystem::pushButton_on_powerMode_5()
{
    before_powerMode = uv_interface->getPowerMode();
    uv_interface->setPowerMode(power_Mode::MODE_5);

    ui->pushButton_powerMode_2->setEnabled(false);
    ui->pushButton_powerMode_3->setEnabled(false);
    ui->pushButton_powerMode_4->setEnabled(false);
    ui->pushButton_powerMode_5->setEnabled(false);

    timer_off_powerMode_5 = new QTimer(this);
    connect(
        timer_off_powerMode_5, SIGNAL(timeout()),
        this, SLOT(off_powerMode_5()));
    timer_off_powerMode_5->start(5000);
}

void PowerSystem::off_powerMode_5()
{
    timer_off_powerMode_5->stop();
    uv_interface->setPowerMode(before_powerMode);

    switch (before_powerMode) {
    case power_Mode::MODE_2:
        ui->pushButton_powerMode_2->setChecked(true);
        break;

    case power_Mode::MODE_3:
        ui->pushButton_powerMode_3->setChecked(true);
        break;

    case power_Mode::MODE_4:
        ui->pushButton_powerMode_4->setChecked(true);
        break;
    }

    ui->pushButton_powerMode_2->setEnabled(true);
    ui->pushButton_powerMode_3->setEnabled(true);
    ui->pushButton_powerMode_4->setEnabled(true);
    ui->pushButton_powerMode_5->setEnabled(true);
}

void PowerSystem::slot_getInterface(IUserInterfaceData *interface)
{
    uv_interface = interface;
}



PowerSystem::~PowerSystem()
{
    delete ui;
}
