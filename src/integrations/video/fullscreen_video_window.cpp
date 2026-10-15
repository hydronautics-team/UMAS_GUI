#include "fullscreen_video_window.h"
#include "attitudeindicator.h"
#include "battery_alert_overlay.h"
#include "battery_status_widget.h"
#include "compass.h"
#include "depth_gauge.h"
#include "video_player_widget.h"
#include <QCloseEvent>
#include <QDateTime>
#include <QDebug>
#include <QPalette>
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

    alarmLabel_ = new QLabel("СВЯЗЬ С РОБОТОМ ОК", fullscreenVideoLabel_);
    alarmLabel_->setAlignment(Qt::AlignCenter);
    alarmLabel_->setStyleSheet(
        "color: #dffffb; font-size: 17px; font-weight: 600; background-color: rgba(18, 102, 95, "
        "210); border: 1px solid #20c7b8; border-radius: 8px; padding: 5px 12px;");

    const QString dataPanelStyle = "color: #dffffb; font-size: 17px; font-weight: 600;"
                                   "background-color: rgba(3, 20, 25, 205);"
                                   "border: 1px solid rgba(93, 226, 215, 120);"
                                   "border-radius: 8px; padding: 7px 12px;";
    motionLabel_ = new QLabel("СКОРОСТЬ: N/A", fullscreenVideoLabel_);
    motionLabel_->setStyleSheet(dataPanelStyle);
    coreStatusLabel_ = new QLabel("CORE: НЕТ ДАННЫХ", fullscreenVideoLabel_);

    compass_ = new Compass(fullscreenVideoLabel_);
    attitudeIndicator_ = new AttitudeIndicator(fullscreenVideoLabel_);
    depthGauge_ = new DepthGauge(fullscreenVideoLabel_);
    batteryStatusWidget_ = new BatteryStatusWidget(fullscreenVideoLabel_);
    batteryAlertOverlay_ = new BatteryAlertOverlay(fullscreenVideoLabel_);
    attitudeIndicator_->setOverlayMode(true);
    orientationLabel_ = new QLabel(fullscreenVideoLabel_);
    orientationLabel_->setAlignment(Qt::AlignCenter);
    orientationLabel_->setStyleSheet(
        "color: white; font-size: 18px; font-weight: bold;"
        "background-color: rgba(5, 12, 18, 170); border-radius: 6px; padding: 4px;");

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
    const qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
    const bool coreFresh =
        pose.timestamp_ms > 0 && nowMs - static_cast<qint64>(pose.timestamp_ms) < 1000;
    const bool battery1Fresh =
        battery.timestamp_1_ms > 0 && nowMs - static_cast<qint64>(battery.timestamp_1_ms) < 3000;
    const bool battery2Fresh =
        battery.timestamp_2_ms > 0 && nowMs - static_cast<qint64>(battery.timestamp_2_ms) < 3000;

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
    batteryAlertOverlay_->setLevel(criticalBattery
                                       ? BatteryAlertOverlay::Level::Critical
                                       : (warningBattery ? BatteryAlertOverlay::Level::Warning
                                                         : BatteryAlertOverlay::Level::None));

    coreStatusLabel_->setText(coreFresh ? "● STINGRAY CORE" : "● CORE НЕТ ДАННЫХ");
    coreStatusLabel_->setStyleSheet(
        coreFresh
            ? "color: #8af5ec; font-size: 15px; font-weight: 700; background-color: rgba(8, 57, "
              "60, 210); border: 1px solid #20c7b8; border-radius: 8px; padding: 6px 11px;"
            : "color: #ffe8ec; font-size: 15px; font-weight: 700; background-color: rgba(120, 22, "
              "39, 220); border: 1px solid #f05268; border-radius: 8px; padding: 6px 11px;");

    const bool lowBattery = (battery1Fresh && battery.voltage_1 < 13.6f) ||
                            (battery2Fresh && battery.voltage_2 < 13.6f);
    if (!coreFresh)
    {
        alarmLabel_->setText("ВНИМАНИЕ: НЕТ ДАННЫХ STINGRAY CORE");
        alarmLabel_->setStyleSheet(
            "color: #ffe8ec; font-size: 17px; font-weight: 600; background-color: rgba(163, 38, "
            "61, 225); border: 1px solid #f05268; border-radius: 8px; padding: 5px 12px;");
    }
    else if (depth > 10.0f)
    {
        alarmLabel_->setText("ВНИМАНИЕ: ПРЕВЫШЕНИЕ ГЛУБИНЫ!");
        alarmLabel_->setStyleSheet(
            "color: #ffe8ec; font-size: 17px; font-weight: 600; background-color: rgba(163, 38, "
            "61, 225); border: 1px solid #f05268; border-radius: 8px; padding: 5px 12px;");
    }
    else if (lowBattery)
    {
        alarmLabel_->setText("ВНИМАНИЕ: НИЗКОЕ НАПРЯЖЕНИЕ АКБ");
        alarmLabel_->setStyleSheet(
            "color: #fff3df; font-size: 17px; font-weight: 600; background-color: rgba(135, 77, "
            "18, 225); border: 1px solid #efb766; border-radius: 8px; padding: 5px 12px;");
    }
    else
    {
        alarmLabel_->setText("СИСТЕМА СТАБИЛЬНА");
        alarmLabel_->setStyleSheet(
            "color: #dffffb; font-size: 17px; font-weight: 600; background-color: rgba(18, 102, "
            "95, 210); border: 1px solid #20c7b8; border-radius: 8px; padding: 5px 12px;");
    }
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
        motionLabel_->setGeometry(30, 30, 310, 48);
    if (batteryStatusWidget_)
        batteryStatusWidget_->setGeometry(w - 280, h - 146, 250, 110);
    if (batteryAlertOverlay_)
        batteryAlertOverlay_->setGeometry(0, 0, w, h);
    if (coreStatusLabel_)
        coreStatusLabel_->setGeometry(30, h - 108, 260, 38);
    if (alarmLabel_)
        alarmLabel_->setGeometry((w - 600) / 2, h - 62, 600, 40);

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
    if (batteryAlertOverlay_)
        batteryAlertOverlay_->raise();
    if (alarmLabel_)
        alarmLabel_->raise();
    if (motionLabel_)
        motionLabel_->raise();
    if (batteryStatusWidget_)
        batteryStatusWidget_->raise();
    if (coreStatusLabel_)
        coreStatusLabel_->raise();
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
    if (auto *player = qobject_cast<VideoPlayerWidget *>(videoPlayer_))
        player->setFullscreenPresentation(false);
    emit windowClosed();
    QWidget::closeEvent(event);
}
