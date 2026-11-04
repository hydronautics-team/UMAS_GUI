#include "battery_status_widget.h"

#include <QPainter>

BatteryStatusWidget::BatteryStatusWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);
}

void BatteryStatusWidget::setBatteries(float firstPercentage, bool firstValid,
                                       float secondPercentage, bool secondValid)
{
    firstPercentage_ = qBound(0.0f, firstPercentage, 100.0f);
    secondPercentage_ = qBound(0.0f, secondPercentage, 100.0f);
    firstValid_ = firstValid;
    secondValid_ = secondValid;
    update();
}

void BatteryStatusWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const qreal rowHeight = height() / 2.0;
    drawBattery(painter, QRectF(0.0, 0.0, width(), rowHeight), 1, firstPercentage_, firstValid_);
    drawBattery(painter, QRectF(0.0, rowHeight, width(), rowHeight), 2, secondPercentage_,
                secondValid_);
}

void BatteryStatusWidget::drawBattery(QPainter &painter, const QRectF &area, int number,
                                      float percentage, bool valid) const
{
    const QRectF body(area.right() - 155.0, area.center().y() - 14.0, 60.0, 28.0);
    const QRectF terminal(body.right() + 3.0, body.center().y() - 6.0, 7.0, 12.0);
    const QColor outline = valid ? QColor(178, 255, 248) : QColor(119, 143, 146);

    painter.setPen(QPen(outline, 2.5));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(body, 5.0, 5.0);
    painter.setPen(Qt::NoPen);
    painter.setBrush(outline);
    painter.drawRoundedRect(terminal, 1.5, 1.5);

    if (valid)
    {
        const QColor chargeColor =
            percentage <= 20.0f
                ? QColor(240, 82, 104)
                : (percentage <= 50.0f ? QColor(239, 183, 102) : QColor(32, 199, 184));
        const qreal fillWidth = (body.width() - 8.0) * percentage / 100.0;
        painter.setBrush(chargeColor);
        painter.drawRoundedRect(
            QRectF(body.left() + 4.0, body.top() + 4.0, fillWidth, body.height() - 8.0), 2.5, 2.5);
    }

    QFont indexFont = painter.font();
    indexFont.setPixelSize(14);
    indexFont.setBold(true);
    painter.setFont(indexFont);
    painter.setPen(QColor(138, 245, 236));
    painter.drawText(QRectF(body.left() - 26.0, area.top(), 21.0, area.height()), Qt::AlignCenter,
                     QString::number(number));

    QFont valueFont = painter.font();
    valueFont.setPixelSize(22);
    valueFont.setWeight(QFont::Bold);
    painter.setFont(valueFont);
    painter.setPen(valid ? QColor(223, 255, 251) : QColor(166, 183, 184));
    painter.drawText(QRectF(terminal.right() + 10.0, area.top(),
                            area.right() - terminal.right() - 12.0, area.height()),
                     Qt::AlignRight | Qt::AlignVCenter,
                     valid ? QStringLiteral("%1%").arg(percentage, 0, 'f', 0)
                           : QStringLiteral("N/A"));
}
