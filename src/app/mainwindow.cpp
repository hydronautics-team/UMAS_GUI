#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "rosbag_recorder.h"
#include <QAction>
#include <QtMath>
#include <QDir>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QPushButton>
#include <QStandardPaths>
#include <QStatusBar>
#include <QFontMetrics>
#include <QTabBar>
#include <QApplication>
#include <cmath>

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
"QMainWindow { background-color: #dcecea; color: #16383d; font-family: 'Inter', 'Segoe UI', sans-serif; font-size: 13px; }"
"QWidget#centralwidget { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f5fbfa, stop:0.55 #eaf5f3, stop:1 #dcecea); color: #16383d; font-family: 'Inter', 'Segoe UI', sans-serif; font-size: 13px; }"
"QWidget { outline: none; }"
"QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f3f9f8, stop:1 #dceae8); border: 1px solid #91b9b7; border-radius: 8px; padding: 6px 13px; color: #16383d; min-height: 22px; font-size: 12px; font-weight: 500; }"
"QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f7fbfa, stop:1 #cce8e4); border: 1px solid #159d93; color: #075e58; }"
"QPushButton:pressed { background: #c5dfdc; border: 1px solid #0f8f87; color: #064d49; padding: 6px 13px; }"
"QPushButton:checked { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #62ddd2, stop:1 #19aaa0); border: 1px solid #07847c; color: #052f2c; font-weight: 700; }"
"QPushButton:disabled { background-color: #e4eeed; color: #829c9d; border-color: #bfd1d0; }"
"QTabWidget::pane { border: 1px solid #b5d2d0; background-color: #edf5f4; border-radius: 9px; top: -1px; }"
"QTabBar::tab { background-color: transparent; color: #557b7d; padding: 8px 16px; margin-right: 3px; border: none; border-bottom: 2px solid transparent; font-weight: 600; }"
"QTabBar::tab:selected { color: #087f77; border-bottom-color: #15a99f; }"
"QTabBar::tab:hover:!selected { color: #075e58; background-color: #dcefed; }"
"QLineEdit, QSpinBox, QComboBox, QDoubleSpinBox { background-color: #f2f8f7; border: 1px solid #a8c9c7; border-radius: 6px; padding: 5px 8px; color: #16383d; selection-background-color: #42c6bc; selection-color: #052f2c; }"
"QLineEdit:focus, QSpinBox:focus, QComboBox:focus, QDoubleSpinBox:focus { border: 1px solid #159d93; background-color: #f7fbfa; }"
"QComboBox QAbstractItemView { background-color: #f2f8f7; border: 1px solid #91b9b7; color: #16383d; selection-background-color: #bde9e5; selection-color: #064d49; }"
"QGroupBox { border: 1px solid #b5d2d0; border-radius: 9px; margin-top: 12px; padding-top: 15px; background-color: #edf5f4; color: #087f77; font-weight: 600; }"
"QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 7px; color: #087f77; }"
"QRadioButton, QCheckBox { color: #23474c; spacing: 7px; }"
"QRadioButton::indicator, QCheckBox::indicator { width: 15px; height: 15px; border: 1px solid #78aaa7; background-color: #f2f8f7; }"
"QRadioButton::indicator { border-radius: 8px; } QCheckBox::indicator { border-radius: 4px; }"
"QRadioButton::indicator:checked, QCheckBox::indicator:checked { background-color: #1ab2a7; border: 2px solid #087f77; }"
"QTextEdit { background-color: #f0f7f6; color: #12665f; border: 1px solid #b5d2d0; border-radius: 7px; padding: 6px; font-family: 'JetBrains Mono', 'Consolas', monospace; }"
"QLCDNumber { background-color: #eef8f7; color: #087f77; border: 1px solid #b5d2d0; border-radius: 6px; }"
"QLabel { color: #23474c; background: transparent; }"
"QFrame#frame_telemetry { background-color: #edf5f4; border: 1px solid #b5d2d0; border-radius: 10px; padding: 5px; }"
"QLabel[hudValue='true'], QPushButton[hudValue='true'] { background-color: #e0f2f0; color: #087f77; border: 1px solid #abd2cf; border-radius: 7px; padding: 4px 9px; font-weight: 600; }"
"QPushButton#btn_killswitch_status { min-height: 0px; padding: 4px 10px; margin: 0px; }"
"QPushButton#btn_killswitch_status:disabled { color: #8f1830; background-color: #ffdce2; border: 1px solid #ef7185; }"
"QLabel[controlValue='true'] { background-color: #e0f2f0; color: #087f77; border: 1px solid #abd2cf; border-radius: 5px; padding: 3px 7px; min-width: 24px; font-family: 'JetBrains Mono', 'Consolas', monospace; font-weight: 600; }"
"QStackedWidget { background-color: #edf5f4; border: 1px solid #b5d2d0; border-radius: 9px; }"
"QTableView, QTableWidget, QListView, QTreeView { background-color: #f0f7f6; alternate-background-color: #e5f0ee; color: #16383d; border: 1px solid #b5d2d0; border-radius: 6px; gridline-color: #c8dcda; selection-background-color: #bde9e5; selection-color: #064d49; }"
"QHeaderView::section { background-color: #dcefed; color: #087f77; border: none; border-right: 1px solid #b5d2d0; border-bottom: 1px solid #b5d2d0; padding: 6px; font-weight: 700; }"
"QProgressBar { background-color: #e1efed; color: #16383d; border: 1px solid #b5d2d0; border-radius: 6px; text-align: center; } QProgressBar::chunk { background-color: #1ab2a7; border-radius: 5px; }"
"QSlider::groove:horizontal { background: #c8dcda; height: 5px; border-radius: 2px; } QSlider::sub-page:horizontal { background: #1ab2a7; border-radius: 2px; }"
"QSlider::handle:horizontal { background: #f2f8f7; border: 2px solid #159d93; width: 15px; margin: -6px 0; border-radius: 8px; }"
"QScrollBar:vertical { background: #e6f1ef; width: 9px; margin: 0; } QScrollBar::handle:vertical { background: #8bb9b6; min-height: 24px; border-radius: 4px; } QScrollBar::handle:vertical:hover { background: #159d93; }"
"QMenuBar { background-color: #eef8f7; border: none; } QStatusBar { background-color: #eef8f7; color: #557b7d; border: none; }"
"QToolTip { background-color: #f2f8f7; color: #16383d; border: 1px solid #159d93; padding: 5px; border-radius: 4px; }";

