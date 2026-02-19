#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    rosBridge = new RosBridge(this);
    rosBridge->start();

    setWidget();
    setInterface();
    setGUI_reper();
    setConsole();
    setTimer_updateImpact(20);
    setBottom();
    setTab();
    setUpdateUI();

    // Инициализация спинбоксов
    gainSpinBoxes = {
        ui->spinBox_gain_surge,
        ui->spinBox_gain_sway,
        ui->spinBox_gain_depth,
        ui->spinBox_gain_yaw,
        ui->spinBox_gain_pitch,
        ui->spinBox_gain_roll
    };

    loadSettings();
    setSpinBoxValuesForCurrentMode();

    // Подключаем сигналы спинбоксов напрямую к saveCurrentModeGains через лямбду
    for (auto spinBox : gainSpinBoxes) {
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                this, [this]() { saveCurrentModeGains(); });
    }

    // Инициализируем текущий режим (по умолчанию Medium)
    setSpeedMode(SpeedMode::Medium);

    connect(ui->gamepad_btn, &QRadioButton::clicked,
        this, &MainWindow::useGamepad);


    QButtonGroup *inputGroup = new QButtonGroup(this);
inputGroup->addButton(ui->radioButton_useJoyStick);
inputGroup->addButton(ui->radioButton_useKeyBoard);
inputGroup->addButton(ui->gamepad_btn);
inputGroup->setExclusive(true);

// Устанавливаем геймпад по умолчанию
ui->gamepad_btn->setChecked(true);
useGamepad(); // активируем режим геймпада
}

void MainWindow::setWidget()
{
    powerSystem = new PowerSystem(this);
    ui->horizontalLayout_for_powerSystem->addWidget(powerSystem);
    checkMsg = new CheckMsg(this);
    ui->horizontalLayout_for_checkMsg->addWidget(checkMsg);
    modeAutomatic = new ModeAutomatic(this);
    ui->verticalLayout_modeAutomatic->addWidget(modeAutomatic);
    diagnostic_board = new Diagnostic_board(this);
    ui->horizontalLayout_diagnosticBoard->addWidget(diagnostic_board);

    connect(
        modeAutomatic, &ModeAutomatic::displayText_toConsole,
        this, &MainWindow::displayText);
    connect(
        modeAutomatic, &ModeAutomatic::set_stackedWidget_mode,
        ui->stackedWidget_mode, &QStackedWidget::setCurrentIndex);
}

void MainWindow::setGUI_reper()
{
    connect(
        ui->pushButton_sendReper, &QPushButton::clicked,
        this, &MainWindow::slot_pushButton_sendReper);
}

void MainWindow::slot_pushButton_sendReper()
{
    qDebug() << "hello";
    QString latitudeStr = ui->lineEdit_reper_latitude->text();
    QString longitudeStr = ui->lineEdit_reper_longitude->text();

    double latitude = latitudeStr.toDouble();
    double longitude = longitudeStr.toDouble();

    CoordinatePoint msg;
    msg.x_point = latitude;
    msg.y_point = longitude;
    uv_interface.setReper(msg);
}

void MainWindow::slot_addMarker_to_gui(double latitude, double longitude)
{
    ui->lineEdit_reper_latitude->setText(QString::number(latitude, 'd', 14));
    ui->lineEdit_reper_longitude->setText(QString::number(longitude, 'd', 14));

    CoordinatePoint msg;
    msg.x_point = latitude;
    msg.y_point = longitude;
    uv_interface.setReper(msg);
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
        this, SLOT(updateUi_fromControl()));
    updateTimer->start(periodUpdateMsec);
    displayText("Таймер обновления джойстика запущен");
}

void MainWindow::useKeyBoard()
{

    if (gamepad) {
        delete gamepad;
        gamepad = nullptr;
    }
    // Если использовался джойстик – тоже удаляем (на всякий случай)
    if (joyStick) {
        delete joyStick;
        joyStick = nullptr;
    }
    // Если клавиатура ещё не создана – создаём
    if (!keyBoard) {
        keyBoard = new KeyBoard(this);
    }
       ui->radioButton_useKeyBoard->setChecked(true);
    m_gamepadActive = false;
    status_keyboard = true;
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
    m_gamepadActive = false;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (keyBoard)
        keyBoard->keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (keyBoard)
        keyBoard->keyReleaseEvent(event);
}

