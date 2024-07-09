    #include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWidget();
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

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
  keyBoard->keyReleaseEvent(event);
}

void MainWindow::updateUi_fromControl(){

    ControlData control = uv_interface.getControlData();
    DataAH127C imuData = uv_interface.getImuData();

    ui->label_impactDataDepth->setText(QString::number(control.depth, 'f', 2));
    ui->label_impactDataRoll->setText(QString::number(control.roll, 'f', 2));
    ui->label_impactDataPitch->setText(QString::number(control.pitch, 'f', 2));
    ui->label_impactDataYaw->setText(QString::number(control.yaw, 'f', 2));
    ui->label_impactDataMarch->setText(QString::number(control.march, 'f', 2));
    ui->label_impactDataLag->setText(QString::number(control.lag, 'f', 2));

    ui->compass->setYawDesirable(control.yaw, imuData.yaw, uv_interface.getCSMode());
}

//

void MainWindow::setBottom()
{
    setBottom_mode();
    setBottom_modeAutomated();
    setBottom_modeAutomatic();
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
        this, &MainWindow::slot_pushButton_missionControl_modeCancel);
    connect(
        ui->pushButton_missionControl_modeStop, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionControl_modeStop);
    connect(
        ui->pushButton_missionControl_modeComplete, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionControl_modeComplete);

    connect(
        ui->pushButton_missionPlanning_goto, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionPlanning_goto);
    connect(
        ui->pushButton_missionPlanning_goto_update, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionPlanning_goto_update);
    connect(
        this, &MainWindow::signal_pushButton_missionPlanning_goto_updateMap,
        ui->map, &Map::updateUi_missionPlanning_goto_goal);
    connect(
        ui->pushButton_missionPlanning_goto_back, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionPlanning_goto_back);
    connect(
        ui->pushButton_missionPlanning_goto_clean, &QPushButton::clicked,
        ui->map, &Map::updateUi_missionPlanning_goto_goal_clear);
    connect(
        ui->pushButton_missionPlanning_goto_on_trajectory, &QPushButton::clicked,
        ui->map, &Map::updateUi_missionPlanning_goto_traj_onoff);
    connect(
        ui->pushButton_missionPlanning_goto_on_trajectory_clear, &QPushButton::clicked,
        ui->map, &Map::updateUi_missionPlanning_goto_traj_clear);

    connect(
        ui->pushButton_missionPlanning_go_trajectory_update, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionPlanning_go_trajectory_update);
    connect(
        this, &MainWindow::signal_pushButton_missionPlanning_go_trajectory_updateMap,
        ui->map, &Map::updateUi_missionPlanning_goto_goal);
    connect(
        ui->pushButton_missionPlanning_go_trajectory_back, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionPlanning_go_trajectory_back);
    connect(
        ui->pushButton_missionPlanning_go_trajectory_clean, &QPushButton::clicked,
        ui->map, &Map::updateUi_missionPlanning_goto_goal_clear);


    connect(
        ui->pushButton_missionPlanning_following, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionPlanning_following);
    connect(
        ui->pushButton_missionPlanning_go_trajectory, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionPlanning_go_trajectory);

        setModeAutomatic_mission_cpp();

}

//cpp start

void MainWindow::setModeAutomatic_mission_cpp()
{
    ui->tableWidget_missionPlanning_cpp_zonaResearch->setColumnCount(2);
    ui->tableWidget_missionPlanning_cpp_zonaResearch->setHorizontalHeaderLabels(QStringList() << "X" << "Y");
    ui->pushButton_missionPlanning_cpp_on_off->setCheckable(true);


    connect(ui->map, &Map::pointAdded, this, &MainWindow::addPointToTable);
    connect(ui->pushButton_missionPlanning_cpp_make, &QPushButton::clicked, this, &MainWindow::slot_pushButton_missionPlanning_cpp_make);
    connect(ui->pushButton_missionPlanning_cpp_make_clean, &QPushButton::clicked, this, &MainWindow::slot_pushButton_missionPlanning_cpp_make_clean);
    connect(this, &MainWindow::requestUpdateChart, ui->map, &Map::updateChart);
    connect(this, &MainWindow::requestClearLines, ui->map, &Map::clearLines);
    connect(this, &MainWindow::plotLineSeries, ui->map, &Map::onPlotLineSeries);
    // Подключение кнопки для переключения состояния
    connect(ui->pushButton_missionPlanning_cpp_on_off, &QPushButton::toggled,
                ui->map, &Map::setMissionPlanning_cpp_Enabled);

    connect(
        ui->pushButton_missionPlanning_cpp_back, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionPlanning_cpp_back);

    connect(
        ui->pushButton_missionPlanning_cpp, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_missionPlanning_goto_back);

    connect(
        ui->pushButton_missionPlanning_cpp, &QPushButton::clicked,
                this, &MainWindow::slot_pushButton_missionPlanning_cpp);
}

void MainWindow::setWidget()
{
    powerSystem = new PowerSystem(this);
    ui->horizontalLayout_for_powerSystem->addWidget(powerSystem);
    checkMsg = new CheckMsg(this);
    ui->horizontalLayout_for_checkMsg->addWidget(checkMsg);
}

