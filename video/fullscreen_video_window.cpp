#include "fullscreen_video_window.h"
#include "attitudeindicator.h"
#include "compass.h"
#include <QPalette>
#include <QDebug>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QDateTime>
#include <cmath>

FullscreenVideoWindow::FullscreenVideoWindow(UVState* uvState, QWidget* parent)
    : QWidget(parent)
    , uvState_(uvState)
    , videoPlayer_(nullptr)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setStyleSheet("background-color: black;");

    // СЛОЙ 1: Создаем единый главный холст под видеоряд
    fullscreenVideoLabel_ = new QLabel(this);
    fullscreenVideoLabel_->setAlignment(Qt::AlignCenter);
    fullscreenVideoLabel_->setStyleSheet("background-color: black;");

    // СЛОЙ 2: Привязываем красные надписи НАПРЯМУЮ к видео-холсту как дочерние элементы
    depthLabel_ = new QLabel("ГЛУБИНА: 0.0 м", fullscreenVideoLabel_);
    depthLabel_->setStyleSheet("color: #dffffb; font-size: 22px; font-weight: 600; background-color: rgba(3, 20, 25, 205); border: 1px solid rgba(93, 226, 215, 120); border-radius: 8px; padding: 5px 12px;");

    yawLabel_ = new QLabel("КУРС: 0°", fullscreenVideoLabel_);
    yawLabel_->setStyleSheet("color: #74eee3; font-size: 22px; font-weight: 600; background-color: rgba(3, 20, 25, 205); border: 1px solid rgba(93, 226, 215, 120); border-radius: 8px; padding: 5px 12px;");

    alarmLabel_ = new QLabel("СВЯЗЬ С РОБОТОМ ОК", fullscreenVideoLabel_);
    alarmLabel_->setStyleSheet("color: #dffffb; font-size: 17px; font-weight: 600; background-color: rgba(18, 102, 95, 210); border: 1px solid #20c7b8; border-radius: 8px; padding: 5px 12px;");

    const QString dataPanelStyle =
        "color: #dffffb; font-size: 17px; font-weight: 600;"
        "background-color: rgba(3, 20, 25, 205);"
        "border: 1px solid rgba(93, 226, 215, 120);"
        "border-radius: 8px; padding: 7px 12px;";
    motionLabel_ = new QLabel("СКОРОСТЬ: N/A", fullscreenVideoLabel_);
    motionLabel_->setStyleSheet(dataPanelStyle);
    batteryLabel_ = new QLabel("АКБ: N/A", fullscreenVideoLabel_);
    batteryLabel_->setStyleSheet(dataPanelStyle);
    coreStatusLabel_ = new QLabel("CORE: НЕТ ДАННЫХ", fullscreenVideoLabel_);

    compass_ = new Compass(fullscreenVideoLabel_);
    attitudeIndicator_ = new AttitudeIndicator(fullscreenVideoLabel_);
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

void FullscreenVideoWindow::setVideoWidget(QWidget* widget)
{
    if (!widget) return;

    videoPlayer_ = widget;
    videoPlayer_->setParent(fullscreenVideoLabel_);

    if (fullscreenVideoLabel_) {
        videoPlayer_->setGeometry(0, 0, fullscreenVideoLabel_->width(), fullscreenVideoLabel_->height());
        videoPlayer_->show();
        videoPlayer_->raise();
    }
}

void FullscreenVideoWindow::drawFrame(const QPixmap& pixmap)
{
    if (fullscreenVideoLabel_ && !pixmap.isNull()) {
        fullscreenVideoLabel_->setPixmap(pixmap.scaled(
            fullscreenVideoLabel_->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        ));
    }
}