void MainWindow::updateUi_fromControl()
{
    ControlData control = uv_interface.getControlData();
    DataAH127C imuData = uv_interface.getImuData();

    if (m_gamepadActive) {
        control.march = m_gamepad_right_marsh;
        control.depth = m_gamepadDepth;
        control.yaw   = m_gamepad_right_course;  
        control.pitch = m_gamepadV;
        control.roll  = m_gamepadCren;
    }
    double desiredYawForCompass = control.yaw;
    if (m_gamepadActive) {

        desiredYawForCompass = control.yaw * 10;
    }



ui->label_controlMarch->setFixedWidth(38); 
ui->label_controlYaw->setFixedWidth(38); 
ui->label_controlDepth->setFixedWidth(38); 
ui->label_controlKren->setFixedWidth(30); 
ui->label_controlDif->setFixedWidth(30); 



    ui->compass->setYawDesirable(desiredYawForCompass, imuData.yaw, uv_interface.getCSMode());
    
    ui->label_controlYaw->setNum(control.yaw * ui->spinBox_gain_yaw->value());
    ui->label_controlMarch->setNum(control.march * ui->spinBox_gain_surge->value());
    ui->label_controlDif->setNum(control.pitch * ui->spinBox_gain_pitch->value());
    ui->label_controlLag->setNum(control.lag * ui->spinBox_gain_sway->value());
    ui->label_controlDepth->setNum(control.depth * ui->spinBox_gain_depth->value());
    ui->label_controlKren->setNum(control.roll * ui->spinBox_gain_roll->value());


    rosBridge->publishCommand(control.march * ui->spinBox_gain_surge->value(),
                              control.lag * ui->spinBox_gain_sway->value(),
                              control.depth * ui->spinBox_gain_depth->value(),
                              control.roll * ui->spinBox_gain_roll->value(),
                              control.pitch * ui->spinBox_gain_pitch->value(),
                              control.yaw * ui->spinBox_gain_yaw->value());
}

void MainWindow::updateUi_Map()
{
    GPS_coordinate gps_coordinate = uv_interface.getCoordinateGPS();
    emit signal_sendCurrentPos(gps_coordinate.latitude, gps_coordinate.longitude);
}

void MainWindow::setBottom()
{
    setBottom_mode();
    setBottom_connection();
    setBottom_modeSelection();
    setBottom_selectAgent();

    connect(ui->pushButton_zeroYaw, &QPushButton::clicked,
            rosBridge, &RosBridge::zeroYaw);

    // Подключение кнопок скорости к единому слоту
    // Предполагается, что в UI кнопки переименованы в pushButton_speedFast и т.д.
    if (ui->pushButton_speedFast) {
        connect(ui->pushButton_speedFast, &QPushButton::clicked,
                this, [this]() { setSpeedMode(SpeedMode::Fast); });
    }

    if (ui->pushButton_speedMedium) {
        connect(ui->pushButton_speedMedium, &QPushButton::clicked,
                this, [this]() { setSpeedMode(SpeedMode::Medium); });
    }

    if (ui->pushButton_speedSlow) {
        connect(ui->pushButton_speedSlow, &QPushButton::clicked,
                this, [this]() { setSpeedMode(SpeedMode::Slow); });
    }
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

    ui->pushButton_modeAutomated_surge->setCheckable(true);
    ui->pushButton_modeAutomated_sway->setCheckable(true);
    ui->pushButton_modeAutomated_depth->setCheckable(true);
    ui->pushButton_modeAutomated_yaw->setCheckable(true);
    ui->pushButton_modeAutomated_pitch->setCheckable(true);
    ui->pushButton_modeAutomated_roll->setCheckable(true);
    QButtonGroup *modeAutomated = new QButtonGroup(this);
    modeAutomated->addButton(ui->pushButton_modeAutomated_surge);
    modeAutomated->addButton(ui->pushButton_modeAutomated_sway);
    modeAutomated->addButton(ui->pushButton_modeAutomated_depth);
    modeAutomated->addButton(ui->pushButton_modeAutomated_yaw);
    modeAutomated->addButton(ui->pushButton_modeAutomated_pitch);
    modeAutomated->addButton(ui->pushButton_modeAutomated_roll);
    modeAutomated->setExclusive(false);

    connect(ui->pushButton_modeAutomated_surge, &QPushButton::toggled,
            rosBridge, &RosBridge::setModeSurge);
    connect(ui->pushButton_modeAutomated_sway, &QPushButton::toggled,
            rosBridge, &RosBridge::setModeSway);
    connect(ui->pushButton_modeAutomated_depth, &QPushButton::toggled,
            rosBridge, &RosBridge::setModeHeave);
    connect(ui->pushButton_modeAutomated_yaw, &QPushButton::toggled,
            rosBridge, &RosBridge::setModeYaw);
    connect(ui->pushButton_modeAutomated_pitch, &QPushButton::toggled,
            rosBridge, &RosBridge::setModePitch);
    connect(ui->pushButton_modeAutomated_roll, &QPushButton::toggled,
            rosBridge, &RosBridge::setModeRoll);

    e_CSModeManualToggled();

    connect(ui->pushButton_modeManual, SIGNAL(clicked()),
            this, SLOT(e_CSModeManualToggled()));
    connect(ui->pushButton_modeAutomated, SIGNAL(clicked()),
            this, SLOT(e_CSModeAutomatedToggled()));
    connect(ui->pushButton_modeAutomatic, SIGNAL(clicked()),
            this, SLOT(e_CSModeAutomaticToggled()));
}

