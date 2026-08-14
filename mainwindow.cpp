#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Центральная модель состояния ПА (телеметрия/состояние для UI)
    uvState = new UVState(this);

    // Инициализация ROS Thread
    rosBridge = new RosBridge(this);
    rosBridge->start();

    connect(this, &MainWindow::publishTwistRequested,
            rosBridge, &RosBridge::publishTwistInternal,
            Qt::QueuedConnection);

    // ROS -> UVState (QueuedConnection: RosBridge живёт в другом потоке)
    connect(rosBridge, &RosBridge::poseReceived,
            uvState, static_cast<void (UVState::*)(const UVState::Pose&)>(&UVState::setPose),
            Qt::QueuedConnection);

    connect(rosBridge, &RosBridge::controlFlagsPublished,
            uvState, &UVState::setControlFlags,
            Qt::QueuedConnection);

    setWidget();
    setConsole();
    setTimer_updateImpact(10);
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

    


    QButtonGroup *inputGroup = new QButtonGroup(this);
    inputGroup->addButton(ui->radioButton_useJoyStick);
    inputGroup->addButton(ui->radioButton_useKeyBoard);
    inputGroup->addButton(ui->gamepad_btn);
    inputGroup->setExclusive(true);
}

void MainWindow::setWidget()
{
    // powerSystem = new PowerSystem(this);
    // ui->horizontalLayout_for_powerSystem->addWidget(powerSystem);
    // checkMsg = new CheckMsg(this);
    // ui->horizontalLayout_for_checkMsg->addWidget(checkMsg);
    // modeAutomatic = new ModeAutomatic(this);
    // ui->verticalLayout_modeAutomatic->addWidget(modeAutomatic);
    diagnostic_board = new Diagnostic_board(this);
    ui->horizontalLayout_diagnosticBoard->addWidget(diagnostic_board);

    // connect(
    //     modeAutomatic,&ModeAutomatic::displayText_toConsole,
    //     this, &MainWindow::displayText);
    // connect(
    //     modeAutomatic, &ModeAutomatic::set_stackedWidget_mode,
    //     ui->stackedWidget_mode, &QStackedWidget::setCurrentIndex);
}


void MainWindow::setConsole()
{
    displayText("Приложение работает");
    displayText("Установите соединение для работы с агентом");
}

void MainWindow::displayText(QString str)
{
    QString currentTime = QTime::currentTime().toString("HH:mm:ss");
    qInfo() << currentTime << str;
    ui->textEdit_console->append(currentTime + " " + str);
}

void MainWindow::setTimer_updateImpact(int periodUpdateMsec)
{
    joyStick = std::make_unique<JoyStick>();
    keyBoard = std::make_unique<KeyBoard>();
    activeInput = joyStick.get();

    connect(ui->radioButton_useJoyStick, &QRadioButton::clicked,
            this, &MainWindow::useJoyStick);
    connect(ui->radioButton_useKeyBoard, &QRadioButton::clicked,
            this, &MainWindow::useKeyBoard);
    connect(ui->gamepad_btn, &QRadioButton::clicked,
            this, &MainWindow::useGamepad);

    updateTimer = new QTimer(this);
    connect(
        updateTimer, SIGNAL(timeout()),
        this, SLOT(updateUi_fromControl()));
    updateTimer->start(periodUpdateMsec);
    displayText("Таймер обновления джойстика запущен");
}

void MainWindow::useKeyBoard()
{
    activeInput = nullptr;
    gamepadInput.reset();
    if (gamepad) { delete gamepad; gamepad = nullptr; }

    if (!keyBoard) {
        keyBoard = std::make_unique<KeyBoard>();
    }

    ui->radioButton_useKeyBoard->setChecked(true);
    status_keyboard = true;
    activeInput = keyBoard.get();
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
    activeInput = nullptr;
    gamepadInput.reset();
    if (gamepad) { delete gamepad; gamepad = nullptr; }

    if (!joyStick) {
        joyStick = std::make_unique<JoyStick>();
    }

    if (!joyStick->isAvailable()) {
        displayText("Джойстик не обнаружен. Переключение на клавиатуру.");
        useKeyBoard();
        return;
    }

    activeInput = joyStick.get();
}