void MainWindow::addPointToTable(qreal x, qreal y) {
    int row = ui->tableWidget_missionPlanning_cpp_zonaResearch->rowCount();
    ui->tableWidget_missionPlanning_cpp_zonaResearch->insertRow(row);
    ui->tableWidget_missionPlanning_cpp_zonaResearch->setItem(row, 0, new QTableWidgetItem(QString::number(x)));
    ui->tableWidget_missionPlanning_cpp_zonaResearch->setItem(row, 1, new QTableWidgetItem(QString::number(y)));
}

void MainWindow::slot_pushButton_missionPlanning_cpp_make() {
    QLineSeries *lineSeries = new QLineSeries();
    int rowCount = ui->tableWidget_missionPlanning_cpp_zonaResearch->rowCount();

    QStringList pointList;

    for (int i = 0; i < rowCount; ++i) {
        qreal x = ui->tableWidget_missionPlanning_cpp_zonaResearch->item(i, 0)->text().toDouble();
        qreal y = ui->tableWidget_missionPlanning_cpp_zonaResearch->item(i, 1)->text().toDouble();
        lineSeries->append(x, y);

        pointList.append(QString::number(x) + "," + QString::number(y));
    }

    if (rowCount > 0) {
        // Замкнуть фигуру, соединяя последнюю точку с первой
        qreal firstX = ui->tableWidget_missionPlanning_cpp_zonaResearch->item(0, 0)->text().toDouble();
        qreal firstY = ui->tableWidget_missionPlanning_cpp_zonaResearch->item(0, 1)->text().toDouble();
        lineSeries->append(firstX, firstY);
    }
    emit requestUpdateChart(lineSeries);

    QString pointsString = pointList.join(" ");

    // Получение значения из doubleSpinBox
    double distanceTack = ui->doubleSpinBox_missionPlanning_cpp_distanceTack->value();
    QString distanceTackStr = QString::number(distanceTack);

    qDebug() << "Python start:";
    qDebug() << "Generated points string:" << pointsString;
    qDebug() << "Distance Tack:" << distanceTackStr;



    // Создание и настройка QProcess
    QProcess process;
    QString program = "python3";
    QStringList arguments;
    arguments << "execute_algorithm.py" << pointsString << distanceTackStr;

    process.setProgram(program);
    process.setArguments(arguments);
    process.setWorkingDirectory("/home/shakuevda/Desktop/pult/UMAS_GUI"); // Укажите путь к директории скрипта

    process.start();

    // Проверка запуска процесса
    if (!process.waitForStarted()) {
        qDebug() << "Failed to start process:" << process.errorString();
        return;
    }

    // Ожидание завершения процесса
    process.waitForFinished();

    // Чтение стандартного вывода и ошибок
    QString output(process.readAllStandardOutput());
    QString error(process.readAllStandardError());

    if (!error.isEmpty()) {
        qDebug() << "Python error:" << error;
    } else {
        qDebug() << "Python output:" << output;

        // Парсинг результата и создание QLineSeries
        QLineSeries *resultSeries = new QLineSeries();
        output.remove('[').remove(']').replace("(", "").replace(")", "").replace(" ", "");
        QStringList pointStrings = output.split(',');

        for (int i = 0; i < pointStrings.size(); i += 2) {
            qreal x = pointStrings[i].toDouble();
            qreal y = pointStrings[i + 1].toDouble();
            resultSeries->append(x, y);
        }

        emit plotLineSeries(resultSeries); // Отправка сигнала с QLineSeries
    }
    qDebug() << "Python finish";

}

void MainWindow::slot_pushButton_missionPlanning_cpp_make_clean() {
    ui->tableWidget_missionPlanning_cpp_zonaResearch->clearContents();
    ui->tableWidget_missionPlanning_cpp_zonaResearch->setRowCount(0);
    emit requestClearLines();
}

void MainWindow::slot_pushButton_missionPlanning_cpp_back()
{
    ui->stackedWidget_missionPlanning->setCurrentIndex(0);
}

void MainWindow::slot_pushButton_missionPlanning_cpp()
{
    ui->stackedWidget_missionPlanning->setCurrentIndex(3);
    displayText("Задайте параметры для покрытия области");
}

//cpp finish

void MainWindow::test_automatic_after()
{
    ui->stackedWidget_mode->setCurrentIndex(0);
    ui->stackedWidget_missionPlanning->setCurrentIndex(0);
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
    uv_interface.setID_mission_AUV(0);
}

void MainWindow::slot_pushButton_missionPlanning_goto()
{
    uv_interface.setID_mission_AUV(1);
    ui->stackedWidget_missionPlanning->setCurrentIndex(2);
    displayText("Задайте параметры для выхода в точку");
}

void MainWindow::slot_pushButton_missionPlanning_goto_update()
{
    double x = ui->doubleSpinBox_missionPlanning_goto_x->value();
    double y = ui->doubleSpinBox_missionPlanning_goto_y->value();
    double r = ui->doubleSpinBox_missionPlanning_goto_r->value();

    emit signal_pushButton_missionPlanning_goto_updateMap(x,y,r,0);
    displayText("Установлена координата для выхода в точку и"
                " радиус удержания позиции");
}

