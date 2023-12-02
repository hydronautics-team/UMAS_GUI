#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setConsole();
    setTimer_updateImpact(20);
    setBottom();
    setTab();
    setMap();
    setUpdateUI();
}

//

void MainWindow::setConsole()
{
    displayText("Приложение работает");
    displayText("Установите соединение для работы с агентом");

    connect(&uv_interface, SIGNAL(displayText_toConsole(QString)),
            this, SLOT(displayText(QString)));
}

void MainWindow::displayText(QString str)
{
    QString currentTime = QTime::currentTime().toString("HH:mm:ss");
    qInfo() << currentTime << str;
    ui->textEdit_console->append(currentTime + " " + str);
}

//

void MainWindow::setTimer_updateImpact(int periodUpdateMsec)
{
    joyStick = new JoyStick(this);
    connect(ui->radioButton_useJoyStick, &QRadioButton::clicked,
            this, &MainWindow::useJoyStick);
    connect(ui->radioButton_useKeyBoard, &QRadioButton::clicked,
            this, &MainWindow::useKeyBoard);
    QTimer *updateTimer = new QTimer(this);
    connect(
        updateTimer, SIGNAL(timeout()),
        this, SLOT(updateUi_fromControl())
        );
    updateTimer->start(periodUpdateMsec);
    displayText("Таймер обновления джойстика запущен");
}

void MainWindow::useKeyBoard()
{
    delete joyStick;

    keyBoard = new KeyBoard(this);
    displayText("Используемые клавиши(должна быть английская раскладка):\n"
                "Клавиша O - вперед по маршу\n"
                "Клавиша L - назад по маршу\n"
                "Клавиша W - вниз по дифференту\n"
                "Клавиша S - вверх по дифференту\n"
                "Клавиша A - влево по курсу\n"
                "Клавиша D - вправо по курсу\n"
                "Клавиша C - вниз по глубине\n"
                "Клавиша V - вверх по глубине\n"
                "Клавиша Q - влево по крену\n"
                "Клавиша E - вправо по крену\n"
                "Клавиша K - влево по лагу\n"
                "Клавиша ; - вправо по лагу\n");

}

