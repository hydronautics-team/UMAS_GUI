#include "mainwindow.h"
#include "./ui_mainwindow.h"


// #include <QtCharts/QScatterSeries>
// #include <QtCharts/QLegendMarker>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtCore/QtMath>
// #include <QtCharts/QChartView>
// #include <QtCharts/QAbstractAxis>
// #include <QtCharts/QAbstractSeries>
// #include <QtCharts/QChart>
#include <QtWidgets/QGraphicsView>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    updateTimer = new QTimer(this);

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
    ui->tabWidget->setTabText(1, "БСО");
    ui->tabWidget->setTabText(0, "Карта");

// //map

//     auto series0 = new QScatterSeries;
//     series0->setName("Агент - 1");
//     series0->setMarkerShape(QScatterSeries::MarkerShapeCircle);
//     series0->setMarkerSize(15.0);

//     series0->append(3, 8);


//     auto chart = new QChart;
//     chart->addSeries(series0);
//     chart->createDefaultAxes();
//     chart->setDropShadowEnabled(false);

//     chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

//     QChartView *chartView = new QChartView(chart);

//     ui->verticalLayout_forMap->addWidget(chartView);

// //
    connect(
        ui->pushButton_modeManual, SIGNAL(clicked()),
        this, SLOT(e_CSModeManualToggled()));

    connect(
        ui->pushButton_modeAutomated, SIGNAL(clicked()),
        this, SLOT(e_CSModeAutomatedToggled()));

    connect(
        ui->pushButton_modeAutomated_gamma, SIGNAL(toggled(bool)),
        this, SLOT(stabilizeRollToggled(bool)));

    connect(
        ui->pushButton_modeAutomated_lag, SIGNAL(toggled(bool)),
        this, SLOT(stabilizeLagToggled(bool)));

    connect(
        ui->pushButton_modeAutomated_march, SIGNAL(toggled(bool)),
        this, SLOT(stabilizeMarchToggled(bool)));

    connect(
        ui->pushButton_modeAutomated_psi, SIGNAL(toggled(bool)),
        this, SLOT(stabilizeYawToggled(bool)));

    connect(
        ui->pushButton_modeAutomated_tetta, SIGNAL(toggled(bool)),
        this, SLOT(stabilizePitchToggled(bool)));

    connect(
        ui->comboBox_modeSelection, SIGNAL(activated(int)),
        this, SLOT(setModeSelection(int)));

    connect(
        ui->pushButton_connection, SIGNAL(clicked()),
        this, SLOT(setConnection()));

    connect(this, SIGNAL(updateCompass(float)),
            this, SLOT(updateUi_Compass(float)));
    connect(this, SIGNAL(updateIMU(DataAH127C)),
            this, SLOT(updateUi_IMU(DataAH127C)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerUpdateImpact(int periodUpdateMsec){
    //QTimer *updateTimer = new QTimer(this);
    connect(
        updateTimer, SIGNAL(timeout()),
        this, SLOT(updateUi_fromControl())
        );
    updateTimer->start(periodUpdateMsec);

}

void MainWindow::updateUi_fromControl(){
    ControlData control = uv_interface.getControlData();
    DataAH127C imuData = uv_interface.getImuData();
    bool mode = uv_interface.getCSMode();

    ui->label_impactDataDepth->setText(QString::number(control.march, 'f', 2));
    ui->label_impactDataRoll->setText(QString::number(control.roll, 'f', 2));
    ui->label_impactDataPitch->setText(QString::number(control.pitch, 'f', 2));
    ui->label_impactDataYaw->setText(QString::number(control.yaw, 'f', 2));

    ui->compass->setYawDesirable(control.yaw, imuData.yaw, mode);
}

void MainWindow::updateUi_Compass(float yaw) {
    ui->compass->setYaw(yaw);
}

void MainWindow::updateUi_IMU(DataAH127C imuData){
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

    ui->label_IMUdata_yaw->setText(QString::number(imuData.yaw, 'f', 2));
    ui->label_IMUdata_pitch->setText(QString::number(imuData.pitch, 'f', 2));
    ui->label_IMUdata_roll->setText(QString::number(imuData.roll, 'f', 2));
}

void MainWindow::setConnection()
{
    pultProtocol = new Pult::PC_Protocol(QHostAddress("192.168.137.2"), 13051,
                                         QHostAddress("192.168.137.3"), 13050, 10);

    qDebug() << "-----start exchange";
    pultProtocol->startExchange();

    connect(pultProtocol, SIGNAL(dataReceived()),
            this, SLOT(updateUi_fromAgent()));
}

void MainWindow::stabilizeMarchToggled(bool state) {
    uv_interface.setControlContoursFlags(e_StabilizationContours::CONTOUR_MARCH, !state);
}

void MainWindow::stabilizeYawToggled(bool state) {
    uv_interface.setControlContoursFlags(e_StabilizationContours::CONTOUR_YAW, !state);
}

void MainWindow::stabilizePitchToggled(bool state) {
    uv_interface.setControlContoursFlags(e_StabilizationContours::CONTOUR_PITCH, !state);
}

void MainWindow::stabilizeRollToggled(bool state) {
    uv_interface.setControlContoursFlags(e_StabilizationContours::CONTOUR_ROLL, !state);
}


void MainWindow::stabilizeLagToggled(bool state) {
    uv_interface.setControlContoursFlags(e_StabilizationContours::CONTOUR_LAG, !state);
}

void MainWindow::e_CSModeManualToggled() {
    uv_interface.setCSMode(e_CSMode::MODE_MANUAL);
}

void MainWindow::e_CSModeAutomatedToggled() {
    uv_interface.setCSMode(e_CSMode::MODE_AUTOMATED);
}

void MainWindow::setModeSelection(int index)
{
    if (index == 1)
        uv_interface.setModeSelection(false);
    else
        uv_interface.setModeSelection(true);

}

void MainWindow::updateUi_fromAgent() {
    DataAH127C imuData = uv_interface.getImuData();

    emit updateCompass(imuData.yaw);
    emit updateIMU(imuData);
}
