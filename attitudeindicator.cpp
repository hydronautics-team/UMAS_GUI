#include "attitudeindicator.h"

#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
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
    p.setRenderHint(QPainter::TextAntialiasing);

    const int side = qMin(width(), height());
    p.translate((width() - side) / 2, (height() - side) / 2);
    const QPointF c(side / 2.0, side / 2.0);
    const qreal R = side / 2.0 - qMax(5, side / 35);
    const qreal pxPerDeg = R / 27.5; // видно примерно ±25 град
    const QColor cyan("#74eee3");
    const QColor teal("#20c7b8");
    const QColor muted("#6ba6aa");
    const QColor amber("#f4b860");

    // Мягкое свечение и глубокий технологичный безель.
    for (int i = 5; i >= 1; --i) {
        QColor glow = teal;
        glow.setAlpha(7 + (5 - i) * 4);
        p.setPen(QPen(glow, i * 2.0));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(c, R - 2, R - 2);
    }
    QRadialGradient bezel(c - QPointF(R * 0.15, R * 0.2), R * 1.25);
    bezel.setColorAt(0.0, QColor("#124653"));
    bezel.setColorAt(0.55, QColor("#082d37"));
    bezel.setColorAt(1.0, QColor("#031419"));
    p.setPen(QPen(QColor("#287584"), qMax<qreal>(1.5, side / 90.0)));
    p.setBrush(bezel);
    p.drawEllipse(c, R, R);

    QPainterPath clip;
    clip.addEllipse(c, R - 4, R - 4);
    p.save();
    p.setClipPath(clip);

    // Горизонт: крен вращает, дифферент сдвигает.
    p.save();
    p.translate(c);
    p.rotate(-m_roll);
    p.translate(0, m_pitch * pxPerDeg);

    p.setPen(Qt::NoPen);
    QLinearGradient sky(0, -R * 2.4, 0, 0);
    sky.setColorAt(0.0, QColor("#062b42"));
    sky.setColorAt(0.58, QColor("#0f6075"));
    sky.setColorAt(1.0, QColor("#238a96"));
    p.setBrush(sky);
    p.drawRect(QRectF(-2 * R, -4 * R, 4 * R, 4 * R));
    QLinearGradient seabed(0, 0, 0, R * 2.8);
    seabed.setColorAt(0.0, QColor("#66543a"));
    seabed.setColorAt(0.5, QColor("#3c352c"));
    seabed.setColorAt(1.0, QColor("#171b1b"));
    p.setBrush(seabed);
    p.drawRect(QRectF(-2 * R, 0, 4 * R, 4 * R));

    // Светящийся горизонт и тонкая перспективная сетка.
    p.setPen(QPen(QColor(116, 238, 227, 45), 7, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(-2 * R, 0), QPointF(2 * R, 0));
    p.setPen(QPen(QColor("#dffefa"), 1.8));
    p.drawLine(QPointF(-2 * R, 0), QPointF(2 * R, 0));
    p.setPen(QPen(QColor(215, 243, 241, 24), 1, Qt::DashLine));
    for (int d = -25; d <= 25; d += 5) {
        if (d != 0)
            p.drawLine(QPointF(-R * 1.2, -d * pxPerDeg), QPointF(R * 1.2, -d * pxPerDeg));
    }

    // лестница дифферента, янтарные метки ограничений с ±15
    QFont f = font();
    f.setPixelSize(qMax(8, side / 18));
    p.setFont(f);
    for (int d = -20; d <= 20; d += 5) {
        if (d == 0) continue;
        const qreal y = -d * pxPerDeg;
        const bool limit = qAbs(d) >= 15;
        const qreal w = (d % 10 == 0) ? R * 0.34 : R * 0.18;
        const QColor mark = limit ? amber : QColor("#e2f7f5");
        p.setPen(QPen(mark, limit ? 2.2 : 1.2, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(-w, y), QPointF(-R * 0.07, y));
        p.drawLine(QPointF(R * 0.07, y), QPointF(w, y));
        if (d % 10 == 0) {
            p.setPen(muted);
            p.drawText(QRectF(w + 2, y - 8, 30, 16), Qt::AlignLeft | Qt::AlignVCenter, QString::number(qAbs(d)));
            p.drawText(QRectF(-w - 32, y - 8, 30, 16), Qt::AlignRight | Qt::AlignVCenter, QString::number(qAbs(d)));
        }
    }
    p.restore();
    p.restore();

    // Сегментированный безель и неподвижная шкала крена.
    p.setBrush(Qt::NoBrush);
    for (int deg = 0; deg < 360; deg += 15) {
        QColor segment = (deg % 90 == 0) ? cyan : QColor("#226674");
        segment.setAlpha(deg % 90 == 0 ? 220 : 120);
        p.setPen(QPen(segment, deg % 90 == 0 ? 3.0 : 1.5, Qt::SolidLine, Qt::RoundCap));
        p.drawArc(QRectF(c.x() - R + 3, c.y() - R + 3, 2 * (R - 3), 2 * (R - 3)),
                  (90 - deg - 5) * 16, 8 * 16);
    }
    for (int t = -30; t <= 30; t += 10) {
        p.save();
        p.translate(c);
        p.rotate(t);
        p.setPen(QPen(qAbs(t) >= 20 ? amber : muted,
                      qAbs(t) >= 20 ? 2.5 : 1.5, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(0, -R + 10), QPointF(0, -R + R * 0.18));
        p.restore();
    }
    // подвижный указатель крена
    p.save();
    p.translate(c);
    p.rotate(m_roll);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(116, 238, 227, 55));
    p.drawEllipse(QPointF(0, -R + 14), 11, 11);
    p.setBrush(cyan);
    p.drawPolygon(QPolygonF() << QPointF(0, -R + 20) << QPointF(-7, -R + 8) << QPointF(7, -R + 8));
    p.restore();

    // Неподвижный символ аппарата с неоновым свечением.
    p.setPen(QPen(QColor(93, 226, 215, 55), 8, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(c.x() - R * 0.52, c.y()), QPointF(c.x() - R * 0.17, c.y()));
    p.drawLine(QPointF(c.x() + R * 0.17, c.y()), QPointF(c.x() + R * 0.52, c.y()));
    p.setPen(QPen(cyan, 3, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(c.x() - R * 0.50, c.y()), QPointF(c.x() - R * 0.18, c.y()));
    p.drawLine(QPointF(c.x() + R * 0.18, c.y()), QPointF(c.x() + R * 0.50, c.y()));
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(93, 226, 215, 60));
    p.drawEllipse(c, 7, 7);
    p.setBrush(cyan);
    p.drawEllipse(c, 3, 3);

    // --- вертикальная скорость справа ---
    const qreal vsX = c.x() + R * 0.72;
    p.setPen(QPen(QColor(107, 166, 170, 135), 1));
    p.drawLine(QPointF(vsX, c.y() - R * 0.35), QPointF(vsX, c.y() + R * 0.35));

    const QColor vsCol = (qAbs(m_vs) > 0.5f) ? amber : cyan;
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
    const QRectF vsCapsule(vsX - 20, c.y() + R * 0.39, 40, qMax<qreal>(14, R * 0.2));
    p.setBrush(QColor(3, 20, 25, 210));
    p.setPen(QPen(QColor(vsCol.red(), vsCol.green(), vsCol.blue(), 110), 1));
    p.drawRoundedRect(vsCapsule, 6, 6);
    p.setPen(vsCol);
    p.drawText(vsCapsule, Qt::AlignCenter, QString::number(m_vs, 'f', 1));

    // Цифровые капсулы дифферента и крена.
    QFont ft = font();
    ft.setPixelSize(qMax(8, side / 18));
    ft.setBold(true);
    p.setFont(ft);
    const qreal pillW = R * 0.57;
    const qreal pillH = qMax<qreal>(15, R * 0.22);
    const qreal pillY = c.y() + R * 0.62;
    const QRectF pitchPill(c.x() - pillW - 2, pillY, pillW, pillH);
    const QRectF rollPill(c.x() + 2, pillY, pillW, pillH);
    p.setBrush(QColor(3, 20, 25, 220));
    p.setPen(QPen(QColor(93, 226, 215, 90), 1));
    p.drawRoundedRect(pitchPill, pillH / 2, pillH / 2);
    p.drawRoundedRect(rollPill, pillH / 2, pillH / 2);
    p.setPen(cyan);
    p.drawText(pitchPill, Qt::AlignCenter,
               "P " + QString::number(m_pitch, 'f', 0) + QChar(176));
    p.drawText(rollPill, Qt::AlignCenter,
               "R " + QString::number(m_roll, 'f', 0) + QChar(176));
}