void MainWindow::useJoyStick()
{
    delete keyBoard;

    joyStick = new JoyStick(this);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    keyBoard->keyPressEvent(event);
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

//

void MainWindow::setBottom()
{
    setBottom_mode();
    setBottom_modeAutomated();
    setBottom_modeAutomatic();
    setBottom_powerMode();
    setBottom_connection();
    setBottom_modeSelection();
    setBottom_setupIMU();
    setBottom_setupIMU_check();
    setBottom_selectAgent();
}

void MainWindow::setBottom_mode()
{
    ui->pushButton_modeManual->setCheckable(true);
    ui->pushButton_modeAutomated->setCheckable(true);
    ui->pushButton_modeAutomatic->setCheckable(true);
    QButtonGroup *mode = new QButtonGroup(this);
    mode->addButton(ui->pushButton_modeManual);
    mode->addButton(ui->pushButton_modeAutomated);
    mode->addButton(ui->pushButton_modeAutomatic);
    mode->setExclusive(true);

    ui->pushButton_modeManual->setChecked(true);
    e_CSModeManualToggled();

    connect(
        ui->pushButton_modeManual, SIGNAL(clicked()),
        this, SLOT(e_CSModeManualToggled()));

    connect(
        ui->pushButton_modeAutomated, SIGNAL(clicked()),
        this, SLOT(e_CSModeAutomatedToggled()));

    connect(
        ui->pushButton_modeAutomatic, SIGNAL(clicked()),
        this, SLOT(e_CSModeAutomaticToggled()));
}

void MainWindow::e_CSModeManualToggled() {
    uv_interface.setCSMode(e_CSMode::MODE_MANUAL);
}

void MainWindow::e_CSModeAutomatedToggled() {
    uv_interface.setCSMode(e_CSMode::MODE_AUTOMATED);
}

void MainWindow::e_CSModeAutomaticToggled()
{
    uv_interface.setCSMode(e_CSMode::MODE_AUTOMATIC);
    ui->stackedWidget_mode->setCurrentIndex(1);
}

void MainWindow::setBottom_modeAutomatic()
{
    connect(
        ui->pushButton_after, SIGNAL(clicked()),
        this, SLOT(test_automatic_after()));

    connect(
        ui->pushButton_missionControl_modeIdle, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionControl_modeIdle);
    connect(
        ui->pushButton_missionControl_modeStart, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionControl_modeStart);
    connect(
        ui->pushButton_missionControl_modeCancel, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionControl_modeComplete);
    connect(
        ui->pushButton_missionControl_modeStop, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionControl_modeStop);
    connect(
        ui->pushButton_missionControl_modeComplete, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionControl_modeCancel);

    connect(
        ui->pushButton_missionPlanning_goto, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionPlanning_goto);
    connect(
        ui->pushButton_missionPlanning_following, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionPlanning_following);

}

void MainWindow::test_automatic_after()
{
    ui->stackedWidget_mode->setCurrentIndex(0);
}

void MainWindow::slot_pushButton_missionControl_modeIdle()
{
    uv_interface.setMissionControl(mission_Control::MODE_IDLE);
}
void MainWindow::slot_pushButton_missionControl_modeStart()
{
    uv_interface.setMissionControl(mission_Control::MODE_START);
}
void MainWindow::slot_pushButton_missionControl_modeCancel()
{
    uv_interface.setMissionControl(mission_Control::MODE_CANCEL);
}
void MainWindow::slot_pushButton_missionControl_modeStop()
{
    uv_interface.setMissionControl(mission_Control::MODE_STOP);
}
void MainWindow::slot_pushButton_missionControl_modeComplete()
{
    uv_interface.setMissionControl(mission_Control::MODE_COMPLETE);
}

void MainWindow::slot_pushButton_missionPlanning_goto()
{
    uv_interface.setID_mission_AUV(1);
}

void MainWindow::slot_pushButton_missionPlanning_following()
{
    uv_interface.setID_mission_AUV(2);
}

void MainWindow::updateUi_DataMission()
{
    int missionStatus = static_cast<int>(uv_interface.getMissionStatus());
    switch (missionStatus) {
    case 0:
        ui->label_missonStatus->setText("ожидание");
        break;
    case 1:
        ui->label_missonStatus->setText("ошибка инициализации миссии");
        break;
    case 2:
        ui->label_missonStatus->setText("миссия запущена и выполняется");
        break;
    case 3:
        ui->label_missonStatus->setText("миссия приостановлена, на паузе");
        break;
    case 4:
        ui->label_missonStatus->setText("миссия завершена");
        break;
    }
}

void MainWindow::setBottom_modeAutomated()
{
    ui->pushButton_modeAutomated_march->setCheckable(true);
    ui->pushButton_modeAutomated_lag->setCheckable(true);
    ui->pushButton_modeAutomated_psi->setCheckable(true);
    ui->pushButton_modeAutomated_tetta->setCheckable(true);
    ui->pushButton_modeAutomated_gamma->setCheckable(true);
    ui->pushButton_modeAutomated_depth->setCheckable(true);
    QButtonGroup *modeAutomated = new QButtonGroup(this);
    modeAutomated->addButton(ui->pushButton_modeAutomated_march);
    modeAutomated->addButton(ui->pushButton_modeAutomated_lag);
    modeAutomated->addButton(ui->pushButton_modeAutomated_psi);
    modeAutomated->addButton(ui->pushButton_modeAutomated_tetta);
    modeAutomated->addButton(ui->pushButton_modeAutomated_gamma);
    modeAutomated->addButton(ui->pushButton_modeAutomated_depth);
    modeAutomated->setExclusive(false);

    ui->pushButton_modeAutomated_gamma->setChecked(true);
    ui->pushButton_modeAutomated_lag->setChecked(true);
    ui->pushButton_modeAutomated_march->setChecked(true);
    ui->pushButton_modeAutomated_psi->setChecked(true);
    ui->pushButton_modeAutomated_tetta->setChecked(true);
    ui->pushButton_modeAutomated_depth->setChecked(true);

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
        ui->pushButton_modeAutomated_depth, SIGNAL(toggled(bool)),
        this, SLOT(stabilizeDepthToggled(bool)));
}

