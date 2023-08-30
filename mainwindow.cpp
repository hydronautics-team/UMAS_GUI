#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qInfo() << "Приложение работает";

    setTimer_updateImpact(joystick.periodUpdateMsec);

    setBottom(ui, this);
    setTab(ui);

    connect(this, SIGNAL(updateCompass(float)),
            this, SLOT(updateUi_Compass(float)));
    connect(this, SIGNAL(updateIMU(DataAH127C)),
            this, SLOT(updateUi_IMU(DataAH127C)));
    connect(this, SIGNAL(updateSetupMsg()),
            this, SLOT(updateUi_SetupMsg()));
}

void MainWindow::setBottom(Ui::MainWindow *ui, QObject *ts)
{
    setBottom_mode(ui, ts);
    setBottom_modeAutomated(ui, ts);
    setBottom_powerMode(ui, ts);
    setBottom_connection(ui, ts);
    setBottom_modeSelection(ui, ts);
    setBottom_setupIMU(ui, ts);
    setBottom_setupIMU_check(ui, ts);
}

void MainWindow::setBottom_mode(Ui::MainWindow *ui, QObject *ts)
{
    ui->pushButton_modeManual->setCheckable(true);
    ui->pushButton_modeAutomated->setCheckable(true);
    QButtonGroup *mode = new QButtonGroup(ts);
    mode->addButton(ui->pushButton_modeManual);
    mode->addButton(ui->pushButton_modeAutomated);
    mode->setExclusive(true);

    ui->pushButton_modeManual->setChecked(true);
    e_CSModeManualToggled();

    connect(
        ui->pushButton_modeManual, SIGNAL(clicked()),
        ts, SLOT(e_CSModeManualToggled()));

    connect(
        ui->pushButton_modeAutomated, SIGNAL(clicked()),
        ts, SLOT(e_CSModeAutomatedToggled()));
}

void MainWindow::setBottom_modeAutomated(Ui::MainWindow *ui, QObject *ts)
{
    ui->pushButton_modeAutomated_march->setCheckable(true);
    ui->pushButton_modeAutomated_lag->setCheckable(true);
    ui->pushButton_modeAutomated_psi->setCheckable(true);
    ui->pushButton_modeAutomated_tetta->setCheckable(true);
    ui->pushButton_modeAutomated_gamma->setCheckable(true);
    QButtonGroup *modeAutomated = new QButtonGroup(ts);
    modeAutomated->addButton(ui->pushButton_modeAutomated_march);
    modeAutomated->addButton(ui->pushButton_modeAutomated_lag);
    modeAutomated->addButton(ui->pushButton_modeAutomated_psi);
    modeAutomated->addButton(ui->pushButton_modeAutomated_tetta);
    modeAutomated->addButton(ui->pushButton_modeAutomated_gamma);
    modeAutomated->setExclusive(false);

    ui->pushButton_modeAutomated_gamma->setChecked(true);
    ui->pushButton_modeAutomated_lag->setChecked(true);
    ui->pushButton_modeAutomated_march->setChecked(true);
    ui->pushButton_modeAutomated_psi->setChecked(true);
    ui->pushButton_modeAutomated_tetta->setChecked(true);

    connect(
        ui->pushButton_modeAutomated_gamma, SIGNAL(toggled(bool)),
        ts, SLOT(stabilizeRollToggled(bool)));

    connect(
        ui->pushButton_modeAutomated_lag, SIGNAL(toggled(bool)),
        ts, SLOT(stabilizeLagToggled(bool)));

    connect(
        ui->pushButton_modeAutomated_march, SIGNAL(toggled(bool)),
        ts, SLOT(stabilizeMarchToggled(bool)));

    connect(
        ui->pushButton_modeAutomated_psi, SIGNAL(toggled(bool)),
        ts, SLOT(stabilizeYawToggled(bool)));

    connect(
        ui->pushButton_modeAutomated_tetta, SIGNAL(toggled(bool)),
        ts, SLOT(stabilizePitchToggled(bool)));

}

