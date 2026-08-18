#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    uvState = new UVState(this);

    rosBridge = new RosBridge(this);
    rosBridge->start();

    

    connect(this, &MainWindow::publishTwistRequested,
            rosBridge, &RosBridge::publishTwistInternal,
            Qt::QueuedConnection);

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
    setupButtonStyles();
    setTab();
    setUpdateUI();

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

    for (auto spinBox : gainSpinBoxes) {
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                this, [this]() { saveCurrentModeGains(); });
    }

    setSpeedMode(SpeedMode::Medium);

    QButtonGroup *inputGroup = new QButtonGroup(this);
    inputGroup->addButton(ui->radioButton_useJoyStick);
    inputGroup->addButton(ui->radioButton_useKeyBoard);
    inputGroup->addButton(ui->gamepad_btn);
    inputGroup->setExclusive(true);



    connect(ui->full_screen, &QCheckBox::toggled, this, [this](bool checked) {
        if (checked) {
            if (!fullscreenWindow_ && videoPlayer_) {
                //  полноэкранное окно
                fullscreenWindow_ = new FullscreenVideoWindow(uvState, this);

                //  плеер в полноэкранное окно
                videoPlayer_->setParent(fullscreenWindow_);
                fullscreenWindow_->setVideoWidget(videoPlayer_);
                videoPlayer_->show();

                //сигнал закрытия окна
                connect(fullscreenWindow_, &FullscreenVideoWindow::windowClosed, this, [this]() {
                    
                    ui->full_screen->blockSignals(true);
                    ui->full_screen->setChecked(false);
                    ui->full_screen->blockSignals(false);

                    //  плеер на место
                    if (videoPlayer_) {
                        videoPlayer_->setParent(ui->tab_video);
                        ui->tab_video->layout()->addWidget(videoPlayer_);
                        videoPlayer_->show();
                        //videoPlayer_->updateVideoWindow();
                    }

                    fullscreenWindow_ = nullptr;
                });

                fullscreenWindow_->showFullScreen();
            }
        } else {
            if (fullscreenWindow_) {
                fullscreenWindow_->close();
                fullscreenWindow_->deleteLater();
                fullscreenWindow_ = nullptr;
            }
        }
    });

    // Запуск видео-потока напрямую через GStreamer
    QTimer::singleShot(500, this, [this]() {
        if (videoPlayer_) {
            videoPlayer_->startStream("5000");
        }
    });
}