void MainWindow::slot_pushButton_missionPlanning_goto_back()
{
    ui->stackedWidget_missionPlanning->setCurrentIndex(0);
}

void MainWindow::slot_pushButton_missionPlanning_go_trajectory_update()
{
    double x1 = ui->doubleSpinBox_missionPlanning_go_trajectory_x_1->value();
    double y1 = ui->doubleSpinBox_missionPlanning_go_trajectory_y_1->value();
    double r1 = ui->doubleSpinBox_missionPlanning_go_trajectory_r_1->value();

    double x2 = ui->doubleSpinBox_missionPlanning_go_trajectory_x_2->value();
    double y2 = ui->doubleSpinBox_missionPlanning_go_trajectory_y_2->value();
    double r2 = ui->doubleSpinBox_missionPlanning_go_trajectory_r_2->value();

    double x3 = ui->doubleSpinBox_missionPlanning_go_trajectory_x_3->value();
    double y3 = ui->doubleSpinBox_missionPlanning_go_trajectory_y_3->value();
    double r3 = ui->doubleSpinBox_missionPlanning_go_trajectory_r_3->value();

    emit signal_pushButton_missionPlanning_go_trajectory_updateMap(x1,y1,r1,0);
    emit signal_pushButton_missionPlanning_go_trajectory_updateMap(x2,y2,r2,0);
    emit signal_pushButton_missionPlanning_go_trajectory_updateMap(x3,y3,r3,0);
    displayText("Установлены координаты для движения по траектории и"
                " радиус удержания позиций");
}

void MainWindow::slot_pushButton_missionPlanning_go_trajectory_back()
{
    ui->stackedWidget_missionPlanning->setCurrentIndex(0);
}

void MainWindow::slot_pushButton_missionPlanning_following()
{
    uv_interface.setID_mission_AUV(2);
}

void MainWindow::slot_pushButton_missionPlanning_go_trajectory()
{
    uv_interface.setID_mission_AUV(3);
    ui->stackedWidget_missionPlanning->setCurrentIndex(1);
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
    communicationAgent1 = new Pult::PC_Protocol(QHostAddress("192.168.1.10"), 13053,
                                                QHostAddress("192.168.1.11"), 13052, 10, 0);
    communicationAgent2 = new Pult::PC_Protocol(QHostAddress("192.168.1.102"), 13053,
                                                QHostAddress("192.168.1.3"), 13052, 10, 1);

//    communicationAgent1 = new Pult::PC_Protocol(QHostAddress("192.168.1.10"), 13055,
//                                                QHostAddress("192.168.1.3"), 13054, 10, 0);
//    communicationAgent2 = new Pult::PC_Protocol(QHostAddress("192.168.1.10"), 13053,
//                                                QHostAddress("192.168.1.11"), 13052, 10, 1);


    communicationAgent1->startExchange();
    communicationAgent2->startExchange();

    if (communicationAgent1->bindState() || communicationAgent2->bindState()) {
        displayText("Соединение установлено");

        connect(communicationAgent1, SIGNAL(dataReceived()),
                this, SLOT(updateUi_fromAgent1()));
        connect(communicationAgent2, SIGNAL(dataReceived()),
                this, SLOT(updateUi_fromAgent2()));
        updateStatePushButton();

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
        uv_interface.setModeSelection(0); //агент
    }
    else{
        uv_interface.setModeSelection(1); //модель
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

void MainWindow::pushButton_selectAgent2(bool stateBottom)
{
    if (stateBottom){
        uv_interface.setCurrentAgent(1);
        displayText("Установлен ввод и вывод данных с агента 2");
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

    ControlContoursFlags state_controlContoursFlags = uv_interface.getControlContoursFlags();
    if (state_controlContoursFlags.yaw)
        ui->pushButton_modeAutomated_psi->setChecked(true);
    else
        ui->pushButton_modeAutomated_psi->setChecked(false);
    if (state_controlContoursFlags.roll)
        ui->pushButton_modeAutomated_gamma->setChecked(true);
    else
        ui->pushButton_modeAutomated_gamma->setChecked(false);
    if (state_controlContoursFlags.pitch)
        ui->pushButton_modeAutomated_tetta->setChecked(true);
    else
        ui->pushButton_modeAutomated_tetta->setChecked(false);
    if (state_controlContoursFlags.march)
        ui->pushButton_modeAutomated_march->setChecked(true);
    else
        ui->pushButton_modeAutomated_march->setChecked(false);
    if (state_controlContoursFlags.lag)
        ui->pushButton_modeAutomated_lag->setChecked(true);
    else
        ui->pushButton_modeAutomated_lag->setChecked(false);
    if (state_controlContoursFlags.depth)
        ui->pushButton_modeAutomated_depth->setChecked(true);
    else
        ui->pushButton_modeAutomated_depth->setChecked(false);




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
            checkMsg, SLOT(updateUi_checkMsg()));
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

MainWindow::~MainWindow()
{
    delete ui;
}
