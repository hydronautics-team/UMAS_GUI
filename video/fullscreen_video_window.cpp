#include "fullscreen_video_window.h"
#include <QPalette>
#include <QDebug>
#include <QResizeEvent>

FullscreenVideoWindow::FullscreenVideoWindow(UVState* uvState, QWidget* parent)
    : QWidget(parent)
    , uvState_(uvState)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setStyleSheet("background-color: black;");

    // СЛОЙ 1: Создаем единый главный холст под видеоряд
    fullscreenVideoLabel_ = new QLabel(this);
    fullscreenVideoLabel_->setAlignment(Qt::AlignCenter);
    fullscreenVideoLabel_->setStyleSheet("background-color: black;");

    // СЛОЙ 2: Привязываем красные надписи НАПРЯМУЮ к видео-холсту как дочерние элементы
    depthLabel_ = new QLabel("ГЛУБИНА: 0.0 м", fullscreenVideoLabel_);
    depthLabel_->setStyleSheet("color: red; font-size: 24px; font-weight: bold; background: transparent;");

    yawLabel_ = new QLabel("КУРС: 0°", fullscreenVideoLabel_);
    yawLabel_->setStyleSheet("color: red; font-size: 24px; font-weight: bold; background: transparent;");

    alarmLabel_ = new QLabel("СВЯЗЬ С РОБОТОМ ОК", fullscreenVideoLabel_);
    alarmLabel_->setStyleSheet("color: green; font-size: 18px; font-weight: bold; background: transparent;");

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

void FullscreenVideoWindow::drawFrame(const QPixmap& pixmap)
{
    if (fullscreenVideoLabel_ && !pixmap.isNull()) {
        // Отрисовываем пиксели в холст сглаженным масштабированием
        fullscreenVideoLabel_->setPixmap(pixmap.scaled(fullscreenVideoLabel_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void FullscreenVideoWindow::updateHudData()
{
    if (!uvState_) return;

    float depth = static_cast<float>(uvState_->pose().z);
    float yaw = uvState_->imu().yaw;

    depthLabel_->setText(QString("ГЛУБИНА: %1 м").arg(QString::number(depth, 'f', 1)));
    yawLabel_->setText(QString("КУРС: %1°").arg(QString::number(static_cast<int>(yaw))));

    if (depth > 10.0f) {
        alarmLabel_->setText("ВНИМАНИЕ: ПРЕВЫШЕНИЕ ГЛУБИНЫ!");
        alarmLabel_->setStyleSheet("color: red; font-size: 18px; font-weight: bold; background: transparent;");
    } else {
        alarmLabel_->setText("СИСТЕМА СТАБИЛЬНА");
        alarmLabel_->setStyleSheet("color: #00ff00; font-size: 18px; font-weight: bold; background: transparent;");
    }
}

void FullscreenVideoWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        emit windowClosed(); 
        this->close();       
    } else {
        QWidget::keyPressEvent(event);
    }
}

// ЖЕСТКИЙ ПЕРЕРАСЧЕТ ГЕОМЕТРИИ ПРИ ОТКРЫТИИ ОКНА НА ВЕСЬ МОНИТОР
void FullscreenVideoWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    int w = event->size().width();
    int h = event->size().height();

    // 1. Растягиваем сам холст видео на 100% экрана хоста
    if (fullscreenVideoLabel_) {
        fullscreenVideoLabel_->setGeometry(0, 0, w, h);
    }
    
    // 2. Выставляем точные координаты для надписей телеметрии
    if (depthLabel_) depthLabel_->setGeometry(30, 30, 400, 40);
    if (yawLabel_)   yawLabel_->setGeometry(30, 80, 400, 40);
    if (alarmLabel_) alarmLabel_->setGeometry(30, h - 60, 600, 40);

    // 3. МАГИЯ: Принудительно выводим каждый текст на передний план прямо внутри холста!
    if (depthLabel_) depthLabel_->raise();
    if (yawLabel_)   yawLabel_->raise();
    if (alarmLabel_) alarmLabel_->raise();
}