void FullscreenVideoWindow::updateHudData()
{
    if (!uvState_) return;

    const UVState::Pose pose = uvState_->pose();
    const UVState::BatteryData battery = uvState_->battery();
    const UVState::ImuData imu = uvState_->imu();
    const qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
    const bool coreFresh = pose.timestamp_ms > 0
        && nowMs - static_cast<qint64>(pose.timestamp_ms) < 1000;
    const bool battery1Fresh = battery.timestamp_1_ms > 0
        && nowMs - static_cast<qint64>(battery.timestamp_1_ms) < 3000;
    const bool battery2Fresh = battery.timestamp_2_ms > 0
        && nowMs - static_cast<qint64>(battery.timestamp_2_ms) < 3000;

    const float depth = static_cast<float>(-pose.z);
    const float yaw = imu.yaw;

    if (coreFresh) {
        const double horizontalSpeed = std::hypot(pose.velocity_x, pose.velocity_y);
        depthLabel_->setText(QStringLiteral("ГЛУБИНА: %1 м").arg(depth, 0, 'f', 1));
        yawLabel_->setText(QStringLiteral("КУРС: %1°")
            .arg(static_cast<int>(qRound(yaw + 360.0f)) % 360, 3, 10, QLatin1Char('0')));
        motionLabel_->setText(QStringLiteral("СКОРОСТЬ: %1 м/с\nВЕРТИКАЛЬНАЯ: %2 м/с")
            .arg(horizontalSpeed, 0, 'f', 2)
            .arg(pose.velocity_z, 0, 'f', 2));
        orientationLabel_->setText(QStringLiteral("Курс %1°   Дифферент %2°   Крен %3°")
            .arg(static_cast<int>(qRound(yaw + 360.0f)) % 360, 3, 10, QLatin1Char('0'))
            .arg(imu.pitch, 0, 'f', 1)
            .arg(imu.roll, 0, 'f', 1));
        compass_->setYaw(yaw);
        attitudeIndicator_->setAttitude(imu.pitch, imu.roll);
    } else {
        depthLabel_->setText("ГЛУБИНА: N/A");
        yawLabel_->setText("КУРС: N/A");
        motionLabel_->setText("СКОРОСТЬ: N/A\nВЕРТИКАЛЬНАЯ: N/A");
        orientationLabel_->setText("НЕТ ДАННЫХ ОРИЕНТАЦИИ");
    }

    const QString battery1Text = battery1Fresh
        ? QStringLiteral("АКБ 1  %1 В  %2%").arg(battery.voltage_1, 0, 'f', 1)
              .arg(battery.percentage_1, 0, 'f', 0)
        : QStringLiteral("АКБ 1  N/A");
    const QString battery2Text = battery2Fresh
        ? QStringLiteral("АКБ 2  %1 В  %2%").arg(battery.voltage_2, 0, 'f', 1)
              .arg(battery.percentage_2, 0, 'f', 0)
        : QStringLiteral("АКБ 2  N/A");
    batteryLabel_->setText(battery1Text + "\n" + battery2Text);

    coreStatusLabel_->setText(coreFresh ? "● STINGRAY CORE" : "● CORE НЕТ ДАННЫХ");
    coreStatusLabel_->setStyleSheet(coreFresh
        ? "color: #8af5ec; font-size: 15px; font-weight: 700; background-color: rgba(8, 57, 60, 210); border: 1px solid #20c7b8; border-radius: 8px; padding: 6px 11px;"
        : "color: #ffe8ec; font-size: 15px; font-weight: 700; background-color: rgba(120, 22, 39, 220); border: 1px solid #f05268; border-radius: 8px; padding: 6px 11px;");

    const bool lowBattery = (battery1Fresh && battery.voltage_1 < 13.6f)
        || (battery2Fresh && battery.voltage_2 < 13.6f);
    if (!coreFresh) {
        alarmLabel_->setText("ВНИМАНИЕ: НЕТ ДАННЫХ STINGRAY CORE");
        alarmLabel_->setStyleSheet("color: #ffe8ec; font-size: 17px; font-weight: 600; background-color: rgba(163, 38, 61, 225); border: 1px solid #f05268; border-radius: 8px; padding: 5px 12px;");
    } else if (depth > 10.0f) {
        alarmLabel_->setText("ВНИМАНИЕ: ПРЕВЫШЕНИЕ ГЛУБИНЫ!");
        alarmLabel_->setStyleSheet("color: #ffe8ec; font-size: 17px; font-weight: 600; background-color: rgba(163, 38, 61, 225); border: 1px solid #f05268; border-radius: 8px; padding: 5px 12px;");
    } else if (lowBattery) {
        alarmLabel_->setText("ВНИМАНИЕ: НИЗКОЕ НАПРЯЖЕНИЕ АКБ");
        alarmLabel_->setStyleSheet("color: #fff3df; font-size: 17px; font-weight: 600; background-color: rgba(135, 77, 18, 225); border: 1px solid #efb766; border-radius: 8px; padding: 5px 12px;");
    } else {
        alarmLabel_->setText("СИСТЕМА СТАБИЛЬНА");
        alarmLabel_->setStyleSheet("color: #dffffb; font-size: 17px; font-weight: 600; background-color: rgba(18, 102, 95, 210); border: 1px solid #20c7b8; border-radius: 8px; padding: 5px 12px;");
    }
}

void FullscreenVideoWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void FullscreenVideoWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    int w = event->size().width();
    int h = event->size().height();

    // Растягиваем холст видео на 100% экрана
    if (fullscreenVideoLabel_) {
        fullscreenVideoLabel_->setGeometry(0, 0, w, h);

        // Растягиваем видео-плеер внутри холста
        if (videoPlayer_) {
            videoPlayer_->setGeometry(0, 0, w, h);
            videoPlayer_->raise();
        }
    }

    // Выставляем точные координаты для надписей телеметрии
    if (depthLabel_) depthLabel_->setGeometry(30, 30, 400, 40);
    if (yawLabel_)   yawLabel_->setGeometry(30, 80, 400, 40);
    if (motionLabel_) motionLabel_->setGeometry(30, 130, 310, 76);
    if (batteryLabel_) batteryLabel_->setGeometry(w - 310, h - 120, 280, 88);
    if (coreStatusLabel_) coreStatusLabel_->setGeometry(30, h - 108, 260, 38);
    if (alarmLabel_) alarmLabel_->setGeometry((w - 600) / 2, h - 62, 600, 40);

    const int instrumentSize = qBound(140, qMin(w, h) / 5, 240);
    const int margin = 30;
    if (compass_)
        compass_->setGeometry(w - instrumentSize - margin, margin, instrumentSize, instrumentSize);
    if (attitudeIndicator_) {
        const int overlayWidth = qBound(440, static_cast<int>(w * 0.48), 820);
        const int overlayHeight = qBound(300, static_cast<int>(h * 0.52), 580);
        attitudeIndicator_->setGeometry((w - overlayWidth) / 2,
                                        (h - overlayHeight) / 2,
                                        overlayWidth, overlayHeight);
    }
    if (orientationLabel_)
        orientationLabel_->setGeometry((w - 560) / 2, margin, 560, 42);

    // Выводим надписи на передний план
    if (depthLabel_) depthLabel_->raise();
    if (yawLabel_)   yawLabel_->raise();
    if (alarmLabel_) alarmLabel_->raise();
    if (motionLabel_) motionLabel_->raise();
    if (batteryLabel_) batteryLabel_->raise();
    if (coreStatusLabel_) coreStatusLabel_->raise();
    if (compass_) compass_->raise();
    if (attitudeIndicator_) attitudeIndicator_->raise();
    if (orientationLabel_) orientationLabel_->raise();
}

void FullscreenVideoWindow::closeEvent(QCloseEvent* event)
{
    emit windowClosed();
    QWidget::closeEvent(event);
}
