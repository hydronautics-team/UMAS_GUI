#include "fullscreen_video_window.h"
#include "attitudeindicator.h"
#include "battery_status_widget.h"
#include "compass.h"
#include "depth_gauge.h"
#include "hud_alert_overlay.h"
#include "hud_notification_stack.h"
#include "video_player_widget.h"
#include <QCloseEvent>
#include <QDateTime>
#include <QDebug>
#include <QFrame>
#include <QPalette>
#include <QPushButton>
#include <QResizeEvent>
#include <cmath>

FullscreenVideoWindow::FullscreenVideoWindow(UVState *uvState, QWidget *parent)
    : QWidget(parent), uvState_(uvState), videoPlayer_(nullptr)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setStyleSheet("background-color: black;");

    // СЛОЙ 1: Создаем единый главный холст под видеоряд
    fullscreenVideoLabel_ = new QLabel(this);
    fullscreenVideoLabel_->setAlignment(Qt::AlignCenter);
    fullscreenVideoLabel_->setStyleSheet("background-color: black;");

    const QString dataPanelStyle = "color: #dffffb; font-size: 17px; font-weight: 600;"
                                   "background-color: rgba(3, 20, 25, 205);"
                                   "border: 1px solid rgba(93, 226, 215, 120);"
                                   "border-radius: 8px; padding: 7px 12px;";
    motionLabel_ = new QLabel("СКОРОСТЬ: N/A", fullscreenVideoLabel_);
    motionLabel_->setStyleSheet(dataPanelStyle);

    compass_ = new Compass(fullscreenVideoLabel_);
    attitudeIndicator_ = new AttitudeIndicator(fullscreenVideoLabel_);
    depthGauge_ = new DepthGauge(fullscreenVideoLabel_);
    batteryStatusWidget_ = new BatteryStatusWidget(fullscreenVideoLabel_);
    alertOverlay_ = new HudAlertOverlay(fullscreenVideoLabel_);
    notificationStack_ = new HudNotificationStack(fullscreenVideoLabel_);
    attitudeIndicator_->setOverlayMode(true);
    orientationLabel_ = new QLabel(fullscreenVideoLabel_);
    orientationLabel_->setAlignment(Qt::AlignCenter);
    orientationLabel_->setStyleSheet(
        "color: white; font-size: 18px; font-weight: bold;"
        "background-color: rgba(5, 12, 18, 170); border-radius: 6px; padding: 4px;");

    secondaryCameraFrame_ = new QFrame(fullscreenVideoLabel_);
    secondaryCameraFrame_->setObjectName("secondaryCameraFrame");
    secondaryCameraFrame_->setStyleSheet(
        "QFrame#secondaryCameraFrame { background-color: rgba(3, 20, 25, 235);"
        "border: 2px solid rgba(32, 199, 184, 210); border-radius: 10px; }");
    secondaryCameraTitle_ = new QLabel("КАМЕРА 2  ·  UDP 5001", secondaryCameraFrame_);
    secondaryCameraTitle_->setAlignment(Qt::AlignCenter);
    secondaryCameraTitle_->setStyleSheet("color: #dffffb; font-size: 13px; font-weight: 700; "
                                         "background: transparent; border: none;");
    secondaryVideoPlayer_ = new VideoPlayerWidget(secondaryCameraFrame_);
    secondaryVideoPlayer_->setFullscreenPresentation(true);
    secondaryCameraFrame_->hide();

    secondaryCameraButton_ = new QPushButton("КАМЕРА 2", fullscreenVideoLabel_);
    secondaryCameraButton_->setCheckable(true);
    secondaryCameraButton_->setFocusPolicy(Qt::NoFocus);
    secondaryCameraButton_->setCursor(Qt::PointingHandCursor);
    secondaryCameraButton_->setStyleSheet(
        "QPushButton { color: #dffffb; font-size: 13px; font-weight: 700;"
        "background-color: rgba(3, 20, 25, 220); border: 1px solid rgba(93, 226, 215, 150);"
        "border-radius: 8px; padding: 7px 14px; }"
        "QPushButton:hover { background-color: rgba(13, 75, 76, 230); border-color: #5de2d7; }"
        "QPushButton:checked { color: #061d20; background-color: #5de2d7; border-color: #9ffff7; "
        "}");
    connect(secondaryCameraButton_, &QPushButton::toggled, this,
            &FullscreenVideoWindow::setSecondaryCameraEnabled);

    hudTimer_ = new QTimer(this);
    connect(hudTimer_, &QTimer::timeout, this, &FullscreenVideoWindow::updateHudData);
    hudTimer_->start(100);

    qDebug() << "Полноэкранное окно HUD создано";
}

