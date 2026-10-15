#include "hud_alert_overlay.h"

#include <QLinearGradient>
#include <QPainter>
#include <cmath>

HudAlertOverlay::HudAlertOverlay(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);
    elapsedTimer_.start();
    animationTimer_.setInterval(50);
    connect(&animationTimer_, &QTimer::timeout, this, qOverload<>(&QWidget::update));
}

void HudAlertOverlay::setAlert(const QString &reason, Level level, bool active)
{
    if (active)
        alerts_.insert(reason, level);
    else
        alerts_.remove(reason);

    updateLevel();
}

void HudAlertOverlay::updateLevel()
{
    Level level = Level::None;
    for (const Level candidate : alerts_)
    {
        if (static_cast<int>(candidate) > static_cast<int>(level))
            level = candidate;
    }

    if (level_ == level)
        return;

    level_ = level;
    if (level_ == Level::Critical)
        animationTimer_.start();
    else
        animationTimer_.stop();

    setVisible(level_ != Level::None);
    update();
}

void HudAlertOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (level_ == Level::None)
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const qreal pulse =
        level_ == Level::Critical ? 0.5 + 0.5 * std::sin(elapsedTimer_.elapsed() / 360.0) : 0.0;
    const int alpha = level_ == Level::Critical ? static_cast<int>(82.0 + pulse * 48.0) : 48;
    const QColor edgeColor =
        level_ == Level::Critical ? QColor(225, 35, 63, alpha) : QColor(232, 161, 66, alpha);
    const int glowWidth = qBound(80, width() / 9, 180);

    QLinearGradient leftGradient(0.0, 0.0, glowWidth, 0.0);
    leftGradient.setColorAt(0.0, edgeColor);
    leftGradient.setColorAt(1.0, QColor(edgeColor.red(), edgeColor.green(), edgeColor.blue(), 0));
    painter.fillRect(QRect(0, 0, glowWidth, height()), leftGradient);

    QLinearGradient rightGradient(width(), 0.0, width() - glowWidth, 0.0);
    rightGradient.setColorAt(0.0, edgeColor);
    rightGradient.setColorAt(1.0, QColor(edgeColor.red(), edgeColor.green(), edgeColor.blue(), 0));
    painter.fillRect(QRect(width() - glowWidth, 0, glowWidth, height()), rightGradient);
}
