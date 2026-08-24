#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QtMath>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <cmath>
#include <chrono>
#include <cmath>

const QString MainWindow::kPlaqueOk  = "background-color: #00ff88; color: #0f1419;"
    " font-weight: bold; border-radius: 6px; border: none; font-size: 15px; padding: 4px 10px;";
const QString MainWindow::kPlaqueErr = "background-color: #ff4444; color: #ffffff;"
    " font-weight: bold; border-radius: 6px; border: none; font-size: 15px; padding: 4px 10px;";

static QIcon makeSunIcon()
{
    QPixmap pm(24, 24);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);

    const QPointF c(12, 12);
    p.setPen(QPen(QColor("#f5a623"), 2, Qt::SolidLine, Qt::RoundCap));
    for (int i = 0; i < 8; ++i) {              // лучи
        const qreal a = i * M_PI / 4.0;
        p.drawLine(QPointF(c.x() + 8  * std::cos(a), c.y() + 8  * std::sin(a)),
                   QPointF(c.x() + 11 * std::cos(a), c.y() + 11 * std::sin(a)));
    }
    p.setPen(QPen(QColor("#f5a623"), 1.5));    // диск
    p.setBrush(QColor("#ffd54f"));
    p.drawEllipse(c, 5.5, 5.5);
    p.end();
    return QIcon(pm);
}

static QIcon makeMoonIcon()
{
    QPixmap pm(24, 24);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);

    QPainterPath moon;
    moon.addEllipse(QRectF(3, 3, 18, 18));
    QPainterPath cut;                          // вырезаем серп
    cut.addEllipse(QRectF(9, 1, 16, 16));
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#cfd8dc"));
    p.drawPath(moon - cut);
    p.end();
    return QIcon(pm);
}