static const QString DARK_THEME =
"QMainWindow { background-color: #031419; color: #d7f3f1; font-family: 'Inter', 'Segoe UI', sans-serif; font-size: 13px; }"
"QWidget#centralwidget { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #041820, stop:0.55 #06232b, stop:1 #082d34); color: #d7f3f1; font-family: 'Inter', 'Segoe UI', sans-serif; font-size: 13px; }"
"QWidget { outline: none; }"
"QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #124653, stop:1 #0a303a); border: 1px solid #236574; border-radius: 8px; padding: 6px 13px; color: #d7f3f1; min-height: 22px; font-size: 12px; font-weight: 500; }"
"QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #176173, stop:1 #0d414d); border: 1px solid #74eee3; color: #ffffff; }"
"QPushButton:pressed { background: #0b2a32; border: 1px solid #20c7b8; color: #8af5ec; padding: 6px 13px; }"
"QPushButton:checked { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #74eee3, stop:1 #20b9ad); border: 1px solid #a7fff7; color: #031416; font-weight: 700; }"
"QPushButton:disabled { background-color: #0a252d; color: #55747a; border-color: #123b45; }"
"QTabWidget::pane { border: 1px solid #174a56; background-color: #09262e; border-radius: 8px; top: -1px; }"
"QTabBar::tab { background-color: transparent; color: #7faaa9; padding: 8px 16px; margin-right: 3px; border: none; border-bottom: 2px solid transparent; font-weight: 600; }"
"QTabBar::tab:selected { color: #74eee3; border-bottom-color: #20c7b8; }"
"QTabBar::tab:hover:!selected { color: #c4e9e6; background-color: #0d3540; }"
"QLineEdit, QSpinBox, QComboBox, QDoubleSpinBox { background-color: #071f27; border: 1px solid #185464; border-radius: 6px; padding: 5px 8px; color: #e7fbf9; selection-background-color: #20c7b8; selection-color: #031416; }"
"QLineEdit:focus, QSpinBox:focus, QComboBox:focus, QDoubleSpinBox:focus { border: 1px solid #5de2d7; background-color: #0a2932; }"
"QComboBox QAbstractItemView { background-color: #09262e; border: 1px solid #247080; color: #d7f3f1; selection-background-color: #19b8aa; selection-color: #031416; }"
"QGroupBox { border: 1px solid #174a56; border-radius: 9px; margin-top: 12px; padding-top: 15px; background-color: #09262e; color: #66e2d7; font-weight: 600; }"
"QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 7px; color: #66e2d7; }"
"QRadioButton, QCheckBox { color: #c9e7e5; spacing: 7px; }"
"QRadioButton::indicator, QCheckBox::indicator { width: 15px; height: 15px; border: 1px solid #247080; background-color: #071f27; }"
"QRadioButton::indicator { border-radius: 8px; }"
"QCheckBox::indicator { border-radius: 4px; }"
"QRadioButton::indicator:checked, QCheckBox::indicator:checked { background-color: #20c7b8; border: 2px solid #8af5ec; }"
"QTextEdit { background-color: #04161c; color: #7de5d5; border: 1px solid #174a56; border-radius: 7px; padding: 6px; font-family: 'JetBrains Mono', 'Consolas', monospace; }"
"QLCDNumber { background-color: #04161c; color: #5de2d7; border: 1px solid #174a56; border-radius: 6px; }"
"QLabel { color: #c9e7e5; background: transparent; }"
"QFrame#frame_telemetry { background-color: #09262e; border: 1px solid #174a56; border-radius: 10px; padding: 5px; }"
"QLabel[hudValue='true'], QPushButton[hudValue='true'] { background-color: #0b3039; color: #74eee3; border: 1px solid #1d5965; border-radius: 7px; padding: 4px 9px; font-weight: 600; }"
"QPushButton#btn_killswitch_status { min-height: 0px; padding: 4px 10px; margin: 0px; }"
"QPushButton#btn_killswitch_status:disabled { color: #ffe8ec; background-color: #a3263d; border: 1px solid #f05268; }"
"QLabel[controlValue='true'] { background-color: #041a20; color: #74eee3; border: 1px solid #174a56; border-radius: 5px; padding: 3px 7px; min-width: 24px; font-family: 'JetBrains Mono', 'Consolas', monospace; font-weight: 600; }"
"QStackedWidget { background-color: #09262e; border: 1px solid #174a56; border-radius: 9px; }"
"QTableView, QTableWidget, QListView, QTreeView { background-color: #071f27; alternate-background-color: #0a2932; color: #d7f3f1; border: 1px solid #174a56; border-radius: 6px; gridline-color: #174a56; selection-background-color: #12665f; selection-color: #efffff; }"
"QHeaderView::section { background-color: #0d3540; color: #74eee3; border: none; border-right: 1px solid #174a56; border-bottom: 1px solid #174a56; padding: 6px; font-weight: 600; }"
"QProgressBar { background-color: #071f27; color: #d7f3f1; border: 1px solid #174a56; border-radius: 6px; text-align: center; }"
"QProgressBar::chunk { background-color: #20c7b8; border-radius: 5px; }"
"QFrame[frameShape='4'], QFrame[frameShape='5'] { color: #174a56; max-height: 1px; }"
"QSlider::groove:horizontal { background: #123b45; height: 5px; border-radius: 2px; }"
"QSlider::sub-page:horizontal { background: #20c7b8; border-radius: 2px; }"
"QSlider::handle:horizontal { background: #8af5ec; border: 2px solid #0d3540; width: 15px; margin: -6px 0; border-radius: 8px; }"
"QScrollBar:vertical { background: #071f27; width: 9px; margin: 0; }"
"QScrollBar::handle:vertical { background: #226674; min-height: 24px; border-radius: 4px; }"
"QScrollBar::handle:vertical:hover { background: #20c7b8; }"
"QMenuBar { background-color: #04161c; border: none; }"
"QStatusBar { background-color: #04161c; color: #7faaa9; border: none; }"
"QToolTip { background-color: #0d3540; color: #efffff; border: 1px solid #5de2d7; padding: 5px; border-radius: 4px; }";