void MainWindow::setBottom_powerMode(Ui::MainWindow *ui, QObject *ts)
{
    QButtonGroup *powerMode = new QButtonGroup(ts);
    powerMode->addButton(ui->pushButton_powerMode_2);
    powerMode->addButton(ui->pushButton_powerMode_3);
    powerMode->addButton(ui->pushButton_powerMode_4);
    powerMode->addButton(ui->pushButton_powerMode_5);

    ui->pushButton_powerMode_2->setCheckable(true);
    ui->pushButton_powerMode_3->setCheckable(true);
    ui->pushButton_powerMode_4->setCheckable(true);
    ui->pushButton_powerMode_5->setCheckable(true);

    ui->pushButton_powerMode_2->setChecked(true);

    uv_interface.setPowerMode(power_Mode::MODE_2);

    connect(
        ui->pushButton_powerMode_2, SIGNAL(clicked()),
        ts, SLOT(pushButton_on_powerMode_2()));

    connect(
        ui->pushButton_powerMode_3, SIGNAL(clicked()),
        ts, SLOT(pushButton_on_powerMode_3()));

    connect(
        ui->pushButton_powerMode_4, SIGNAL(clicked()),
        ts, SLOT(pushButton_on_powerMode_4()));

    connect(
        ui->pushButton_powerMode_5, SIGNAL(clicked()),
        ts, SLOT(pushButton_on_powerMode_5()));
}

void MainWindow::setBottom_connection(Ui::MainWindow *ui, QObject *ts)
{
    ui->pushButton_breakConnection->setEnabled(false);

    connect(
        ui->pushButton_connection, SIGNAL(clicked()),
        ts, SLOT(setConnection()));
    connect(
        ui->pushButton_breakConnection, SIGNAL(clicked()),
        ts, SLOT(breakConnection()));
}

void MainWindow::setBottom_modeSelection(Ui::MainWindow *ui, QObject *ts)
{
    setModeSelection(0);
    connect(
        ui->comboBox_modeSelection, SIGNAL(activated(int)),
        ts, SLOT(setModeSelection(int)));
}

void MainWindow::setBottom_setupIMU(Ui::MainWindow *ui, QObject *ts)
{
    connect(
        ui->pushButton_setupIMU, SIGNAL(clicked()),
        ts, SLOT(getWindow_setupIMU()));
}

void MainWindow::setBottom_setupIMU_check(Ui::MainWindow *ui, QObject *ts)
{
    connect(
        ui->pushButton_setupIMU_check, SIGNAL(clicked()),
        ts, SLOT(getWindow_setupIMU_check()));
}

void MainWindow::setTab(Ui::MainWindow *ui)
{
    ui->tabWidget->setTabText(0, "Карта");
    ui->tabWidget->setTabText(1, "БСО");
    ui->tabWidget->setTabText(2,  "Контроль сообщений");
    ui->tabWidget->setTabText(3,  "Режимы питания");
}