void MainWindow::e_CSModeManualToggled()
{
    uv_interface.setCSMode(e_CSMode::MODE_MANUAL);
}

void MainWindow::e_CSModeAutomatedToggled()
{
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

    connect(ui->pushButton_connection, SIGNAL(clicked()),
            this, SLOT(setConnection()));
    connect(ui->pushButton_breakConnection, SIGNAL(clicked()),
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

void MainWindow::updateUi_fromAgent1()
{
    DataAH127C imuData = uv_interface.getImuData();

    emit updateCompass(imuData.yaw);
    emit updateIMU(imuData);
    emit updateSetupMsg();
    emit updateDataMission();
    emit updateMap();
}

void MainWindow::breakConnection()
{
    ui->pushButton_connection->setEnabled(true);
    displayText("Соединение разорвано");
    communicationAgent1->stopExhange();
    delete communicationAgent1;
    ui->pushButton_breakConnection->setEnabled(false);
}

void MainWindow::setBottom_modeSelection()
{
    setModeSelection(0);
    connect(ui->comboBox_modeSelection, SIGNAL(activated(int)),
            this, SLOT(setModeSelection(int)));
}

void MainWindow::setModeSelection(int index)
{
    if (index == 1) {
        uv_interface.setModeSelection(0); // агент
    } else {
        uv_interface.setModeSelection(1); // модель
    }
}

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
    if (stateBottom) {
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
    ui->tabWidget->setTabText(2, "Контроль сообщений");
    ui->tabWidget->setTabText(3, "Режимы питания");
    ui->tabWidget->setTabText(4, "Карта ГИC");
    ui->tabWidget->setCurrentIndex(4);
}

void MainWindow::setUpdateUI()
{
    connect(this, SIGNAL(updateCompass(float)),
            this, SLOT(updateUi_Compass(float)));
    connect(this, SIGNAL(updateSetupMsg()),
            checkMsg, SLOT(updateUi_checkMsg()));
    connect(this, SIGNAL(updateDataMission()),
            modeAutomatic, SLOT(updateUi_DataMission()));
    connect(this, &MainWindow::updateMap,
            this, &MainWindow::updateUi_Map);
}

void MainWindow::loadSettings()
{
    QSettings settings("/UMAS_GUI/umas_settings.ini", QSettings::IniFormat);

    for (int mode = 0; mode < 3; ++mode) {
        settings.beginGroup(QString("SpeedMode_%1").arg(mode));

        QMap<QString, double> gains;
        for (int i = 0; i < gainNames.size(); ++i) {
            double defaultValue = 0;
            if (mode == static_cast<int>(SpeedMode::Slow)) defaultValue = 10;
            else if (mode == static_cast<int>(SpeedMode::Medium)) defaultValue = 50;
            else if (mode == static_cast<int>(SpeedMode::Fast)) defaultValue = 100;

            gains[gainNames[i]] = settings.value(gainNames[i], defaultValue).toDouble();
        }

        speedModeGains[mode] = gains;
        settings.endGroup();
    }

    int savedMode = settings.value("CurrentSpeedMode", static_cast<int>(SpeedMode::Medium)).toInt();
    currentMode = static_cast<SpeedMode>(savedMode);
}

void MainWindow::saveSettings()
{
    QSettings settings("/UMAS_GUI/umas_settings.ini", QSettings::IniFormat);

    for (int mode = 0; mode < 3; ++mode) {
        settings.beginGroup(QString("SpeedMode_%1").arg(mode));

        for (int i = 0; i < gainNames.size(); ++i) {
            settings.setValue(gainNames[i], speedModeGains[mode][gainNames[i]]);
        }

        settings.endGroup();
    }

    settings.setValue("CurrentSpeedMode", static_cast<int>(currentMode));
}

void MainWindow::saveCurrentModeGains()
{
    for (int i = 0; i < gainSpinBoxes.size(); ++i) {
        speedModeGains[static_cast<int>(currentMode)][gainNames[i]] = gainSpinBoxes[i]->value();
    }
    saveSettings();
}

void MainWindow::setSpinBoxValuesForCurrentMode()
{
    for (auto spinBox : gainSpinBoxes) {
        spinBox->blockSignals(true);
    }

    for (int i = 0; i < gainSpinBoxes.size(); ++i) {
        gainSpinBoxes[i]->setValue(speedModeGains[static_cast<int>(currentMode)][gainNames[i]]);
    }

    for (auto spinBox : gainSpinBoxes) {
        spinBox->blockSignals(false);
    }
}

// Единый слот для переключения режима скорости
void MainWindow::setSpeedMode(SpeedMode mode)
{
    saveCurrentModeGains();
    currentMode = mode;
    setSpinBoxValuesForCurrentMode();

    // Обновляем стили кнопок (используем новые имена)
    ui->pushButton_speedFast->setStyleSheet(
        mode == SpeedMode::Fast ? "background-color: purple; font-size: 25px" :
                                   "background-color: white; font-size: 25px");
    ui->pushButton_speedMedium->setStyleSheet(
        mode == SpeedMode::Medium ? "background-color: purple; font-size: 25px" :
                                     "background-color: white; font-size: 25px");
    ui->pushButton_speedSlow->setStyleSheet(
        mode == SpeedMode::Slow ? "background-color: purple; font-size: 25px" :
                                   "background-color: white; font-size: 25px");

    saveSettings();
}


void MainWindow::useGamepad()
{
    if (joyStick) { delete joyStick; joyStick = nullptr; }
    if (keyBoard) { delete keyBoard; keyBoard = nullptr; }
    if (gamepad) { delete gamepad; gamepad = nullptr; }

    gamepad = new Gamepad(0, this);
    if (!gamepad->isConnected()) {
        displayText("Геймпад не обнаружен! Проверьте подключение.");
        delete gamepad;
        gamepad = nullptr;
        return;
    }


    connect(gamepad, &Gamepad::backButtonPressed,
            this, &MainWindow::useKeyBoard);

    displayText("Геймпад подключен. Режим управления с геймпада активирован.");
m_gamepadActive = true;
    // Сброс всех значений при старте
    m_gamepadMarch = 0;
    m_gamepadDepth = 0;
    m_gamepadYaw = 0;
    m_gamepadPitch = 0;
    m_gamepadCren = 0;
    m_gamepadU = 0;
    m_gamepadV = 0;
    m_gamepad_right_marsh = 0;
    m_gamepad_right_course = 0;

    // правый джойстик
     connect(gamepad, &Gamepad::rightStickXMoved, this, [this](float value) {
        m_gamepadU = std::round(value)  / 100.0f;   // преобразуем -100..100 в -1..1
        qDebug() << m_gamepadU << "Ось U";

    });
    connect(gamepad, &Gamepad::rightStickYMoved, this, [this](float value) {
        m_gamepadV = std::round(value) / 100.0f;
        qDebug() << m_gamepadV << "Ось V";
    });

     // левый джойстик

    connect(gamepad, &Gamepad::leftStickXMoved, this, [this](float value){
  
        m_gamepad_right_course = std::round(value) / 100.0f;
         qDebug() << "leftStickX value:" << value << "-> m_gamepad_right_course:" << m_gamepad_right_course;
    });

        connect(gamepad, &Gamepad::leftStickYMoved, this, [this](float value){
        m_gamepad_right_marsh = (-1) * std::round(value) / 100.0f;
    });



// --- Ккрен X / B ---
connect(gamepad, &Gamepad::buttonXPressed, this, [this]() { m_gamepadCren = -10; });
connect(gamepad, &Gamepad::buttonXReleased, this, [this]() { m_gamepadCren = 0; });
connect(gamepad, &Gamepad::buttonBPressed, this, [this]() { m_gamepadCren = 10; });
connect(gamepad, &Gamepad::buttonBReleased, this, [this]() { m_gamepadCren = 0; });

// --- Глубина (вниз/вверх): L1 / R1 ---
connect(gamepad, &Gamepad::L1Pressed, this, [this]() { m_gamepadDepth = 10; });
connect(gamepad, &Gamepad::L1Released, this, [this]() { m_gamepadDepth = 0; });
connect(gamepad, &Gamepad::R1Pressed, this, [this]() { m_gamepadDepth = -10; });
connect(gamepad, &Gamepad::R1Released, this, [this]() { m_gamepadDepth = 0; });

// --- Переключение режимов скорости (влево/вправо): крестовина ---
connect(gamepad, &Gamepad::dPadRightPressed, this, [this]() {
    // Переход к предыдущему режиму
    int current = static_cast<int>(currentMode);
    int previous = (current - 1 + 3) % 3;  
    setSpeedMode(static_cast<SpeedMode>(previous));
    
});
connect(gamepad, &Gamepad::dPadLeftPressed, this, [this]() {
    int current = static_cast<int>(currentMode);
    int next = (current + 1) % 3;
    setSpeedMode(static_cast<SpeedMode>(next));
    
});


}
void MainWindow::updateUi_Compass(float yaw)
{
    ui->compass->setYaw(yaw);
}

MainWindow::~MainWindow()
{
    delete ui;
}