void MainWindow::stabilizeRollToggled(bool state) {
    uv_interface.setControlContoursFlags(e_StabilizationContours::CONTOUR_ROLL, state);
}

void MainWindow::stabilizeLagToggled(bool state) {
    uv_interface.setControlContoursFlags(e_StabilizationContours::CONTOUR_LAG, state);
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

void MainWindow::stabilizeDepthToggled(bool state) {
    uv_interface.setControlContoursFlags(e_StabilizationContours::CONTOUR_DEPTH, state);
}

void MainWindow::setBottom_powerMode()
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

    uv_interface.setPowerMode(power_Mode::MODE_2);
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
    uv_interface.setPowerMode(before_powerMode);

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

void MainWindow::setBottom_connection()
{
    ui->pushButton_breakConnection->setEnabled(false);

    connect(
        ui->pushButton_connection, SIGNAL(clicked()),
        this, SLOT(setConnection()));
    connect(
        ui->pushButton_breakConnection, SIGNAL(clicked()),
        this, SLOT(breakConnection()));
}

void MainWindow::setConnection()
{
    ui->pushButton_connection->setEnabled(false);
    ui->pushButton_breakConnection->setEnabled(true);

    communicationAgent2 = new Pult::PC_Protocol(QHostAddress("192.168.1.146"), 13053,
                                                QHostAddress("192.168.1.72"), 13052, 10, 0);
    communicationAgent1 = new Pult::PC_Protocol(QHostAddress("192.168.1.146"), 13051,
                                                QHostAddress("192.168.1.11"), 13050, 10, 1);

    communicationAgent1->startExchange();
    communicationAgent2->startExchange();

    if (communicationAgent1->bindState() || communicationAgent2->bindState()) {
        displayText("Соединение установлено");

        connect(communicationAgent1, SIGNAL(dataReceived()),
                this, SLOT(updateUi_fromAgent1()));
        connect(communicationAgent2, SIGNAL(dataReceived()),
                this, SLOT(updateUi_fromAgent2()));
    } else {
        ui->pushButton_connection->setEnabled(true);
        ui->pushButton_breakConnection->setEnabled(false);
        displayText("Попробуйте снова");
    }

    if (communicationAgent1->bindState()){
        ui->pushButton_selectAgent1->setStyleSheet("background-color: green");
        displayText("Соединение c 1 агентом установлено");
    } else {
        ui->pushButton_selectAgent1->setStyleSheet("background-color: red");
        displayText("Соединение c 1 агентом не установлено");
    }
    if (communicationAgent2->bindState()){
        ui->pushButton_selectAgent2->setStyleSheet("background-color: green");
        displayText("Соединение c 2 агентом установлено");
    } else {
        ui->pushButton_selectAgent2->setStyleSheet("background-color: red");
        displayText("Соединение c 2 агентом не установлено");
    }
}

void MainWindow::updateUi_fromAgent1() {
    DataAH127C imuData = uv_interface.getImuData();
    DataUWB dataUWB = uv_interface.getDataUWB();

    emit updateCompass(imuData.yaw);
    emit updateIMU(imuData);
    emit updateSetupMsg();
    emit updateMap(dataUWB);
    emit updateDataMission();
}

void MainWindow::updateUi_fromAgent2()
{
    DataUWB dataUWB_agent2 = uv_interface.getDataUWB(1);
    emit updateMapForAgent2(dataUWB_agent2);
}

void MainWindow::breakConnection()
{
    ui->pushButton_connection->setEnabled(true);

    displayText("Соединение разорвано");
    communicationAgent1->stopExhange();

    delete communicationAgent1;

    ui->pushButton_breakConnection->setEnabled(false);
}

//

void MainWindow::setBottom_modeSelection()
{
    setModeSelection(0);
    connect(
        ui->comboBox_modeSelection, SIGNAL(activated(int)),
        this, SLOT(setModeSelection(int)));
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

void MainWindow::setBottom_setupIMU()
{
    connect(
        ui->pushButton_setupIMU, SIGNAL(clicked()),
        this, SLOT(getWindow_setupIMU()));
}

void MainWindow::getWindow_setupIMU()
{
    SetupIMU window_setupIMU;
    window_setupIMU.setModal(false);
    window_setupIMU.exec();
}

//

void MainWindow::setBottom_setupIMU_check()
{
    connect(
        ui->pushButton_setupIMU_check, SIGNAL(clicked()),
        this, SLOT(getWindow_setupIMU_check()));
}

void MainWindow::setBottom_selectAgent()
{
    ui->pushButton_selectAgent1->setCheckable(true);
    ui->pushButton_selectAgent2->setCheckable(true);
    QButtonGroup *buttonGroup_selectAgent = new QButtonGroup(this);
    buttonGroup_selectAgent->addButton(ui->pushButton_selectAgent1);
    buttonGroup_selectAgent->addButton(ui->pushButton_selectAgent2);
    buttonGroup_selectAgent->setExclusive(true);

    ui->pushButton_selectAgent1->setChecked(true);

    connect(ui->pushButton_selectAgent1, &QAbstractButton::toggled,
            this, &MainWindow::pushButton_selectAgent1);
    connect(ui->pushButton_selectAgent2, &QAbstractButton::toggled,
            this, &MainWindow::pushButton_selectAgent2);
}

void MainWindow::pushButton_selectAgent1(bool stateBottom)
{
    if (stateBottom){
        uv_interface.setCurrentAgent(0);
        displayText("Установлен ввод и вывод данных с агента 1");
    }
}

void MainWindow::pushButton_selectAgent2(bool stateBottom)
{
    if (stateBottom){
        uv_interface.setCurrentAgent(1);
        displayText("Установлен ввод и вывод данных с агента 2");
    }
}

void MainWindow::getWindow_setupIMU_check()
{
    SetupIMU_check window_setupIMU_check;
    window_setupIMU_check.setModal(false);
    window_setupIMU_check.exec();
}

//

void MainWindow::setTab()
{
    ui->tabWidget->setTabText(0, "Карта");
    ui->tabWidget->setTabText(1, "БСО");
    ui->tabWidget->setTabText(2,  "Контроль сообщений");
    ui->tabWidget->setTabText(3,  "Режимы питания");
}

//

void MainWindow::setMap()
{
    connect(ui->map, SIGNAL(sendLocationUWB(double*,double*)),
            this, SLOT(setLocationUWB(double*,double*)));
}

void MainWindow::setLocationUWB(double *x, double *y)
{
    PultUWB pultUWB;

    for (int count = 0; count < 2; count++)
    {
        pultUWB.beacon_x[count] = *x;
        pultUWB.beacon_y[count] = *y;
        x++;
        y++;
    }

    uv_interface.setDataPultUWB(pultUWB);
}

//

void MainWindow::setUpdateUI()
{
    connect(this, SIGNAL(updateCompass(float)),
            this, SLOT(updateUi_Compass(float)));
    connect(this, SIGNAL(updateIMU(DataAH127C)),
            this, SLOT(updateUi_IMU(DataAH127C)));
    connect(this, SIGNAL(updateSetupMsg()),
            this, SLOT(updateUi_SetupMsg()));
    connect(this, SIGNAL(updateDataMission()),
            this, SLOT(updateUi_DataMission()));

    connect(this, SIGNAL(updateMap(DataUWB)),
            ui->map,SLOT(updateUi_map(DataUWB)));
    connect(this, SIGNAL(updateMapForAgent2(DataUWB)),
            ui->map,SLOT(updateUi_map2(DataUWB)));
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

    ui->label_tab_setupMsg_send_powerMode_count->setNum(2+static_cast<int>(pMode));

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

MainWindow::~MainWindow()
{
    delete ui;
}