static const QString LIGHT_THEME =
"QMainWindow, QWidget#centralwidget { background-color: #f2f4f6; color: #1c2733; }"
"QPushButton { background-color: #ffffff; border: 1px solid #c3cad2; border-radius: 4px; padding: 5px 12px; color: #1c2733; min-height: 20px; font-size: 12px; }"
"QPushButton:hover { border: 1px solid #0088bb; color: #006699; }"
"QPushButton:pressed { background-color: #00a0d0; color: #ffffff; }"
"QPushButton:disabled { background-color: #e6e9ec; color: #9aa4ad; border: 1px solid #d4d9de; }"
"QPushButton:checked { background-color: #00a0d0; color: #ffffff; font-weight: bold; }"
"QTabWidget::pane { border: 1px solid #c3cad2; background-color: #ffffff; border-radius: 4px; }"
"QTabBar::tab { background-color: #e4e8ec; color: #5a6672; padding: 6px 14px; border: 1px solid #c3cad2; border-bottom: none; font-size: 12px; }"
"QTabBar::tab:selected { background-color: #00a0d0; color: #ffffff; font-weight: bold; }"
"QTabBar::tab:hover:!selected { background-color: #d7dde3; }"
"QLineEdit, QSpinBox, QComboBox, QDoubleSpinBox { background-color: #ffffff; border: 1px solid #c3cad2; border-radius: 3px; padding: 3px 6px; color: #1c2733; font-size: 12px; }"
"QLineEdit:focus, QSpinBox:focus, QComboBox:focus { border: 1px solid #0088bb; }"
"QGroupBox { border: 1px solid #c3cad2; border-radius: 5px; margin-top: 10px; font-weight: bold; color: #0077aa; background-color: #ffffff; padding-top: 14px; font-size: 12px; }"
"QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; color: #0077aa; }"
"QRadioButton, QCheckBox { color: #1c2733; spacing: 5px; font-size: 12px; }"
"QTextEdit { background-color: #ffffff; color: #007a33; border: 1px solid #c3cad2; border-radius: 3px; padding: 3px; font-family: 'Consolas', 'Courier New', monospace; font-size: 11px; }"
"Line { color: #c3cad2; max-height: 1px; }"
"QLabel { color: #1c2733; background: transparent; font-size: 12px; }"
"QScrollBar:vertical { background: #eef1f4; width: 10px; }"
"QScrollBar::handle:vertical { background: #c3cad2; min-height: 20px; border-radius: 5px; }"
"QScrollBar::handle:vertical:hover { background: #0088bb; }"
"QScrollBar:horizontal { background: #eef1f4; height: 10px; }"
"QScrollBar::handle:horizontal { background: #c3cad2; min-width: 20px; border-radius: 5px; }"
"QMenuBar { background-color: #f2f4f6; color: #1c2733; border-bottom: 1px solid #c3cad2; }"
"QMenuBar::item:selected { background-color: #00a0d0; color: #ffffff; }"
"QMenu { background-color: #ffffff; border: 1px solid #c3cad2; color: #1c2733; }"
"QMenu::item:selected { background-color: #00a0d0; color: #ffffff; }"
"QStatusBar { background-color: #f2f4f6; color: #5a6672; border-top: 1px solid #c3cad2; }"
"QToolTip { background-color: #ffffff; color: #1c2733; border: 1px solid #0088bb; padding: 3px; border-radius: 3px; }";

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

        // === Телеметрия из ROS -> плашки ===
    connect(rosBridge, &RosBridge::poseUpdated, this, [this](double x, double y, double z) {
        updatePlaque(ui->lbl_depth_value, QString::number(z, 'f', 2) + " м");
        updateSpeedFromPose(x, y);
        //kickWatchdog(); //TODO: закомментить, пока не будут реализованы все топики телеметрии, иначе будет постоянно сбрасываться телеметрия в N/A
    }, Qt::QueuedConnection);

    connect(rosBridge, &RosBridge::bottomReceived, this, [this](double m) {
        updatePlaque(ui->lbl_bottom_value, QString::number(m, 'f', 2) + " м", m < 0.5);
        //kickWatchdog(); //TODO: закомментить, пока не будут реализованы все топики телеметрии, иначе будет постоянно сбрасываться телеметрия в N/A
    }, Qt::QueuedConnection);

    connect(rosBridge, &RosBridge::temperatureReceived, this, [this](double t) {
        updatePlaque(ui->lbl_temp_value, QString::number(t, 'f', 0) + "°C", t > 60.0);
    }, Qt::QueuedConnection);

    connect(rosBridge, &RosBridge::leakReceived, this, [this](bool leak) {
        updatePlaque(ui->lbl_leak_value, leak ? "ЕСТЬ!" : "OK", leak);
    }, Qt::QueuedConnection);

    connect(rosBridge, &RosBridge::battery1Received, this, [this](double p) {
        updatePlaque(ui->lbl_voltage_value, QString::number(p, 'f', 0) + " %", p < 20.0);
    }, Qt::QueuedConnection);

    connect(rosBridge, &RosBridge::battery2Received, this, [this](double p) {
        updatePlaque(ui->lbl_voltage2_value, QString::number(p, 'f', 0) + " %", p < 20.0);
    }, Qt::QueuedConnection);

    connect(rosBridge, &RosBridge::killswitchReceived, this, [this](bool active) {
        updateKillswitch(active);
    }, Qt::QueuedConnection);

    connect(rosBridge, &RosBridge::heartbeatReceived, this, [this](int ms) {
        updatePlaque(ui->lbl_ping_value, QString::number(ms) + " мс");
        //kickWatchdog(); //TODO: закомментить, пока не будут реализованы все топики телеметрии, иначе будет постоянно сбрасываться телеметрия в N/A
    }, Qt::QueuedConnection);

    // Watchdog: нет данных 3 с → сброс в N/A
    // telemetryWatchdog_ = new QTimer(this);
    // telemetryWatchdog_->setInterval(3000);
    // connect(telemetryWatchdog_, &QTimer::timeout, this, &MainWindow::resetTelemetryToDefault);
    // telemetryWatchdog_->start();

    setWidget();
    setConsole();
    setTimer_updateImpact(10);
    setBottom();

    darkStyle_ = styleSheet();

    ui->btn_theme_toggle->setText(QString());
    ui->btn_theme_toggle->setIconSize(QSize(22, 22));
    applyTheme(true);
    ui->btn_theme_toggle->setIcon(makeMoonIcon());

    connect(ui->btn_theme_toggle, &QPushButton::clicked,
            this, &MainWindow::toggleTheme);

    setupButtonStyles(true);

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

void MainWindow::updatePlaque(QLabel *label, const QString &text, bool alarm)
{
    label->setStyleSheet(alarm ? kPlaqueErr : kPlaqueOk);
    label->setText(text);
}

void MainWindow::updateKillswitch(bool active)
{
    // active = true → красный (опасность), false → зелёный (норма)
    ui->btn_killswitch_status->setStyleSheet(active ? kPlaqueErr : kPlaqueOk);
    ui->btn_killswitch_status->setText(active ? "НАЖАТ" : "НЕ НАЖАТ");
}

void MainWindow::updateSpeedFromPose(double x, double y)
{
    const auto now = std::chrono::steady_clock::now();
    if (lastPoseTime_.time_since_epoch().count() != 0) {
        const double dt = std::chrono::duration<double>(now - lastPoseTime_).count();
        if (dt > 1e-6) {
            const double dx = x - lastPoseX_;
            const double dy = y - lastPoseY_;
            const double v = std::sqrt(dx*dx + dy*dy) / dt;
            updatePlaque(ui->lbl_speed_value, QString::number(v, 'f', 2) + " м/с");
        }
    }
    lastPoseX_ = x; lastPoseY_ = y; lastPoseTime_ = now;
}

void MainWindow::kickWatchdog()
{
    telemetryWatchdog_->start();  // перезапуск — «я ещё жив»
}

