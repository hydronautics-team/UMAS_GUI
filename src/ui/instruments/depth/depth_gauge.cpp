#include "depth_gauge.h"

#include <QPainter>
#include <QtMath>

DepthGauge::DepthGauge(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);
}

void DepthGauge::setTelemetry(float depthMeters, float verticalSpeed, bool valid)
{
    depthMeters_ = qMax(0.0f, depthMeters);
    verticalSpeed_ = verticalSpeed;
    valid_ = valid;
    update();
}

void DepthGauge::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRectF tapeRect(4.0, 6.0, width() - 8.0, height() - 66.0);

    if (!valid_)
    {
        QFont unavailableFont = painter.font();
        unavailableFont.setPixelSize(18);
        unavailableFont.setBold(true);
        painter.setFont(unavailableFont);
        painter.setPen(QColor(255, 232, 236));
        painter.drawText(tapeRect, Qt::AlignCenter, QStringLiteral("N/A"));
        painter.setPen(QColor(116, 238, 227));
        painter.drawText(QRectF(0.0, height() - 52.0, width(), 34.0), Qt::AlignCenter,
                         QStringLiteral("VZ  N/A"));
        return;
    }

    const qreal centerY = tapeRect.center().y();
    const qreal pixelsPerMeter = qMax(24.0, tapeRect.height() / 10.0);
    const qreal firstTick = qFloor(depthMeters_ * 2.0f) / 2.0;

    QFont scaleFont = painter.font();
    scaleFont.setPixelSize(13);
    scaleFont.setWeight(QFont::DemiBold);
    painter.setFont(scaleFont);

    for (qreal value = firstTick - 6.0; value <= firstTick + 6.0; value += 0.5)
    {
        if (value < 0.0)
            continue;

        const qreal y = centerY + (value - depthMeters_) * pixelsPerMeter;
        if (y < tapeRect.top() + 8.0 || y > tapeRect.bottom() - 8.0)
            continue;

        const bool major = qAbs(value - qRound(value)) < 0.01;
        const qreal tickStart = major ? tapeRect.right() - 34.0 : tapeRect.right() - 22.0;
        painter.setPen(
            QPen(major ? QColor(178, 255, 248) : QColor(93, 226, 215, 185), major ? 2.6 : 1.7));
        painter.drawLine(QPointF(tickStart, y), QPointF(tapeRect.right() - 8.0, y));

        if (major)
        {
            painter.drawText(QRectF(tapeRect.left() + 5.0, y - 10.0, tapeRect.width() - 47.0, 20.0),
                             Qt::AlignRight | Qt::AlignVCenter, QString::number(value, 'f', 0));
        }
    }

    const QColor accent(32, 199, 184);
    painter.setPen(QPen(accent, 3.0));
    painter.setBrush(accent);
    QPolygonF pointer;
    pointer << QPointF(tapeRect.left() + 3.0, centerY)
            << QPointF(tapeRect.left() + 14.0, centerY - 8.0)
            << QPointF(tapeRect.left() + 14.0, centerY + 8.0);
    painter.drawPolygon(pointer);

    const QRectF valueRect(tapeRect.left() + 15.0, centerY - 21.0, tapeRect.width() - 23.0, 42.0);
    painter.setPen(QPen(accent, 2.0));
    painter.setBrush(QColor(7, 70, 72, 235));
    painter.drawRoundedRect(valueRect, 7.0, 7.0);
    QFont valueFont = painter.font();
    valueFont.setPixelSize(19);
    valueFont.setBold(true);
    painter.setFont(valueFont);
    painter.setPen(QColor(223, 255, 251));
    painter.drawText(valueRect, Qt::AlignCenter,
                     QStringLiteral("%1 м").arg(depthMeters_, 0, 'f', 1));

    QFont speedFont = painter.font();
    speedFont.setPixelSize(16);
    speedFont.setWeight(QFont::Black);
    painter.setFont(speedFont);
    painter.setPen(QColor(116, 238, 227));
    const QString direction =
        verticalSpeed_ > 0.01f
            ? QStringLiteral("↑")
            : (verticalSpeed_ < -0.01f ? QStringLiteral("↓") : QStringLiteral("•"));
    painter.drawText(
        QRectF(0.0, height() - 52.0, width(), 34.0), Qt::AlignCenter,
        QStringLiteral("VZ %1 %2 м/с").arg(direction).arg(qAbs(verticalSpeed_), 0, 'f', 2));
}