void MainWindow::setWidget()
{
    // powerSystem = new PowerSystem(this);
    // ui->horizontalLayout_for_powerSystem->addWidget(powerSystem);
    // checkMsg = new CheckMsg(this);
    // ui->horizontalLayout_for_checkMsg->addWidget(checkMsg);
    // modeAutomatic = new ModeAutomatic(this);
    // ui->verticalLayout_modeAutomatic->addWidget(modeAutomatic);
    // diagnostic_board = new Diagnostic_board(this);
    // ui->horizontalLayout_diagnosticBoard->addWidget(diagnostic_board);

    videoPlayer_ = new VideoPlayerWidget(this);

    if (!ui->tab_video->layout()) {
        ui->tab_video->setLayout(new QVBoxLayout(ui->tab_video));
    }

    ui->tab_video->layout()->addWidget(videoPlayer_);
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

    connect(gamepad, &Gamepad::dPadRightPressed, this, [this]() {
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

void MainWindow::updateUi_fromControl()
{
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

    QButtonGroup *speedGroup = new QButtonGroup(this);
    speedGroup->addButton(ui->pushButton_speedFast);
    speedGroup->addButton(ui->pushButton_speedMedium);
    speedGroup->addButton(ui->pushButton_speedSlow);
    speedGroup->setExclusive(true);

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
    ui->tabWidget->setTabText(2, "Контроль сообщений");
    ui->tabWidget->setTabText(3, "Режимы питания");
    ui->tabWidget->setTabText(4, "Исполнительные устройства");
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::setUpdateUI()
{
    connect(this, SIGNAL(updateCompass(float)),
            this, SLOT(updateUi_Compass(float)));
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

void MainWindow::setSpeedMode(SpeedMode mode)
{
    saveCurrentModeGains();
    currentMode = mode;
    setSpinBoxValuesForCurrentMode();

    // Тема сама покрасит :checked в зелёный #00ff88
    ui->pushButton_speedFast->setChecked(mode == SpeedMode::Fast);
    ui->pushButton_speedMedium->setChecked(mode == SpeedMode::Medium);
    ui->pushButton_speedSlow->setChecked(mode == SpeedMode::Slow);

    saveSettings();


    saveSettings();
}

void MainWindow::updateUi_Compass(float yaw)
{
    ui->compass->setYaw(yaw);
}

void MainWindow::resetTelemetryToDefault()
{
    isConnected = false;
    
    // Красный фон для индикации отсутствия связи
    QString noConnectionStyle = "background-color: #ff4444; color: #ffffff; font-weight: bold; border-radius: 6px; border: none; font-size: 15px; padding: 4px 10px;";
    
    ui->lbl_depth_value->setStyleSheet(noConnectionStyle);
    ui->lbl_depth_value->setText("N/A");
    
    ui->lbl_bottom_value->setStyleSheet(noConnectionStyle);
    ui->lbl_bottom_value->setText("N/A");
    
    ui->lbl_voltage_value->setStyleSheet(noConnectionStyle);
    ui->lbl_voltage_value->setText("N/A");
    
    ui->lbl_voltage2_value->setStyleSheet(noConnectionStyle);
    ui->lbl_voltage2_value->setText("N/A");
    
    ui->lbl_speed_value->setStyleSheet(noConnectionStyle);
    ui->lbl_speed_value->setText("N/A");
    
    ui->btn_killswitch_status->setStyleSheet(noConnectionStyle);
    ui->btn_killswitch_status->setText("N/A");
    
    ui->lbl_ping_value->setStyleSheet(noConnectionStyle);
    ui->lbl_ping_value->setText("N/A");
}

void MainWindow::updateTelemetryFromState()
{
    isConnected = true;
    
    // Зелёный фон для нормальных данных
    QString connectedStyle = "background-color: #00ff88; color: #0f1419; font-weight: bold; border-radius: 6px; border: none; font-size: 15px; padding: 4px 10px;";
    
    ui->lbl_depth_value->setStyleSheet(connectedStyle);
    ui->lbl_bottom_value->setStyleSheet(connectedStyle);
    ui->lbl_voltage_value->setStyleSheet(connectedStyle);
    ui->lbl_voltage2_value->setStyleSheet(connectedStyle);
    ui->lbl_speed_value->setStyleSheet(connectedStyle);
    ui->btn_killswitch_status->setStyleSheet(connectedStyle);
    ui->lbl_ping_value->setStyleSheet(connectedStyle);
    
    // Здесь обновляй реальные значения из uvState
    // ui->lbl_depth_value->setText(QString::number(uvState->getDepth(), 'f', 2) + " м");
    // и т.д.
}

void MainWindow::setupButtonStyles()
{
    // Кнопки скоростей: стиль на виджете — :checked работает надёжно
    const QString speedStyle =
        "QPushButton { background-color: #1e2a38; border: 1px solid #2d4052; border-radius: 4px;"
        " color: #e0e6ed; min-height: 28px; font-size: 13px; }"
        "QPushButton:hover { border: 1px solid #00ff88; color: #ffffff; }"
        "QPushButton:checked { background-color: #00ff88; color: #0f1419;"
        " border: 1px solid #00ff88; font-weight: bold; }";
    for (auto *b : {ui->pushButton_speedFast, ui->pushButton_speedMedium, ui->pushButton_speedSlow})
        b->setStyleSheet(speedStyle);

    // Кнопки режимов управления
    const QString modeStyle =
        "QPushButton { background-color: #1e2a38; border: 1px solid #2d4052; border-radius: 4px;"
        " color: #e0e6ed; min-height: 24px; }"
        "QPushButton:hover { border: 1px solid #00bcd4; color: #ffffff; }"
        "QPushButton:checked { background-color: #00bcd4; color: #0f1419; font-weight: bold; }";
    for (auto *b : {ui->pushButton_modeManual, ui->pushButton_modeAutomated, ui->pushButton_modeAutomatic})
        b->setStyleSheet(modeStyle);

    // Кнопки каналов автоматизации
    for (auto *b : {ui->pushButton_modeAutomated_surge, ui->pushButton_modeAutomated_sway,
                    ui->pushButton_modeAutomated_depth, ui->pushButton_modeAutomated_yaw,
                    ui->pushButton_modeAutomated_pitch, ui->pushButton_modeAutomated_roll})
        b->setStyleSheet(modeStyle);
}

MainWindow::~MainWindow()
{
    delete ui;
}