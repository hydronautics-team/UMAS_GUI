#include "fullscreen_video_window.h"
#include "attitudeindicator.h"
#include "compass.h"
#include <QPalette>
#include <QDebug>
#include <QCloseEvent>
#include <QResizeEvent>

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

    compass_ = new Compass(fullscreenVideoLabel_);
    attitudeIndicator_ = new AttitudeIndicator(fullscreenVideoLabel_);
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

    float depth = static_cast<float>(uvState_->pose().z);
    const UVState::ImuData imu = uvState_->imu();
    float yaw = imu.yaw;

    depthLabel_->setText(QString("ГЛУБИНА: %1 м").arg(QString::number(depth, 'f', 1)));
    yawLabel_->setText(QString("КУРС: %1°").arg(static_cast<int>(qRound(yaw + 360.0f)) % 360, 3, 10, QLatin1Char('0')));
    orientationLabel_->setText(QStringLiteral("Курс %1°   Дифферент %2°   Крен %3°")
                                   .arg(static_cast<int>(qRound(yaw + 360.0f)) % 360, 3, 10, QLatin1Char('0'))
                                   .arg(imu.pitch, 0, 'f', 1)
                                   .arg(imu.roll, 0, 'f', 1));
    compass_->setYaw(yaw);
    attitudeIndicator_->setAttitude(imu.pitch, imu.roll);

    if (depth > 10.0f) {
        alarmLabel_->setText("ВНИМАНИЕ: ПРЕВЫШЕНИЕ ГЛУБИНЫ!");
        alarmLabel_->setStyleSheet("color: #ffd9dc; font-size: 17px; font-weight: 600; background-color: rgba(110, 48, 56, 220); border: 1px solid #d86b77; border-radius: 8px; padding: 5px 12px;");
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
    if (alarmLabel_) alarmLabel_->setGeometry(30, h - 60, 600, 40);

    const int instrumentSize = qBound(140, qMin(w, h) / 5, 240);
    const int margin = 30;
    if (compass_)
        compass_->setGeometry(w - instrumentSize - margin, margin, instrumentSize, instrumentSize);
    if (attitudeIndicator_)
        attitudeIndicator_->setGeometry(w - instrumentSize - margin,
                                        margin + instrumentSize + 15,
                                        instrumentSize, instrumentSize);
    if (orientationLabel_)
        orientationLabel_->setGeometry((w - 560) / 2, margin, 560, 42);

    // Выводим надписи на передний план
    if (depthLabel_) depthLabel_->raise();
    if (yawLabel_)   yawLabel_->raise();
    if (alarmLabel_) alarmLabel_->raise();
    if (compass_) compass_->raise();
    if (attitudeIndicator_) attitudeIndicator_->raise();
    if (orientationLabel_) orientationLabel_->raise();
}

void FullscreenVideoWindow::closeEvent(QCloseEvent* event)
{
    emit windowClosed();
    QWidget::closeEvent(event);
}