void MainWindow::setTimer_updateImpact(int periodUpdateMsec){
    QTimer *updateTimer = new QTimer(this);
    connect(
        updateTimer, SIGNAL(timeout()),
        this, SLOT(updateUi_fromControl())
        );
    updateTimer->start(periodUpdateMsec);
    qInfo() << "Таймер обновления джойстика запущен";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateUi_fromAgent() {
    DataAH127C imuData = uv_interface.getImuData();

    emit updateCompass(imuData.yaw);
    emit updateIMU(imuData);
    emit updateSetupMsg();
}

void MainWindow::updateUi_fromControl(){

    ControlData control = uv_interface.getControlData();
    DataAH127C imuData = uv_interface.getImuData();
    bool mode = uv_interface.getCSMode();

    ui->label_impactDataDepth->setText(QString::number(control.depth, 'f', 2));
    ui->label_impactDataRoll->setText(QString::number(control.roll, 'f', 2));
    ui->label_impactDataPitch->setText(QString::number(control.pitch, 'f', 2));
    ui->label_impactDataYaw->setText(QString::number(control.yaw, 'f', 2));
    ui->label_impactDataMarch->setText(QString::number(control.march, 'f', 2));
    ui->label_impactDataLag->setText(QString::number(control.lag, 'f', 2));

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

void MainWindow::updateUi_SetupMsg()
{
    power_Mode pMode = uv_interface.getPowerMode();
    bool modeSelection = uv_interface.getModeSelection();
    bool CsMode = uv_interface.getCSMode();
    ControlContoursFlags controlContoursFlags = uv_interface.getControlContoursFlags();
    ControlData control = uv_interface.getControlData();
    AUVCurrentData auvData = uv_interface.getAUVCurrentData();
    FlagAH127C_bort flagAH127C_bort = uv_interface.getFlagAH127C_bort();
    FlagAH127C_pult flagAH127C_pult = uv_interface.getFlagAH127C_pult();
    int checksum_msg_gui_send = uv_interface.getChecksumMsgGuiSend();
    int checksum_msg_agent_send = uv_interface.getChecksumMsgAgentSend();
    int checksum_msg_gui_received = uv_interface.getChecksumMsgGuiReceived();

//    send

    ui->label_tab_setupMsg_send_powerMode_count->setNum(static_cast<int>(pMode));

    if (modeSelection == 1)
        ui->label_tab_setupMsg_send_modeAUV_selection_mode->setText("модель");
    else
        ui->label_tab_setupMsg_send_modeAUV_selection_mode->setText("агент");

    if (static_cast<int>(CsMode)){
        ui->label_tab_setupMsg_send_cSMode_count->setText("автоматизированный");
    } else {
        ui->label_tab_setupMsg_send_cSMode_count->setText("ручной");
    }

    if (controlContoursFlags.yaw) {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_yaw->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_yaw->setText("незамкнут");
    }

    if (controlContoursFlags.pitch) {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_pitch->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_pitch->setText("незамкнут");
    }

    if (controlContoursFlags.roll) {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_roll->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_roll->setText("незамкнут");
    }

    if (controlContoursFlags.march) {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_march->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_march->setText("незамкнут");
    }

    if (controlContoursFlags.lag) {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_lag->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_lag->setText("незамкнут");
    }

    ui->label_tab_setupMsg_send_Impact_data_count_yaw->setNum(control.yaw);
    ui->label_tab_setupMsg_send_Impact_data_count_pitch->setNum(control.pitch);
    ui->label_tab_setupMsg_send_Impact_data_count_roll->setNum(control.roll);
    ui->label_tab_setupMsg_send_Impact_data_count_march->setNum(control.march);
    ui->label_tab_setupMsg_send_Impact_data_count_lag->setNum(control.lag);
    ui->label_tab_setupMsg_send_Impact_data_count_depth->setNum(control.depth);

//    received

    if (auvData.modeAUV_Real == 1)
        ui->labelt_tab_setupMsg_received_modeAUV_selection_mode->setText("модель");
    else
        ui->labelt_tab_setupMsg_received_modeAUV_selection_mode->setText("агент");

    if (auvData.modeReal) {
        ui->label_tab_setupMsg_received_cSMode_count->setText("автоматизированный");
    } else {
        ui->label_tab_setupMsg_received_cSMode_count->setText("ручной");
    }

    if (auvData.controlReal.yaw) {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_yaw->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_yaw->setText("незамкнут");
    }

    if (auvData.controlReal.pitch) {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_pitch->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_pitch->setText("незамкнут");
    }

    if (auvData.controlReal.roll) {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_roll->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_roll->setText("незамкнут");
    }

    if (auvData.controlReal.march) {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_march->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_march->setText("незамкнут");
    }

    if (auvData.controlReal.lag) {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_lag->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_lag->setText("незамкнут");
    }

    ui->label_tab_setupMsg_received_Impact_data_count_vma1->setNum(auvData.signalVMA_real.VMA1);
    ui->label_tab_setupMsg_received_Impact_data_count_vma2->setNum(auvData.signalVMA_real.VMA2);
    ui->label_tab_setupMsg_received_Impact_data_count_vma3->setNum(auvData.signalVMA_real.VMA3);
    ui->label_tab_setupMsg_received_Impact_data_count_vma4->setNum(auvData.signalVMA_real.VMA4);
    ui->label_tab_setupMsg_received_Impact_data_count_vma5->setNum(auvData.signalVMA_real.VMA5);
    ui->label_tab_setupMsg_received_Impact_data_count_vma6->setNum(auvData.signalVMA_real.VMA6);

//    флаги для настройки БСО

    ui->label_tab_setupMsg_flagsSetupIMU_pult_init->setNum(flagAH127C_pult.initCalibration);
    ui->label_tab_setupMsg_flagsSetupIMU_pult_save->setNum(flagAH127C_pult.saveCalibration);
    ui->label_tab_setupMsg_flagsSetupIMU_bort_end->setNum(flagAH127C_bort.startCalibration);
    ui->label_tab_setupMsg_flagsSetupIMU_bort_start->setNum(flagAH127C_bort.endCalibration);

//    количество посылок

    ui->label_tab_setupMsg_send_checksum_count->setNum(checksum_msg_gui_send);
    ui->label_tab_setupMsg_received_checksum_send_count->setNum(checksum_msg_agent_send);
    ui->labe_tab_setupMsg_received_checksum_received_count_->setNum(checksum_msg_gui_received);
}

void MainWindow::getWindow_setupIMU()
{
    SetupIMU window_setupIMU;
    window_setupIMU.setModal(false);
    window_setupIMU.exec();
}

void MainWindow::getWindow_setupIMU_check()
{
    SetupIMU_check window_setupIMU_check;
    window_setupIMU_check.setModal(false);
    window_setupIMU_check.exec();
}

void MainWindow::setConnection()
{
    ui->pushButton_connection->setEnabled(false);
    ui->pushButton_breakConnection->setEnabled(true);


    pultProtocol = new Pult::PC_Protocol(QHostAddress("192.168.137.2"), 13051,
                                         QHostAddress("192.168.137.11"), 13050, 10);

    pultProtocol->startExchange();

    if (pultProtocol->bindState()) {
        qDebug() << "-----start exchange";
        connect(pultProtocol, SIGNAL(dataReceived()),
                this, SLOT(updateUi_fromAgent()));
    } else {
        ui->pushButton_connection->setEnabled(true);
        ui->pushButton_breakConnection->setEnabled(false);
        qInfo() << "Попробуйте снова";
    }
}

void MainWindow::breakConnection()
{
    ui->pushButton_connection->setEnabled(true);
    qDebug() << "stop exchange";


    pultProtocol->stopExhange();
    delete pultProtocol;

    ui->pushButton_breakConnection->setEnabled(false);
}

void MainWindow::pushButton_on_powerMode_2()
{
    uv_interface.setPowerMode(power_Mode::MODE_2);
}

void MainWindow::pushButton_on_powerMode_3()
{
    uv_interface.setPowerMode(power_Mode::MODE_3);
}

void MainWindow::pushButton_on_powerMode_4()
{
    uv_interface.setPowerMode(power_Mode::MODE_4);
}

void MainWindow::pushButton_on_powerMode_5()
{
    before_powerMode = uv_interface.getPowerMode();
    uv_interface.setPowerMode(power_Mode::MODE_5);

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

void MainWindow::off_powerMode_5()
{
    timer_off_powerMode_5->stop();

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

void MainWindow::stabilizeMarchToggled(bool state) {
    uv_interface.setControlContoursFlags(e_StabilizationContours::CONTOUR_MARCH, state);
}

void MainWindow::stabilizeYawToggled(bool state) {
    uv_interface.setControlContoursFlags(e_StabilizationContours::CONTOUR_YAW, state);
}

void MainWindow::stabilizePitchToggled(bool state) {
    uv_interface.setControlContoursFlags(e_StabilizationContours::CONTOUR_PITCH, state);
}

void MainWindow::stabilizeRollToggled(bool state) {
    uv_interface.setControlContoursFlags(e_StabilizationContours::CONTOUR_ROLL, state);
}


void MainWindow::stabilizeLagToggled(bool state) {
    uv_interface.setControlContoursFlags(e_StabilizationContours::CONTOUR_LAG, state);
}

void MainWindow::e_CSModeManualToggled() {
    uv_interface.setCSMode(e_CSMode::MODE_MANUAL);
}

void MainWindow::e_CSModeAutomatedToggled() {
    uv_interface.setCSMode(e_CSMode::MODE_AUTOMATED);
}

void MainWindow::setModeSelection(int index)
{
    if (index == 1){
        uv_interface.setModeSelection(false); //агент
    }
    else{
        uv_interface.setModeSelection(true); //модель
    }
}