void MainWindow::useGamepad()
{
    Gamepad* newGamepad = new Gamepad(0, this);
    if (!newGamepad->isConnected()) {
        displayText("Геймпад не обнаружен! Проверьте подключение.");
        delete newGamepad;
        newGamepad = nullptr;
        useKeyBoard();
        return;
    }

    // Отсоединяемся от других источников ввода только после успешного подключения
    activeInput = nullptr;
    joyStick.reset();
    keyBoard.reset();
    gamepadInput.reset();
    if (gamepad) { delete gamepad; gamepad = nullptr; }
    gamepad = newGamepad;


    connect(gamepad, &Gamepad::backButtonPressed,
            this, &MainWindow::useKeyBoard);

    displayText("Геймпад подключен. Режим управления с геймпада активирован.");
    gamepadInput = std::make_unique<GamepadInputSource>(gamepad, this);
    activeInput = gamepadInput.get();

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


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (activeInput == keyBoard.get() && keyBoard) {
        keyBoard->keyPressEvent(event);
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (activeInput == keyBoard.get() && keyBoard) {
        keyBoard->keyReleaseEvent(event);
    }
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::updateUi_fromControl(){
    if (activeInput != nullptr) {
        const auto command = activeInput->poll();
        if (command.has_value()) {
            controlService.apply(command.value());
        }
    }

    const auto scaled = applyGains(controlService.snapshot());
    updateControlLabels(scaled);

    emit publishTwistRequested(
        scaled.march,
        scaled.lag,
        scaled.depth,
        scaled.roll,
        scaled.pitch,
        scaled.yaw);
}

umas::input::ControlCommand MainWindow::applyGains(const umas::input::ControlCommand& raw) const
{
    umas::input::ControlCommand scaled = raw;
    scaled.yaw *= ui->spinBox_gain_yaw->value();
    scaled.march *= ui->spinBox_gain_surge->value();
    scaled.pitch *= ui->spinBox_gain_pitch->value();
    scaled.lag *= ui->spinBox_gain_sway->value();
    scaled.depth *= ui->spinBox_gain_depth->value();
    scaled.roll *= ui->spinBox_gain_roll->value();
    return scaled;
}

void MainWindow::updateControlLabels(const umas::input::ControlCommand& scaled)
{
    ui->label_controlYaw->setNum(scaled.yaw);
    ui->label_controlMarch->setNum(scaled.march);
    ui->label_controlDif->setNum(scaled.pitch);
    ui->label_controlLag->setNum(scaled.lag);
    ui->label_controlDepth->setNum(scaled.depth);
    ui->label_controlKren->setNum(scaled.roll);
}

void MainWindow::setBottom()
{
    setBottom_mode();

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
    connect(ui->pushButton_zeroYaw, &QPushButton::clicked,
            rosBridge, &RosBridge::zeroYawInternal,
            Qt::QueuedConnection);

}

void MainWindow::setBottom_mode()
{
    ui->pushButton_modeManual->setCheckable(true);
    ui->pushButton_modeAutomated->setCheckable(true);
    ui->pushButton_modeAutomatic->setCheckable(false);
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
            this, [this](bool checked){ emit controlFlagRequested(0, checked); });

    connect(ui->pushButton_modeAutomated_sway, &QPushButton::toggled,
            this, [this](bool checked){ emit controlFlagRequested(1, checked); });

    connect(ui->pushButton_modeAutomated_depth, &QPushButton::toggled,
            this, [this](bool checked){ emit controlFlagRequested(2, checked); });

    connect(ui->pushButton_modeAutomated_yaw, &QPushButton::toggled,
            this, [this](bool checked){ emit controlFlagRequested(3, checked); });

    connect(ui->pushButton_modeAutomated_pitch, &QPushButton::toggled,
            this, [this](bool checked){ emit controlFlagRequested(4, checked); });

    connect(ui->pushButton_modeAutomated_roll, &QPushButton::toggled,
            this, [this](bool checked){ emit controlFlagRequested(5, checked); });

    connect(this, &MainWindow::controlFlagRequested,
            rosBridge, &RosBridge::setControlFlagInternal,
            Qt::QueuedConnection);
}


void MainWindow::setTab()
{
    ui->tabWidget->setTabText(0, "Камера");
    ui->tabWidget->setTabText(1, "БСО");
    ui->tabWidget->setTabText(2,  "Контроль сообщений");
    ui->tabWidget->setTabText(3,  "Режимы питания");
    ui->tabWidget->setCurrentIndex(4);
}

void MainWindow::setUpdateUI()
{
    connect(this, SIGNAL(updateCompass(float)),
            this, SLOT(updateUi_Compass(float)));
    // connect(this, SIGNAL(updateSetupMsg()),
    //         checkMsg, SLOT(updateUi_checkMsg()));
    // connect(this, SIGNAL(updateDataMission()),
    //         modeAutomatic, SLOT(updateUi_DataMission()));
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

void MainWindow::updateUi_Compass(float yaw)
{
    ui->compass->setYaw(yaw);
}

MainWindow::~MainWindow()
{
    delete ui;
}