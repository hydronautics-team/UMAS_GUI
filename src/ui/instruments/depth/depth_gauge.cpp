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

    const qreal centerY = tapeRect.center().y();
    const qreal pixelsPerMeter = qMax(24.0, tapeRect.height() / 10.0);
    const qreal displayedDepth = valid_ ? depthMeters_ : 5.0;
    const qreal firstTick = qFloor(displayedDepth * 2.0f) / 2.0;

    QFont scaleFont = painter.font();
    scaleFont.setPixelSize(17);
    scaleFont.setWeight(QFont::Bold);
    painter.setFont(scaleFont);

    for (qreal value = firstTick - 6.0; value <= firstTick + 6.0; value += 0.5)
    {
        if (value < 0.0)
            continue;

        const qreal y = centerY + (value - displayedDepth) * pixelsPerMeter;
        if (y < tapeRect.top() + 8.0 || y > tapeRect.bottom() - 8.0)
            continue;

        const bool major = qAbs(value - qRound(value)) < 0.01;
        const qreal tickStart = major ? tapeRect.right() - 34.0 : tapeRect.right() - 22.0;
        painter.setPen(
            QPen(major ? QColor(178, 255, 248) : QColor(93, 226, 215, 200), major ? 3.4 : 2.2));
        painter.drawLine(QPointF(tickStart, y), QPointF(tapeRect.right() - 8.0, y));

        const bool coveredByCurrentValue = valid_ && qAbs(value - depthMeters_) < 0.26;
        if (major && !coveredByCurrentValue)
        {
            painter.drawText(QRectF(tapeRect.left() + 5.0, y - 10.0, tapeRect.width() - 47.0, 20.0),
                             Qt::AlignRight | Qt::AlignVCenter, QString::number(value, 'f', 0));
        }
    }

    if (!valid_)
        return;

    const QColor accent(32, 199, 184);
    const QRectF valueRect(tapeRect.left() + 3.0, centerY - 21.0, tapeRect.width() - 47.0, 42.0);
    painter.setPen(QPen(accent, 2.0));
    painter.setBrush(QColor(7, 70, 72, 235));
    painter.drawRoundedRect(valueRect, 7.0, 7.0);

    painter.setPen(QPen(accent, 2.0));
    painter.setBrush(QColor(7, 70, 72, 235));
    QPolygonF pointer;
    pointer << QPointF(valueRect.right() + 14.0, centerY)
            << QPointF(valueRect.right(), centerY - 8.0)
            << QPointF(valueRect.right(), centerY + 8.0);
    painter.drawPolygon(pointer);

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
        QRectF(0.0, height() - 52.0, width() - 2.0, 34.0), Qt::AlignRight | Qt::AlignVCenter,
        QStringLiteral("%1 %2 м/с").arg(direction).arg(qAbs(verticalSpeed_), 0, 'f', 2));
}
