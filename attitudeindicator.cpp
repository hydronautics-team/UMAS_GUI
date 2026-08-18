#include "attitudeindicator.h"

#include <QPainter>
#include <QPainterPath>
#include <QtMath>

AttitudeIndicator::AttitudeIndicator(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(120, 120);
}

void AttitudeIndicator::setPitch(float d) { m_pitch = d; update(); }
void AttitudeIndicator::setRoll(float d)  { m_roll = d; update(); }
void AttitudeIndicator::setVerticalSpeed(float v) { m_vs = v; update(); }
void AttitudeIndicator::setAttitude(float p, float r) { m_pitch = p; m_roll = r; update(); }

void AttitudeIndicator::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int side = qMin(width(), height());
    p.translate((width() - side) / 2, (height() - side) / 2);
    const QPointF c(side / 2.0, side / 2.0);
    const qreal R = side / 2.0 - 4;
    const qreal pxPerDeg = R / 27.5; // видно примерно ±25 град

    // безель
    p.setPen(QPen(QColor("#2d4052"), 2));
    p.setBrush(QColor("#0a0e13"));
    p.drawEllipse(c, R, R);

    QPainterPath clip;
    clip.addEllipse(c, R - 1, R - 1);
    p.setClipPath(clip);

    // --- горизонт: крен вращает, дифферент сдвигает ---
    p.save();
    p.translate(c);
    p.rotate(-m_roll);
    p.translate(0, m_pitch * pxPerDeg);

    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#14365a"));                 // небо
    p.drawRect(QRectF(-2 * R, -4 * R, 4 * R, 4 * R));
    p.setBrush(QColor("#4d3a26"));                 // земля
    p.drawRect(QRectF(-2 * R, 0, 4 * R, 4 * R));
    p.setPen(QPen(QColor("#e0e6ed"), 2));
    p.drawLine(QPointF(-2 * R, 0), QPointF(2 * R, 0));

    // лестница дифферента, янтарные метки ограничений с ±15
    QFont f = font();
    f.setPixelSize(qMax(8, side / 18));
    p.setFont(f);
    for (int d = -20; d <= 20; d += 5) {
        if (d == 0) continue;
        const qreal y = -d * pxPerDeg;
        const bool limit = qAbs(d) >= 15;
        const qreal w = (d % 10 == 0) ? R * 0.34 : R * 0.18;
        p.setPen(QPen(limit ? QColor("#ffb347") : QColor("#e0e6ed"), limit ? 2 : 1));
        p.drawLine(QPointF(-w, y), QPointF(w, y));
        if (d % 10 == 0) {
            p.setPen(QColor("#8fa9c0"));
            p.drawText(QRectF(w + 2, y - 8, 30, 16), Qt::AlignLeft | Qt::AlignVCenter, QString::number(qAbs(d)));
            p.drawText(QRectF(-w - 32, y - 8, 30, 16), Qt::AlignRight | Qt::AlignVCenter, QString::number(qAbs(d)));
        }
    }
    p.restore();

    // --- неподвижная шкала крена (янтарная зона от ±20) ---
    for (int t = -30; t <= 30; t += 10) {
        p.save();
        p.translate(c);
        p.rotate(t);
        p.setPen(QPen(qAbs(t) >= 20 ? QColor("#ffb347") : QColor("#8fa9c0"), qAbs(t) >= 20 ? 2.5 : 1.5));
        p.drawLine(QPointF(0, -R + 3), QPointF(0, -R + 11));
        p.restore();
    }
    // подвижный указатель крена
    p.save();
    p.translate(c);
    p.rotate(m_roll);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#00ff88"));
    p.drawPolygon(QPolygonF() << QPointF(0, -R + 15) << QPointF(-6, -R + 4) << QPointF(6, -R + 4));
    p.restore();

    // --- неподвижный символ аппарата ---
    p.setPen(QPen(QColor("#00ff88"), 3, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(c.x() - R * 0.50, c.y()), QPointF(c.x() - R * 0.18, c.y()));
    p.drawLine(QPointF(c.x() + R * 0.18, c.y()), QPointF(c.x() + R * 0.50, c.y()));
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#00ff88"));
    p.drawEllipse(c, 3, 3);

    // --- вертикальная скорость справа ---
    const qreal vsX = c.x() + R * 0.72;
    p.setPen(QPen(QColor("#41608a"), 1));
    p.drawLine(QPointF(vsX, c.y() - R * 0.35), QPointF(vsX, c.y() + R * 0.35));

    const QColor vsCol = (qAbs(m_vs) > 0.5f) ? QColor("#ffb347") : QColor("#00ff88");
    if (qAbs(m_vs) > 0.02f) {
        const qreal len = qMin<qreal>(qAbs(m_vs) / 1.0, 1.0) * R * 0.33;
        const qreal dir = (m_vs > 0) ? -1 : 1; // вверх = всплытие
        p.setPen(QPen(vsCol, 3, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(vsX, c.y()), QPointF(vsX, c.y() + dir * len));
        p.setBrush(vsCol);
        p.setPen(Qt::NoPen);
        p.drawPolygon(QPolygonF() << QPointF(vsX, c.y() + dir * (len + 7))
                      << QPointF(vsX - 5, c.y() + dir * len)
                      << QPointF(vsX + 5, c.y() + dir * len));
    }
    QFont fv = font();
    fv.setPixelSize(qMax(8, side / 18));
    fv.setBold(true);
    p.setFont(fv);
    p.setPen(vsCol);
    p.drawText(QRectF(vsX - 24, c.y() + R * 0.40, 48, 16), Qt::AlignCenter, QString::number(m_vs, 'f', 1));

    // --- цифровые P / R внизу ---
    QFont ft = font();
    ft.setPixelSize(qMax(8, side / 18));
    p.setFont(ft);
    p.setPen(QColor("#8fa9c0"));
    p.drawText(QRectF(c.x() - R, c.y() + R * 0.55, R, 16), Qt::AlignLeft,
               "P " + QString::number(m_pitch, 'f', 0) + QChar(176));
    p.drawText(QRectF(c.x(), c.y() + R * 0.55, R, 16), Qt::AlignRight,
               "R " + QString::number(m_roll, 'f', 0) + QChar(176));
}
