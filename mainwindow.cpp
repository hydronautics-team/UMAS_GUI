#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWidget();
    setInterface();
    setConsole();
    setTimer_updateImpact(20);
    setBottom();
    setTab();
    setUpdateUI();
}

//

void MainWindow::setWidget()
{
    powerSystem = new PowerSystem(this);
    ui->horizontalLayout_for_powerSystem->addWidget(powerSystem);
    checkMsg = new CheckMsg(this);
    ui->horizontalLayout_for_checkMsg->addWidget(checkMsg);
    checkImu = new CheckImu(this);
    ui->horizontalLayout_for_checkImu->addWidget(checkImu);
    modeAutomatic = new ModeAutomatic(this);
    ui->verticalLayout_modeAutomatic->addWidget(modeAutomatic);
    diagnostic_board = new Diagnostic_board(this);
    ui->horizontalLayout_diagnosticBoard->addWidget(diagnostic_board);

    // setMission_map
    connect(
        modeAutomatic->ui->pushButton_missionPlanning_goto_clean, &QPushButton::clicked,
        ui->map, &Map::updateUi_missionPlanning_goto_goal_clear);
    connect(
        modeAutomatic->ui->pushButton_missionPlanning_goto_on_trajectory, &QPushButton::clicked,
        ui->map, &Map::updateUi_missionPlanning_goto_traj_onoff);
    connect(
        modeAutomatic->ui->pushButton_missionPlanning_goto_on_trajectory_clear, &QPushButton::clicked,
        ui->map, &Map::updateUi_missionPlanning_goto_traj_clear);
    // setMission_goTo
    connect(
        modeAutomatic, &ModeAutomatic::signal_pushButton_missionPlanning_goto_updateMap,
        ui->map, &Map::updateUi_missionPlanning_goto_goal);
    // setMission_go_trajectory
    connect(
        modeAutomatic, &ModeAutomatic::signal_pushButton_missionPlanning_go_trajectory_updateMap,
        ui->map, &Map::updateUi_missionPlanning_goto_goal);
    connect(
        modeAutomatic->ui->pushButton_missionPlanning_go_trajectory_clean, &QPushButton::clicked,
        ui->map, &Map::updateUi_missionPlanning_goto_goal_clear);
    // setMission_cpp
    connect(
        ui->map, &Map::pointAdded,
        modeAutomatic, &ModeAutomatic::addPointToTable);
    connect(
        modeAutomatic, &ModeAutomatic::requestUpdateChart,
        ui->map, &Map::updateChart);
    connect(
        modeAutomatic, &ModeAutomatic::requestClearLines,
        ui->map, &Map::clearLines);
    connect(
        modeAutomatic, &ModeAutomatic::plotLineSeries,
        ui->map, &Map::onPlotLineSeries);
        // Подключение кнопки для переключения состояния
    connect(
        modeAutomatic->ui->pushButton_missionPlanning_cpp_on_off, &QPushButton::toggled,
        ui->map, &Map::setMissionPlanning_cpp_Enabled);
    connect(
        modeAutomatic,&ModeAutomatic::displayText_toConsole,
        this, &MainWindow::displayText);
    connect(
        modeAutomatic, &ModeAutomatic::set_stackedWidget_mode,
        ui->stackedWidget_mode, &QStackedWidget::setCurrentIndex);

    MapWidget *mapWidget = new MapWidget(this);
    ui->horizontalLayout_mapWidget->addWidget(mapWidget);

    connect(
        modeAutomatic->ui->pushButton_missionPlanning_cpp_on_off, &QPushButton::toggled,
        mapWidget, &MapWidget::toggleAddPointMode);
    connect(
        modeAutomatic, &ModeAutomatic::requestClearLines,
        mapWidget, &MapWidget::clearMapItems);
//    connect(
//        modeAutomatic->ui->pushButton_missionPlanning_cpp_make, &QPushButton::clicked,
//        mapWidget, &MapWidget::getAllPoints);
    connect(
        mapWidget, &MapWidget::signal_addPointToTable,
        modeAutomatic, &ModeAutomatic::addPointToTable);
    connect(
        modeAutomatic, &ModeAutomatic::requestAddLine,
        mapWidget, &MapWidget::addLine);

//    // Пример добавления точки
//    QGeoCoordinate point(55.769975, 37.690433);
//    emit mapWidget->addPoint(point);

//    // Пример добавления линии
//    QVector<QGeoCoordinate> path;
//    path << QGeoCoordinate(55.7558, 37.6173) << QGeoCoordinate(55.769975, 37.690433);
//    emit mapWidget->addLine(path);



}