static void setupRosbagRecording(MainWindow *window)
{
    auto *recorder = new RosbagRecorder(window);
    QMenu *recordingMenu = window->menuBar()->addMenu(QStringLiteral("Запись"));
    QAction *startAction = recordingMenu->addAction(QStringLiteral("Начать запись rosbag"));
    QAction *stopAction = recordingMenu->addAction(QStringLiteral("Остановить запись rosbag"));
    stopAction->setEnabled(false);

    auto *statusLabel = new QLabel(QStringLiteral("ROSBAG: ВЫКЛ"), window);
    statusLabel->setToolTip(QStringLiteral("Состояние локальной записи ROS 2 topics"));
    window->statusBar()->addPermanentWidget(statusLabel);

    const QString configuredRoot = qEnvironmentVariable("UMAS_ROSBAG_DIR").trimmed();
    const QString defaultRoot =
        QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation))
            .filePath(QStringLiteral("UMAS/bags"));
    const QString outputRoot = configuredRoot.isEmpty() ? defaultRoot : configuredRoot;

    const QStringList topics = {QStringLiteral("/vectornav/raw/common"),
                                QStringLiteral("/vectornav/imu"),
                                QStringLiteral("/dvl/data"),
                                QStringLiteral("/dvl/position"),
                                QStringLiteral("/stingray_core/pressure_sensor/depth"),
                                QStringLiteral("/stingray_core/battery/state"),
                                QStringLiteral("/core/state/odometry"),
                                QStringLiteral("/control/data"),
                                QStringLiteral("/control/loop_flags"),
                                QStringLiteral("/thruster/cmd"),
                                QStringLiteral("/diagnostics"),
                                QStringLiteral("/tf"),
                                QStringLiteral("/tf_static"),
                                QStringLiteral("/parameter_events")};

    QObject::connect(startAction, &QAction::triggered, window,
                     [recorder, outputRoot, topics]()
                     { recorder->startRecording(outputRoot, topics); });
    QObject::connect(stopAction, &QAction::triggered, recorder, &RosbagRecorder::stopRecording);

    QObject::connect(recorder, &RosbagRecorder::recordingStarted, window,
                     [startAction, stopAction, statusLabel](const QString &path)
                     {
                         startAction->setEnabled(false);
                         stopAction->setEnabled(true);
                         statusLabel->setText(QStringLiteral("ROSBAG: ЗАПИСЬ"));
                         statusLabel->setToolTip(path);
                     });

    QObject::connect(recorder, &RosbagRecorder::recordingStopped, window,
                     [startAction, stopAction, statusLabel](const QString &path)
                     {
                         startAction->setEnabled(true);
                         stopAction->setEnabled(false);
                         statusLabel->setText(QStringLiteral("ROSBAG: ВЫКЛ"));
                         statusLabel->setToolTip(path);
                     });

    QObject::connect(recorder, &RosbagRecorder::recordingError, window,
                     [recorder, startAction, stopAction, statusLabel](const QString &message)
                     {
                         startAction->setEnabled(!recorder->isRecording());
                         stopAction->setEnabled(recorder->isRecording());
                         statusLabel->setText(QStringLiteral("ROSBAG: ОШИБКА"));
                         statusLabel->setToolTip(message);
                         qWarning() << "Rosbag recording error:" << message;
                     });
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("Hydronautics • UMAS Control Station"));
    harmonizeLegacyStyles();
    setupBrandHeader();
    setupRosbagRecording(this);

    qRegisterMetaType<UVState::ImuData>("UVState::ImuData");
    qRegisterMetaType<UVState::Pose>("UVState::Pose");
    qRegisterMetaType<UVState::BatteryData>("UVState::BatteryData");

    uvState = new UVState(this);

    rosBridge = new RosBridge(this);
    rosBridge->start();

    connect(this, &MainWindow::publishTwistRequested,
            rosBridge, &RosBridge::publishTwistInternal,
            Qt::QueuedConnection);

    connect(rosBridge, &RosBridge::poseReceived,
            uvState, static_cast<void (UVState::*)(const UVState::Pose&)>(&UVState::setPose),
            Qt::QueuedConnection);

    connect(rosBridge, &RosBridge::imuReceived,
            uvState, &UVState::setImu,
            Qt::QueuedConnection);

    connect(rosBridge, &RosBridge::batteryReceived,
            uvState, &UVState::setBattery,
            Qt::QueuedConnection);

    connect(uvState, &UVState::imuUpdated, this, [this](const UVState::ImuData& imu) {
        ui->compass->setYaw(imu.yaw);
        ui->attitudeIndicator->setAttitude(imu.pitch, imu.roll);
        ui->lbl_heading->setText(QStringLiteral("КУРС: %1°")
                                     .arg(static_cast<int>(qRound(imu.yaw + 360.0f)) % 360, 3, 10, QLatin1Char('0')));
        ui->lbl_pitch->setText(QStringLiteral("ДИФФЕРЕНТ: %1°").arg(imu.pitch, 0, 'f', 1));
        ui->lbl_roll->setText(QStringLiteral("КРЕН: %1°").arg(imu.roll, 0, 'f', 1));
    });

    connect(rosBridge, &RosBridge::controlFlagsPublished,
            uvState, &UVState::setControlFlags,
            Qt::QueuedConnection);

    setWidget();
    setConsole();
    setTimer_updateImpact(10);
    setBottom();

    darkStyle_ = DARK_THEME;

    ui->btn_theme_toggle->setText(QString());
    ui->btn_theme_toggle->setIconSize(QSize(22, 22));

    connect(ui->btn_theme_toggle, &QPushButton::clicked,
            this, &MainWindow::toggleTheme);

    setTab();
    setupActuators();

        brightnessTimer_ = new QTimer(this);
    brightnessTimer_->setInterval(100);  // каждые 100 мс
    connect(brightnessTimer_, &QTimer::timeout, this, [this]() {
        if (brightnessStep_ == 0) return;
        int current = ui->slider_brightness->value();
        int newValue = qBound(0, current + brightnessStep_, 500);
        ui->slider_brightness->setValue(newValue);
    });

    setUpdateUI();

    gainSpinBoxes = {
        ui->spinBox_gain_surge,
        ui->spinBox_gain_sway,
        ui->spinBox_gain_depth,
        ui->spinBox_gain_yaw,
        ui->spinBox_gain_pitch,
        ui->spinBox_gain_roll
    };
    qApp->installEventFilter(this);

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

    // Применяем тему после создания всех групп и настройки виджетов. Иначе
    // радиокнопки выбора источника управления впервые получают актуальный
    // стиль только после ручного переключения темы.
    applyTheme(isDark_);
   

    connect(ui->full_screen, &QPushButton::toggled, this, [this](bool checked) {
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

                    FullscreenVideoWindow* closedWindow = fullscreenWindow_;

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
                    if (closedWindow)
                        closedWindow->deleteLater();
                });

                fullscreenWindow_->showFullScreen();
            }
        } else {
            if (fullscreenWindow_) {
                fullscreenWindow_->close();
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

void MainWindow::setupBrandHeader()
{
    menuBar()->setFixedHeight(58);

    auto *brand = new QWidget(menuBar());
    brand->setObjectName(QStringLiteral("brandHeader"));
    auto *layout = new QHBoxLayout(brand);
    layout->setContentsMargins(14, 6, 18, 6);
    layout->setSpacing(11);

    auto *logo = new QLabel(brand);
    logo->setObjectName(QStringLiteral("brandLogo"));
    logo->setFixedSize(44, 44);
    logo->setAlignment(Qt::AlignCenter);
    logo->setPixmap(QPixmap(QStringLiteral(":/branding/hydronautics_logo.jpg"))
                        .scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logo->setStyleSheet(QStringLiteral(
        "background-color: #f7fbfc; border: 1px solid #245867; border-radius: 8px; padding: 1px;"));

    auto *title = new QLabel(
        QStringLiteral("<span style='font-size:16px; font-weight:700; letter-spacing:1px; color:#74eee3;'>HYDRONAUTICS</span>"
                       "<br><span style='font-size:10px; color:#7faaa9;'>UMAS CONTROL STATION</span>"),
        brand);
    title->setObjectName(QStringLiteral("brandTitle"));
    title->setTextFormat(Qt::RichText);

    layout->addWidget(logo);
    layout->addWidget(title);
    menuBar()->setCornerWidget(brand, Qt::TopLeftCorner);
}

void MainWindow::harmonizeLegacyStyles()
{
    // Приводим оставшиеся стили из .ui к общей палитре, пока старые формы
    // постепенно переводятся на централизованную тему.
    const QList<QPair<QString, QString>> palette = {
        {"#0d1b2a", "#061a21"}, {"#151b23", "#09262e"},
        {"#1e2a38", "#0d3540"}, {"#2d4052", "#185464"},
        {"#00bcd4", "#20c7b8"}, {"#00ff88", "#74eee3"},
        {"#0f1419", "#031416"}, {"#8a99a8", "#7faaa9"},
        {"#e0e6ed", "#d7f3f1"}, {"#0a0e13", "#04161c"},
        {"#4ade80", "#7de5d5"}, {"#4a5568", "#55747a"},
        {"#ff4444", "#a3263d"}
    };

    const auto widgets = findChildren<QWidget*>();
    for (QWidget *widget : widgets) {
        QString style = widget->styleSheet();
        if (!style.isEmpty()) {
            for (const auto &colors : palette)
                style.replace(colors.first, colors.second, Qt::CaseInsensitive);
            widget->setStyleSheet(style);
        }
    }

    for (QWidget *widget : {static_cast<QWidget*>(ui->lbl_depth_value),
                            static_cast<QWidget*>(ui->lbl_bottom_value),
                            static_cast<QWidget*>(ui->lbl_speed_value),
                            static_cast<QWidget*>(ui->lbl_temp_value),
                            static_cast<QWidget*>(ui->lbl_leak_value),
                            static_cast<QWidget*>(ui->lbl_voltage_value),
                            static_cast<QWidget*>(ui->lbl_voltage2_value),
                            static_cast<QWidget*>(ui->btn_killswitch_status),
                            static_cast<QWidget*>(ui->lbl_ping_value)})
        widget->setProperty("hudValue", true);

    for (QLabel *label : {ui->label_controlMarch, ui->label_controlYaw,
                          ui->label_controlLag, ui->label_controlDif,
                          ui->label_controlDepth, ui->label_controlKren})
        label->setProperty("controlValue", true);

    // Панели и интервалы уже формируют визуальную иерархию — дополнительные
    // горизонтальные линии только дробят компактный операторский интерфейс.
    for (QFrame *separator : {ui->line_separator1, ui->line_separator_light,
                              ui->line_separator2})
        separator->hide();

    ui->horizontalLayout_2->setContentsMargins(12, 10, 12, 10);
    ui->horizontalLayout_2->setSpacing(12);
    ui->horizontalLayout_telemetry->setSpacing(8);
}

void MainWindow::ensureInteractiveTextFits()
{
    const auto buttons = findChildren<QPushButton*>();
    for (QPushButton *button : buttons) {
        if (button->text().isEmpty() || button == ui->btn_theme_toggle ||
            button == ui->btn_killswitch_status)
            continue;

        const QFontMetrics metrics(button->font());
        const int textWidth = metrics.horizontalAdvance(button->text());
        const int safeWidth = qMin(260, textWidth + 34);
        const int safeHeight = metrics.height() + 18;
        button->setMinimumWidth(qMax(button->minimumWidth(), safeWidth));
        button->setMinimumHeight(qMax(button->minimumHeight(), safeHeight));
    }

    const QFontMetrics fullScreenMetrics(ui->full_screen->font());
    ui->full_screen->setMinimumWidth(fullScreenMetrics.horizontalAdvance(ui->full_screen->text()) + 54);
    ui->full_screen->setMinimumHeight(fullScreenMetrics.height() + 18);

    QTabBar *tabs = ui->tabWidget->tabBar();
    QFont tabFont = tabs->font();
    tabFont.setWeight(QFont::DemiBold);
    tabs->setFont(tabFont);
    tabs->setExpanding(false);
    tabs->setUsesScrollButtons(true);
    const QFontMetrics tabMetrics(tabFont);
    int requiredTabWidth = 0;
    for (int index = 0; index < tabs->count(); ++index)
        requiredTabWidth += tabMetrics.horizontalAdvance(tabs->tabText(index)) + 42;
    tabs->setMinimumWidth(requiredTabWidth);
}

void MainWindow::setWidget()
{
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
    if (ui->radioButton_useKeyBoard->isChecked()) {
    useKeyBoard();
} else if (ui->radioButton_useJoyStick->isChecked()) {
    useJoyStick();
} else if (ui->gamepad_btn->isChecked()) {
    useGamepad();
}

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

    // connect(gamepad, &Gamepad::backButtonPressed,
    //         this, &MainWindow::useKeyBoard);

    displayText("Геймпад подключен. Режим управления с геймпада активирован.");
    gamepadInput = std::make_unique<GamepadInputSource>(gamepad, this);
    activeInput = gamepadInput.get();

        // D-Pad вправо — увеличить яркость (пока зажата)
    connect(gamepad, &Gamepad::dPadRightPressed, this, [this]() {
        brightnessStep_ = +20;
        brightnessTimer_->start();
        // Сразу применить один шаг
        int current = ui->slider_brightness->value();
        ui->slider_brightness->setValue(qMin(current + 20, 500));
    });
    connect(gamepad, &Gamepad::dPadRightReleased, this, [this]() {
        brightnessStep_ = 0;
        brightnessTimer_->stop();
    });

    // D-Pad влево — уменьшить яркость (пока зажата)
    connect(gamepad, &Gamepad::dPadLeftPressed, this, [this]() {
        brightnessStep_ = -20;
        brightnessTimer_->start();
        int current = ui->slider_brightness->value();
        ui->slider_brightness->setValue(qMax(current - 20, 0));
    });
    connect(gamepad, &Gamepad::dPadLeftReleased, this, [this]() {
        brightnessStep_ = 0;
        brightnessTimer_->stop();
    });

    // L1 (number 4) — поворот влево (раньше было на L2)
    connect(gamepad, &Gamepad::L1Pressed, this, [this]() {
        onLeftTriggerMoved(100.0f);
    });
    connect(gamepad, &Gamepad::L1Released, this, [this]() {
        onLeftTriggerMoved(0.0f);
    });

    // R1 (number 5) — поворот вправо (раньше было на R2)
    connect(gamepad, &Gamepad::R1Pressed, this, [this]() {
        onRightTriggerMoved(100.0f);
    });
    connect(gamepad, &Gamepad::R1Released, this, [this]() {
        onRightTriggerMoved(0.0f);
    });

    // Y — «Разжать»
    connect(gamepad, &Gamepad::buttonYPressed,  this, [this]() {
        setGripState("open");
    });
    connect(gamepad, &Gamepad::buttonYReleased, this, [this]() {
        setGripState("stop");
    });

    // A — «Сжать»
    connect(gamepad, &Gamepad::buttonAPressed,  this, [this]() {
        setGripState("close");
    });
    connect(gamepad, &Gamepad::buttonAReleased, this, [this]() {
        setGripState("stop");
    });

        // Кнопка 11 — toggle: замкнуть/разомкнуть 3 контура
    connect(gamepad, &Gamepad::button11Pressed, this, [this]() {
        bool currentlyClosed = ui->pushButton_modeAutomated_yaw->isChecked()
                            && ui->pushButton_modeAutomated_pitch->isChecked()
                            && ui->pushButton_modeAutomated_roll->isChecked();

        if (currentlyClosed) {
            // Размыкаем
            ui->pushButton_modeAutomated_yaw->setChecked(false);
            ui->pushButton_modeAutomated_pitch->setChecked(false);
            ui->pushButton_modeAutomated_roll->setChecked(false);
            displayText("Геймпад: контуры Курс/Крен/Дифферент разомкнуты");
        } else {
            // Замыкаем (включаем автоматизированный режим если нужно)
            ui->pushButton_modeAutomated->setChecked(true);
            ui->pushButton_modeAutomated_yaw->setChecked(true);
            ui->pushButton_modeAutomated_pitch->setChecked(true);
            ui->pushButton_modeAutomated_roll->setChecked(true);
            displayText("Геймпад: контуры Курс/Крен/Дифферент замкнуты");
        }
    });

    // Кнопка 12 — круговая смена режима скоростей
    connect(gamepad, &Gamepad::button12Pressed, this, [this]() {
        int current = static_cast<int>(currentMode);
        int next = (current + 1) % 3;
        setSpeedMode(static_cast<SpeedMode>(next));
        displayText(QString("Геймпад: режим скорости → %1")
                    .arg(next == 0 ? "Слабый" : next == 1 ? "Средний" : "Быстрый"));
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

void MainWindow::setupActuators()
{
    // === Кнопки режимов света ===
    QButtonGroup *lightModeGroup = new QButtonGroup(this);
    lightModeGroup->addButton(ui->btn_light_off, 0);
    lightModeGroup->addButton(ui->btn_light_mode1, 1);
    lightModeGroup->addButton(ui->btn_light_mode2, 2);
    lightModeGroup->addButton(ui->btn_light_mode3, 3);
    lightModeGroup->setExclusive(true);

    connect(lightModeGroup, &QButtonGroup::idClicked, this, [this](int id) {
        onLightModeChanged(static_cast<unsigned>(id));
    });

    // === Слайдер яркости ===
    connect(ui->slider_brightness, &QSlider::valueChanged,
            this, &MainWindow::onBrightnessChanged);

    ui->slider_brightness->setEnabled(false);

    // === Кнопки манипулятора ===
    connect(ui->btn_manip_compress, &QPushButton::clicked, this, [this]() {
        setGripState("close");
    });
    connect(ui->btn_manip_release, &QPushButton::clicked, this, [this]() {
        setGripState("open");
    });
    connect(ui->btn_manip_rotate_cw, &QPushButton::clicked, this, [this]() {
        setTurnState("right");
    });
    connect(ui->btn_manip_rotate_ccw, &QPushButton::clicked, this, [this]() {
        setTurnState("left");
    });
    connect(ui->btn_manip_stop, &QPushButton::clicked, this, [this]() {
        setTurnState("stop");
        setGripState("stop");
    });
}

void MainWindow::onLightModeChanged(unsigned mode)
{
    rosBridge->publishLightsMode(mode);
    // Слайдер активен только когда включён хотя бы один режим (mode > 0)
    ui->slider_brightness->setEnabled(mode > 0);
    
    QString modeText;
    switch (mode) {
        case 0: modeText = "Свет выключен"; break;
        case 1: modeText = "Свет: Режим 1"; break;
        case 2: modeText = "Свет: Режим 2"; break;
        case 3: modeText = "Свет: Режим 3"; break;
        default: modeText = "Свет: Неизвестный режим"; break;
    }
    displayText(modeText);
}

void MainWindow::onBrightnessChanged(int value)
{
    ui->lbl_brightness_value->setText(QString::number(value));
    rosBridge->publishLightsBrightness(static_cast<unsigned>(value));
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
    QString noConnectionStyle = "background-color: #a3263d; color: #ffe8ec; font-weight: 600; border-radius: 7px; border: 1px solid #f05268; font-size: 15px; padding: 4px 10px;";
    
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
    QString connectedStyle = "background-color: #12665f; color: #dffffb; font-weight: 600; border-radius: 7px; border: 1px solid #20c7b8; font-size: 15px; padding: 4px 10px;";
    
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

void MainWindow::toggleTheme()
{
    isDark_ = !isDark_;
    applyTheme(isDark_);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QWidget *focusedWidget = QApplication::focusWidget();
        QWidget *clickedWidget = qobject_cast<QWidget *>(watched);

        for (QSpinBox *spinBox : gainSpinBoxes) {
            const bool ownsFocus = focusedWidget
                && (focusedWidget == spinBox || spinBox->isAncestorOf(focusedWidget));
            const bool clickedInside = clickedWidget
                && (clickedWidget == spinBox || spinBox->isAncestorOf(clickedWidget));

            if (ownsFocus && !clickedInside) {
                spinBox->clearFocus();
                break;
            }
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::applyTheme(bool dark)
{
    setStyleSheet(dark ? DARK_THEME : LIGHT_THEME);

    ui->stackedWidget_mode->setStyleSheet(dark
        ? "background-color: #09262e; border: 1px solid #174a56; border-radius: 9px;"
        : "background-color: #edf5f4; border: 1px solid #b5d2d0; border-radius: 9px;");

    const QString titleStyle = dark
        ? "color: #66e2d7; font-weight: 600; font-size: 15px;"
        : "color: #087f77; font-weight: 600; font-size: 15px;";
    for (QLabel *l : {ui->lbl_depth_title, ui->lbl_bottom_title, ui->lbl_speed_title,
                      ui->lbl_temp_title,  ui->lbl_leak_title,   ui->lbl_voltage_title,
                      ui->lbl_voltage2_title, ui->lbl_kill_title, ui->lbl_ping_title})
        l->setStyleSheet(titleStyle);

    ui->lbl_heading->setStyleSheet(dark
        ? "font-size: 14px; font-weight: 600; color: #74eee3;"
        : "font-size: 14px; font-weight: 600; color: #087f77;");

    setupButtonStyles(dark);

    // Локальный стиль исключает зависимость переключателей источника
    // управления от момента полировки глобальной таблицы стилей Qt.
    const QString inputSelectorStyle = dark
        ? "QRadioButton { color: #c9e7e5; spacing: 8px; font-weight: 500; }"
          "QRadioButton::indicator { width: 16px; height: 16px; border-radius: 9px; border: 1px solid #397886; background-color: #071f27; }"
          "QRadioButton::indicator:hover { border-color: #69ddd3; background-color: #0b3039; }"
          "QRadioButton::indicator:checked { background-color: #69ddd3; border: 4px solid #0b3039; }"
        : "QRadioButton { color: #23474c; spacing: 8px; font-weight: 500; }"
          "QRadioButton::indicator { width: 16px; height: 16px; border-radius: 9px; border: 1px solid #789fa0; background-color: #f2f8f7; }"
          "QRadioButton::indicator:hover { border-color: #087e79; background-color: #e0f2f0; }"
          "QRadioButton::indicator:checked { background-color: #087e79; border: 4px solid #dcecea; }";
    for (QRadioButton *selector : {ui->radioButton_useJoyStick,
                                   ui->gamepad_btn,
                                   ui->radioButton_useKeyBoard}) {
        selector->setStyleSheet(inputSelectorStyle);
        selector->update();
    }

    ui->btn_theme_toggle->setStyleSheet(dark
        ? "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #124653, stop:1 #0a303a); border: 1px solid #236574; border-radius: 9px; }"
          "QPushButton:hover { background-color: #176173; border-color: #74eee3; }"
          "QPushButton:pressed { background-color: #0b2a32; border-color: #20c7b8; }"
        : "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f3f9f8, stop:1 #dceae8); border: 1px solid #91b9b7; border-radius: 9px; }"
          "QPushButton:hover { background-color: #d7ece9; border-color: #159d93; }"
          "QPushButton:pressed { background-color: #c5dfdc; border-color: #0f8f87; }");

    ui->full_screen->setText(QStringLiteral("Полный экран"));
    ui->full_screen->setStyleSheet(dark
        ? "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #124653, stop:1 #0a303a); color: #d7f3f1; border: 1px solid #236574; border-radius: 9px; padding: 7px 14px; font-weight: 600; }"
          "QPushButton:hover { border-color: #74eee3; color: #ffffff; }"
          "QPushButton:checked { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #176173, stop:1 #0d414d); border-color: #74eee3; color: #8af5ec; }"
        : "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f4faf9, stop:1 #dcebe9); color: #16383d; border: 1px solid #91b9b7; border-radius: 9px; padding: 7px 14px; font-weight: 600; }"
          "QPushButton:hover { border-color: #159d93; color: #075e58; }"
          "QPushButton:checked { background-color: #ccebe7; border-color: #0f8f87; color: #064d49; }");

    ui->compass->setLightTheme(!dark);
    ui->attitudeIndicator->setLightTheme(!dark);

    if (QLabel *brandTitle = findChild<QLabel*>(QStringLiteral("brandTitle"))) {
        brandTitle->setText(dark
            ? QStringLiteral("<span style='font-size:16px; font-weight:700; letter-spacing:1px; color:#74eee3;'>HYDRONAUTICS</span><br><span style='font-size:10px; color:#7faaa9;'>UMAS CONTROL STATION</span>")
            : QStringLiteral("<span style='font-size:16px; font-weight:700; letter-spacing:1px; color:#087f77;'>HYDRONAUTICS</span><br><span style='font-size:10px; color:#557b7d;'>UMAS CONTROL STATION</span>"));
    }

    ui->btn_theme_toggle->setIcon(dark ? makeMoonIcon() : makeSunIcon());
    ensureInteractiveTextFits();
}

void MainWindow::setupButtonStyles(bool dark)
{
    const QString speedStyle = dark
        ? "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #124653, stop:1 #0a303a); border: 1px solid #236574; border-radius: 8px;"
          " color: #d7f3f1; min-height: 28px; font-size: 13px; }"
          "QPushButton:hover { background-color: #176173; border: 1px solid #74eee3; color: #ffffff; }"
          "QPushButton:pressed { background-color: #0b2a32; color: #8af5ec; }"
          "QPushButton:checked { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #74eee3, stop:1 #20b9ad); color: #031416;"
          " border: 1px solid #a7fff7; font-weight: 700; }"
        : "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f3f9f8, stop:1 #dceae8); border: 1px solid #91b9b7; border-radius: 8px;"
          " color: #16383d; min-height: 28px; font-size: 13px; }"
          "QPushButton:hover { background-color: #d7ece9; border: 1px solid #159d93; color: #075e58; }"
          "QPushButton:pressed { background-color: #c5dfdc; color: #064d49; }"
          "QPushButton:checked { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #62ddd2, stop:1 #19aaa0); color: #052f2c;"
          " border: 1px solid #07847c; font-weight: 700; }";
    for (auto *b : {ui->pushButton_speedFast, ui->pushButton_speedMedium, ui->pushButton_speedSlow})
        b->setStyleSheet(speedStyle);

    const QString modeStyle = dark
        ? "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #124653, stop:1 #0a303a); border: 1px solid #236574; border-radius: 8px;"
          " color: #d7f3f1; min-height: 24px; }"
          "QPushButton:hover { background-color: #176173; border: 1px solid #74eee3; color: #ffffff; }"
          "QPushButton:pressed { background-color: #0b2a32; color: #8af5ec; }"
          "QPushButton:checked { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #74eee3, stop:1 #20b9ad); color: #031416; border: 1px solid #a7fff7; font-weight: 700; }"
        : "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f3f9f8, stop:1 #dceae8); border: 1px solid #91b9b7; border-radius: 8px;"
          " color: #16383d; min-height: 24px; }"
          "QPushButton:hover { background-color: #d7ece9; border: 1px solid #159d93; color: #075e58; }"
          "QPushButton:pressed { background-color: #c5dfdc; color: #064d49; }"
          "QPushButton:checked { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #62ddd2, stop:1 #19aaa0); color: #052f2c; border: 1px solid #07847c; font-weight: 700; }";
    for (auto *b : {ui->pushButton_modeManual, ui->pushButton_modeAutomated, ui->pushButton_modeAutomatic,
                    ui->pushButton_modeAutomated_surge, ui->pushButton_modeAutomated_sway,
                    ui->pushButton_modeAutomated_depth, ui->pushButton_modeAutomated_yaw,
                    ui->pushButton_modeAutomated_pitch, ui->pushButton_modeAutomated_roll})
        b->setStyleSheet(modeStyle);
    for (auto *b : {ui->btn_light_off, ui->btn_light_mode1,
                ui->btn_light_mode2, ui->btn_light_mode3})
        b->setStyleSheet(speedStyle);

    if (dark) {
        ui->btn_manip_stop->setStyleSheet(
            "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #c93a52, stop:1 #861d31);"
            " color: #fff1f3; border: 1px solid #f05268; border-radius: 8px; font-weight: 700; }"
            "QPushButton:hover { background-color: #dc4760; border-color: #ff91a1; }"
            "QPushButton:pressed { background-color: #711426; color: #ffdce2; }");
    } else {
        ui->btn_manip_stop->setStyleSheet(
            "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #fff4f6, stop:1 #ffdce2);"
            " color: #8f1830; border: 1px solid #ef7185; border-radius: 8px; font-weight: 700; }"
            "QPushButton:hover { background-color: #ffcbd4; border-color: #d9435c; }"
            "QPushButton:pressed { background-color: #f4b9c3; color: #711426; }");
    }
}

// ============================================================
// Поворот (курс) — L1/R1
// ============================================================

void MainWindow::onLeftTriggerMoved(float v)
{
    const bool pressed = (v > TRIGGER_THRESHOLD);
    if (pressed == m_l1Pressed) return;
    m_l1Pressed = pressed;

    setTurnState(pressed ? "left" : "stop");
}

void MainWindow::onRightTriggerMoved(float v)
{
    const bool pressed = (v > TRIGGER_THRESHOLD);
    if (pressed == m_r1Pressed) return;
    m_r1Pressed = pressed;

    setTurnState(pressed ? "right" : "stop");
}

void MainWindow::setTurnState(const QString& state)
{
    rosBridge->setTurnState(state);
    displayText(QString("Поворот: %1").arg(state));
}

// ============================================================
// Манипулятор (клешня) — кнопки в UI
// ============================================================

void MainWindow::setGripState(const QString& state)
{
    rosBridge->setGripState(state);
    displayText(QString("Манипулятор: %1").arg(state));
}

MainWindow::~MainWindow()
{
    delete ui;
}
