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

    // connect(&uv_interface, SIGNAL(displayText_toConsole(QString)),
    //         this, SLOT(displayText(QString)));
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

    updateTimer = new QTimer(this);
    connect(
        updateTimer, SIGNAL(timeout()),
        this, SLOT(updateUi_fromControl())
        );
    updateTimer->start(periodUpdateMsec);
    displayText("Таймер обновления джойстика запущен");
}

void MainWindow::useKeyBoard()
{
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
    activeInput = joyStick.get();
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


    // e_CSModeManualToggled();

    // connect(
    //     ui->pushButton_modeManual, SIGNAL(clicked()),
    //     this, SLOT(e_CSModeManualToggled()));

    // connect(
    //     ui->pushButton_modeAutomated, SIGNAL(clicked()),
    //     this, SLOT(e_CSModeAutomatedToggled()));

    // connect(
    //     ui->pushButton_modeAutomatic, SIGNAL(clicked()),
    //     this, SLOT(e_CSModeAutomaticToggled()));
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

void MainWindow::updateUi_Compass(float yaw) {
    ui->compass->setYaw(yaw);
}

MainWindow::~MainWindow()
{
    delete ui;
}