void MainWindow::resetTelemetryToDefault()
{
    isConnected = false;
    for (QLabel *l : {ui->lbl_depth_value, ui->lbl_bottom_value, ui->lbl_speed_value,
                      ui->lbl_temp_value,  ui->lbl_leak_value,   ui->lbl_voltage_value,
                      ui->lbl_voltage2_value, ui->lbl_ping_value}) {
        l->setStyleSheet(kPlaqueErr);
        l->setText("N/A");
    }
    updateKillswitch(false);
    lastPoseTime_ = {};
    displayText("Таймаут телеметрии: сброс плашек");
}

void MainWindow::toggleTheme()
{
    isDark_ = !isDark_;
    applyTheme(isDark_);
}

void MainWindow::applyTheme(bool dark)
{
    setStyleSheet(dark ? darkStyle_ : LIGHT_THEME);

    ui->stackedWidget_mode->setStyleSheet(dark
        ? "background-color: #151b23; border: 1px solid #2d4052; border-radius: 6px;"
        : "background-color: #ffffff; border: 1px solid #c3cad2; border-radius: 6px;");

    const QString titleStyle = dark
        ? "color: #00bcd4; font-weight: bold; font-size: 15px;"
        : "color: #0077aa; font-weight: bold; font-size: 15px;";
    for (QLabel *l : {ui->lbl_depth_title, ui->lbl_bottom_title, ui->lbl_speed_title,
                      ui->lbl_temp_title,  ui->lbl_leak_title,   ui->lbl_voltage_title,
                      ui->lbl_voltage2_title, ui->lbl_kill_title, ui->lbl_ping_title})
        l->setStyleSheet(titleStyle);

    ui->lbl_heading->setStyleSheet(dark
        ? "font-size: 14px; font-weight: bold; color: #00bcd4;"
        : "font-size: 14px; font-weight: bold; color: #0077aa;");

    setupButtonStyles(dark);

    ui->btn_theme_toggle->setStyleSheet(dark
        ? "QPushButton { background-color: #1e2a38; border: 2px solid #2d4052; border-radius: 8px; }"
          "QPushButton:hover { border-color: #00bcd4; }"
          "QPushButton:pressed { background-color: #2d4052; }"
        : "QPushButton { background-color: #ffffff; border: 2px solid #c3cad2; border-radius: 8px; }"
          "QPushButton:hover { border-color: #0088bb; }"
          "QPushButton:pressed { background-color: #e4e8ec; }");

    ui->btn_theme_toggle->setIcon(dark ? makeMoonIcon() : makeSunIcon());
}

void MainWindow::setupButtonStyles(bool dark)
{
    const QString speedStyle = dark
        ? "QPushButton { background-color: #1e2a38; border: 1px solid #2d4052; border-radius: 4px;"
          " color: #e0e6ed; min-height: 28px; font-size: 13px; }"
          "QPushButton:hover { border: 1px solid #00ff88; color: #ffffff; }"
          "QPushButton:checked { background-color: #00ff88; color: #0f1419;"
          " border: 1px solid #00ff88; font-weight: bold; }"
        : "QPushButton { background-color: #ffffff; border: 1px solid #c3cad2; border-radius: 4px;"
          " color: #1c2733; min-height: 28px; font-size: 13px; }"
          "QPushButton:hover { border: 1px solid #00994d; color: #006633; }"
          "QPushButton:checked { background-color: #00c86e; color: #05331f;"
          " border: 1px solid #00c86e; font-weight: bold; }";
    for (auto *b : {ui->pushButton_speedFast, ui->pushButton_speedMedium, ui->pushButton_speedSlow})
        b->setStyleSheet(speedStyle);

    const QString modeStyle = dark
        ? "QPushButton { background-color: #1e2a38; border: 1px solid #2d4052; border-radius: 4px;"
          " color: #e0e6ed; min-height: 24px; }"
          "QPushButton:hover { border: 1px solid #00bcd4; color: #ffffff; }"
          "QPushButton:checked { background-color: #00bcd4; color: #0f1419; font-weight: bold; }"
        : "QPushButton { background-color: #ffffff; border: 1px solid #c3cad2; border-radius: 4px;"
          " color: #1c2733; min-height: 24px; }"
          "QPushButton:hover { border: 1px solid #0088bb; color: #006699; }"
          "QPushButton:checked { background-color: #00a0d0; color: #ffffff; font-weight: bold; }";
    for (auto *b : {ui->pushButton_modeManual, ui->pushButton_modeAutomated, ui->pushButton_modeAutomatic,
                    ui->pushButton_modeAutomated_surge, ui->pushButton_modeAutomated_sway,
                    ui->pushButton_modeAutomated_depth, ui->pushButton_modeAutomated_yaw,
                    ui->pushButton_modeAutomated_pitch, ui->pushButton_modeAutomated_roll})
        b->setStyleSheet(modeStyle);
}

MainWindow::~MainWindow()
{
    delete ui;
}