FullscreenVideoWindow::~FullscreenVideoWindow()
{
    hudTimer_->stop();
    qDebug() << "Полноэкранное окно HUD уничтожено";
}

void FullscreenVideoWindow::setVideoWidget(QWidget *widget)
{
    if (!widget)
        return;

    videoPlayer_ = widget;
    videoPlayer_->setParent(fullscreenVideoLabel_);
    if (auto *player = qobject_cast<VideoPlayerWidget *>(videoPlayer_))
        player->setFullscreenPresentation(true);

    if (fullscreenVideoLabel_)
    {
        videoPlayer_->setGeometry(0, 0, fullscreenVideoLabel_->width(),
                                  fullscreenVideoLabel_->height());
        videoPlayer_->show();
        videoPlayer_->raise();
    }
}

void FullscreenVideoWindow::drawFrame(const QPixmap &pixmap)
{
    if (fullscreenVideoLabel_ && !pixmap.isNull())
    {
        fullscreenVideoLabel_->setPixmap(pixmap.scaled(
            fullscreenVideoLabel_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void FullscreenVideoWindow::updateHudData()
{
    if (!uvState_)
        return;

    const UVState::Pose pose = uvState_->pose();
    const UVState::BatteryData battery = uvState_->battery();
    const UVState::ImuData imu = uvState_->imu();
    const UVState::SafetyData safety = uvState_->safety();
    const qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
    const bool coreFresh =
        pose.timestamp_ms > 0 && nowMs - static_cast<qint64>(pose.timestamp_ms) < 1000;
    const bool battery1Fresh =
        battery.timestamp_1_ms > 0 && nowMs - static_cast<qint64>(battery.timestamp_1_ms) < 3000;
    const bool battery2Fresh =
        battery.timestamp_2_ms > 0 && nowMs - static_cast<qint64>(battery.timestamp_2_ms) < 3000;
    const bool leakFresh = safety.leak_timestamp_ms > 0 &&
                           nowMs - static_cast<qint64>(safety.leak_timestamp_ms) < 3000;
    const bool killSwitchFresh =
        safety.kill_switch_timestamp_ms > 0 &&
        nowMs - static_cast<qint64>(safety.kill_switch_timestamp_ms) < 3000;
    const bool leakDetected = leakFresh && safety.leak_detected;
    const bool killSwitchEngaged = killSwitchFresh && safety.kill_switch_engaged;
    const bool safetyUnavailable = coreFresh && (!leakFresh || !killSwitchFresh);

    const float depth = static_cast<float>(-pose.z);
    const float yaw = imu.yaw;

    if (coreFresh)
    {
        const double horizontalSpeed = std::hypot(pose.velocity_x, pose.velocity_y);
        motionLabel_->setText(QStringLiteral("СКОРОСТЬ: %1 м/с").arg(horizontalSpeed, 0, 'f', 2));
        orientationLabel_->setText(
            QStringLiteral("Курс %1°   Дифферент %2°   Крен %3°")
                .arg(static_cast<int>(qRound(yaw + 360.0f)) % 360, 3, 10, QLatin1Char('0'))
                .arg(imu.pitch, 0, 'f', 1)
                .arg(imu.roll, 0, 'f', 1));
        compass_->setYaw(yaw);
        attitudeIndicator_->setAttitude(imu.pitch, imu.roll);
        depthGauge_->setTelemetry(depth, static_cast<float>(pose.velocity_z));
    }
    else
    {
        motionLabel_->setText("СКОРОСТЬ: N/A");
        orientationLabel_->setText("НЕТ ДАННЫХ ОРИЕНТАЦИИ");
        depthGauge_->setTelemetry(0.0f, 0.0f, false);
    }

    batteryStatusWidget_->setBatteries(battery.percentage_1, battery1Fresh, battery.percentage_2,
                                       battery2Fresh);
    const bool criticalBattery = (battery1Fresh && battery.percentage_1 <= 15.0f) ||
                                 (battery2Fresh && battery.percentage_2 <= 15.0f);
    const bool warningBattery = (battery1Fresh && battery.percentage_1 <= 25.0f) ||
                                (battery2Fresh && battery.percentage_2 <= 25.0f);
    const bool depthExceeded = coreFresh && depth > 10.0f;
    const bool lowVoltage = (battery1Fresh && battery.voltage_1 < 13.6f) ||
                            (battery2Fresh && battery.voltage_2 < 13.6f);

    alertOverlay_->setAlert("connection", HudAlertOverlay::Level::Critical, !coreFresh);
    alertOverlay_->setAlert("depth", HudAlertOverlay::Level::Critical, depthExceeded);
    alertOverlay_->setAlert("battery_critical", HudAlertOverlay::Level::Critical, criticalBattery);
    alertOverlay_->setAlert("battery_warning", HudAlertOverlay::Level::Warning,
                            warningBattery && !criticalBattery);
    alertOverlay_->setAlert("battery_voltage", HudAlertOverlay::Level::Warning, lowVoltage);
    alertOverlay_->setAlert("leak", HudAlertOverlay::Level::Critical, leakDetected);
    alertOverlay_->setAlert("kill_switch", HudAlertOverlay::Level::Critical, killSwitchEngaged);
    alertOverlay_->setAlert("safety_unavailable", HudAlertOverlay::Level::Warning,
                            safetyUnavailable);

    notificationStack_->setCondition("connection", !coreFresh, "НЕТ СВЯЗИ СО STINGRAY CORE",
                                     HudNotificationStack::Severity::Error);
    notificationStack_->setCondition(
        "depth", depthExceeded, QStringLiteral("ПРЕВЫШЕНИЕ ГЛУБИНЫ: %1 м").arg(depth, 0, 'f', 1),
        HudNotificationStack::Severity::Error);
    notificationStack_->setCondition("battery_critical", criticalBattery,
                                     "КРИТИЧЕСКИЙ ЗАРЯД АККУМУЛЯТОРА",
                                     HudNotificationStack::Severity::Error);
    notificationStack_->setCondition("battery_warning", warningBattery && !criticalBattery,
                                     "НИЗКИЙ ЗАРЯД АККУМУЛЯТОРА",
                                     HudNotificationStack::Severity::Warning);
    notificationStack_->setCondition("battery_voltage", lowVoltage,
                                     "НИЗКОЕ НАПРЯЖЕНИЕ АККУМУЛЯТОРА",
                                     HudNotificationStack::Severity::Warning);
    notificationStack_->setCondition("leak", leakDetected, "ОБНАРУЖЕНА ПРОТЕЧКА",
                                     HudNotificationStack::Severity::Error);
    notificationStack_->setCondition("kill_switch", killSwitchEngaged,
                                     "KILL SWITCH АКТИВЕН · УПРАВЛЕНИЕ ЗАБЛОКИРОВАНО",
                                     HudNotificationStack::Severity::Error);
    notificationStack_->setCondition("safety_unavailable", safetyUnavailable,
                                     "НЕТ СВЕЖИХ ДАННЫХ ДАТЧИКОВ БЕЗОПАСНОСТИ",
                                     HudNotificationStack::Severity::Warning);

    const bool stable = coreFresh && !depthExceeded && !criticalBattery && !warningBattery &&
                        !lowVoltage && !leakDetected && !killSwitchEngaged && !safetyUnavailable;
    if (stable && !wasStable_)
        notificationStack_->showInfo("stable", "СИСТЕМА СТАБИЛЬНА", 4000);
    wasStable_ = stable;
}

void FullscreenVideoWindow::setSecondaryCameraEnabled(bool enabled)
{
    if (enabled)
    {
        secondaryVideoPlayer_->startStream("5001");
        secondaryCameraFrame_->show();
        secondaryCameraButton_->setText("СКРЫТЬ");
    }
    else
    {
        secondaryVideoPlayer_->stopStream();
        secondaryCameraFrame_->hide();
        secondaryCameraButton_->setText("КАМЕРА 2");
    }

    secondaryCameraFrame_->raise();
    secondaryCameraButton_->raise();
    const int notificationBottom = enabled ? secondaryCameraFrame_->y() - 16 : height() - 138;
    notificationStack_->setGeometry(30, 30, qBound(300, width() / 4, 430),
                                    qMax(120, notificationBottom - 30));
}

void FullscreenVideoWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        close();
    }
    else
    {
        QWidget::keyPressEvent(event);
    }
}

void FullscreenVideoWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    int w = event->size().width();
    int h = event->size().height();

    // Растягиваем холст видео на 100% экрана
    if (fullscreenVideoLabel_)
    {
        fullscreenVideoLabel_->setGeometry(0, 0, w, h);

        // Растягиваем видео-плеер внутри холста
        if (videoPlayer_)
        {
            videoPlayer_->setGeometry(0, 0, w, h);
            videoPlayer_->raise();
        }
    }

    // Выставляем точные координаты для надписей телеметрии
    if (motionLabel_)
        motionLabel_->setGeometry(w - 606, h - 84, 310, 48);
    if (batteryStatusWidget_)
        batteryStatusWidget_->setGeometry(w - 280, h - 146, 250, 110);
    if (alertOverlay_)
        alertOverlay_->setGeometry(0, 0, w, h);

    const int secondaryWidth = qBound(300, w / 4, 420);
    const int secondaryHeight = secondaryWidth * 9 / 16 + 34;
    const int secondaryTop = h - secondaryHeight - 94;
    if (secondaryCameraFrame_)
    {
        secondaryCameraFrame_->setGeometry(30, secondaryTop, secondaryWidth, secondaryHeight);
        secondaryCameraTitle_->setGeometry(10, 5, secondaryWidth - 20, 24);
        secondaryVideoPlayer_->setGeometry(4, 32, secondaryWidth - 8, secondaryHeight - 36);
    }
    if (secondaryCameraButton_)
        secondaryCameraButton_->setGeometry(30, h - 60, 110, 24);
    if (notificationStack_)
    {
        const int notificationBottom =
            secondaryCameraFrame_->isVisible() ? secondaryTop - 16 : h - 138;
        notificationStack_->setGeometry(30, 30, qBound(300, w / 4, 430),
                                        qMax(120, notificationBottom - 30));
    }

    const int instrumentSize = qBound(140, qMin(w, h) / 5, 240);
    const int margin = 30;
    if (compass_)
        compass_->setGeometry(w - instrumentSize - margin, margin, instrumentSize, instrumentSize);
    if (depthGauge_)
    {
        const int gaugeWidth = qBound(150, w / 10, 190);
        const int gaugeTop = margin + instrumentSize + 16;
        const int gaugeBottom = h - 138;
        depthGauge_->setGeometry(w - gaugeWidth - margin, gaugeTop, gaugeWidth,
                                 qMax(220, gaugeBottom - gaugeTop));
    }
    if (attitudeIndicator_)
    {
        const int overlayWidth = qBound(440, static_cast<int>(w * 0.48), 820);
        const int overlayHeight = qBound(300, static_cast<int>(h * 0.52), 580);
        attitudeIndicator_->setGeometry((w - overlayWidth) / 2, (h - overlayHeight) / 2,
                                        overlayWidth, overlayHeight);
    }
    if (orientationLabel_)
        orientationLabel_->setGeometry((w - 560) / 2, margin, 560, 42);

    // Выводим надписи на передний план
    if (alertOverlay_)
        alertOverlay_->raise();
    if (motionLabel_)
        motionLabel_->raise();
    if (batteryStatusWidget_)
        batteryStatusWidget_->raise();
    if (notificationStack_)
        notificationStack_->raise();
    if (secondaryCameraFrame_)
        secondaryCameraFrame_->raise();
    if (secondaryCameraButton_)
        secondaryCameraButton_->raise();
    if (compass_)
        compass_->raise();
    if (depthGauge_)
        depthGauge_->raise();
    if (attitudeIndicator_)
        attitudeIndicator_->raise();
    if (orientationLabel_)
        orientationLabel_->raise();
}

void FullscreenVideoWindow::closeEvent(QCloseEvent *event)
{
    secondaryVideoPlayer_->stopStream();
    if (auto *player = qobject_cast<VideoPlayerWidget *>(videoPlayer_))
        player->setFullscreenPresentation(false);
    emit windowClosed();
    QWidget::closeEvent(event);
}