void MainWindow::setInterface()
{
    connect(
        this, &MainWindow::signal_setInterface,
        powerSystem, &PowerSystem::slot_getInterface);
    connect(
        this, &MainWindow::signal_setInterface,
        checkMsg, &CheckMsg::slot_getInterface);
    connect(
        this, &MainWindow::signal_setInterface,
        modeAutomatic, &ModeAutomatic::slot_getInterface);

    signal_setInterface(&uv_interface);

}

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

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
  keyBoard->keyReleaseEvent(event);
}

void MainWindow::updateUi_fromControl(){

    ControlData control = uv_interface.getControlData();
    DataAH127C imuData = uv_interface.getImuData();

    ui->compass->setYawDesirable(control.yaw, imuData.yaw, uv_interface.getCSMode());

    ui->label_controlYaw->setNum(control.yaw);
    ui->label_controlMarch->setNum(control.march);
}

//

void MainWindow::setBottom()
{
    setBottom_mode();
    setBottom_connection();
    setBottom_modeSelection();
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
    QString ip_pult = ui->lineEdit_ip_pult->text();
    QString ip_agent = ui->lineEdit_ip_agent->text();
    communicationAgent1 = new Pult::PC_Protocol(QHostAddress(ip_pult), 13053,
                                                QHostAddress(ip_agent), 13050, 10, 0);
    communicationAgent1->startExchange();

    if (communicationAgent1->bindState()) {
        displayText("Соединение установлено");
        ui->pushButton_selectAgent1->setStyleSheet("background-color: green");
        connect(communicationAgent1, SIGNAL(dataReceived()),
                this, SLOT(updateUi_fromAgent1()));
        updateStatePushButton();
    } else {
        ui->pushButton_connection->setEnabled(true);
        ui->pushButton_breakConnection->setEnabled(false);
        ui->pushButton_selectAgent1->setStyleSheet("background-color: red");
        displayText("Попробуйте снова");
    }
}

void MainWindow::updateUi_fromAgent1() {
    DataAH127C imuData = uv_interface.getImuData();
    displayText("DEBUG: сообщение получено");

    emit updateCompass(imuData.yaw);
    emit updateIMU(imuData);
    emit updateSetupMsg();
    emit updateDataMission();
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
        uv_interface.setModeSelection(0); //агент
    }
    else{
        uv_interface.setModeSelection(1); //модель
    }
}


//



void MainWindow::setBottom_selectAgent()
{
    ui->pushButton_selectAgent1->setCheckable(true);
    QButtonGroup *buttonGroup_selectAgent = new QButtonGroup(this);
    buttonGroup_selectAgent->addButton(ui->pushButton_selectAgent1);
    buttonGroup_selectAgent->setExclusive(true);

    ui->pushButton_selectAgent1->setChecked(true);

    connect(ui->pushButton_selectAgent1, &QAbstractButton::toggled,
            this, &MainWindow::pushButton_selectAgent1);
    connect(this, &MainWindow::updateStatePushButton,
            this, &MainWindow::updateUi_statePushButton);
}

void MainWindow::pushButton_selectAgent1(bool stateBottom)
{
    if (stateBottom){
        uv_interface.setCurrentAgent(0);
        displayText("Установлен ввод и вывод данных с агента 1");
    }
    updateStatePushButton();
}


void MainWindow::updateUi_statePushButton()
{
    // вывод данных: модель/агент
    if (uv_interface.getModeSelection())
        ui->comboBox_modeSelection->setCurrentIndex(0);
    else
        ui->comboBox_modeSelection->setCurrentIndex(1);


    //
    switch (uv_interface.getCSMode()) {
    case e_CSMode::MODE_MANUAL:
        ui->pushButton_modeManual->setChecked(true);
        break;
    case e_CSMode::MODE_AUTOMATED:
        ui->pushButton_modeAutomated->setChecked(true);
        break;
    case e_CSMode::MODE_AUTOMATIC:
        ui->pushButton_modeAutomatic->setChecked(true);
        break;
    }
}

void MainWindow::setTab()
{
    ui->tabWidget->setTabText(0, "Карта");
    ui->tabWidget->setTabText(1, "БСО");
    ui->tabWidget->setTabText(2,  "Контроль сообщений");
    ui->tabWidget->setTabText(3,  "Режимы питания");
}

void MainWindow::setUpdateUI()
{
    connect(this, SIGNAL(updateCompass(float)),
            this, SLOT(updateUi_Compass(float)));
    connect(this, SIGNAL(updateIMU(DataAH127C)),
            checkImu, SLOT(updateUi_imu(DataAH127C)));
    connect(this, SIGNAL(updateSetupMsg()),
            checkMsg, SLOT(updateUi_checkMsg()));
    connect(this, SIGNAL(updateDataMission()),
            modeAutomatic, SLOT(updateUi_DataMission()));
}

void MainWindow::updateUi_Compass(float yaw) {
    ui->compass->setYaw(yaw);
}

MainWindow::~MainWindow()
{
    delete ui;
}
