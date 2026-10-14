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
void AttitudeIndicator::setLightTheme(bool light) { m_lightTheme = light; update(); }
void AttitudeIndicator::setOverlayMode(bool overlay)
{
    m_overlayMode = overlay;
    setAttribute(Qt::WA_TranslucentBackground, overlay);
    setAutoFillBackground(false);
    update();
}

void AttitudeIndicator::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    if (m_overlayMode) {
        paintOverlay(p);
        return;
    }

    const int side = qMin(width(), height());
    p.translate((width() - side) / 2, (height() - side) / 2);
    const QPointF c(side / 2.0, side / 2.0);
    const qreal R = side / 2.0 - qMax(5, side / 35);
    const qreal pxPerDeg = R / 27.5; // видно примерно ±25 град
    const QColor cyan(m_lightTheme ? "#087e79" : "#69ddd3");
    const QColor teal(m_lightTheme ? "#169c95" : "#24b8ae");
    const QColor muted(m_lightTheme ? "#587c82" : "#78a7ad");
    const QColor amber(m_lightTheme ? "#b87524" : "#efb766");

    // Мягкое свечение и глубокий технологичный безель.
    for (int i = 5; i >= 1; --i) {
        QColor glow = teal;
        glow.setAlpha(7 + (5 - i) * 4);
        p.setPen(QPen(glow, i * 2.0));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(c, R - 2, R - 2);
    }
    QRadialGradient bezel(c - QPointF(R * 0.15, R * 0.2), R * 1.25);
    bezel.setColorAt(0.0, QColor(m_lightTheme ? "#eef6f5" : "#123d49"));
    bezel.setColorAt(0.55, QColor(m_lightTheme ? "#d6e6e5" : "#092a34"));
    bezel.setColorAt(1.0, QColor(m_lightTheme ? "#b9d0d0" : "#04171d"));
    p.setPen(QPen(QColor(m_lightTheme ? "#789fa0" : "#2c6875"), qMax<qreal>(1.5, side / 90.0)));
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
    sky.setColorAt(0.0, QColor(m_lightTheme ? "#79b8ce" : "#0d4262"));
    sky.setColorAt(0.58, QColor(m_lightTheme ? "#9bd3dc" : "#176f86"));
    sky.setColorAt(1.0, QColor(m_lightTheme ? "#c4e8e5" : "#2f9ca2"));
    p.setBrush(sky);
    p.drawRect(QRectF(-2 * R, -4 * R, 4 * R, 4 * R));
    QLinearGradient seabed(0, 0, 0, R * 2.8);
    seabed.setColorAt(0.0, QColor(m_lightTheme ? "#4c7180" : "#173846"));
    seabed.setColorAt(0.5, QColor(m_lightTheme ? "#365866" : "#102a36"));
    seabed.setColorAt(1.0, QColor(m_lightTheme ? "#243f4b" : "#071a23"));
    p.setBrush(seabed);
    p.drawRect(QRectF(-2 * R, 0, 4 * R, 4 * R));

    // Светящийся горизонт и тонкая перспективная сетка.
    p.setPen(QPen(m_lightTheme ? QColor(8, 90, 86, 70) : QColor(116, 238, 227, 60),
                  8, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(-2 * R, 0), QPointF(2 * R, 0));
    p.setPen(QPen(QColor(m_lightTheme ? "#f6fffd" : "#eafffc"), 2.2));
    p.drawLine(QPointF(-2 * R, 0), QPointF(2 * R, 0));
    p.setPen(QPen(m_lightTheme ? QColor(15, 85, 82, 30) : QColor(215, 243, 241, 24),
                  1, Qt::DashLine));
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
        const QColor mark = limit ? amber : QColor(m_lightTheme ? "#f5fbfa" : "#dcefed");
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
        QColor segment = (deg % 90 == 0) ? cyan : QColor(m_lightTheme ? "#789fa0" : "#2c6875");
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
    p.setBrush(m_lightTheme ? QColor(8, 127, 119, 60) : QColor(116, 238, 227, 55));
    p.drawEllipse(QPointF(0, -R + 14), 11, 11);
    p.setBrush(cyan);
    p.drawPolygon(QPolygonF() << QPointF(0, -R + 20) << QPointF(-7, -R + 8) << QPointF(7, -R + 8));
    p.restore();

    // Неподвижный символ аппарата с неоновым свечением.
    p.setPen(QPen(m_lightTheme ? QColor(8, 127, 119, 55) : QColor(93, 226, 215, 55),
                  8, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(c.x() - R * 0.52, c.y()), QPointF(c.x() - R * 0.17, c.y()));
    p.drawLine(QPointF(c.x() + R * 0.17, c.y()), QPointF(c.x() + R * 0.52, c.y()));
    p.setPen(QPen(cyan, 3, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(c.x() - R * 0.50, c.y()), QPointF(c.x() - R * 0.18, c.y()));
    p.drawLine(QPointF(c.x() + R * 0.18, c.y()), QPointF(c.x() + R * 0.50, c.y()));
    p.setPen(Qt::NoPen);
    p.setBrush(m_lightTheme ? QColor(8, 127, 119, 60) : QColor(93, 226, 215, 60));
    p.drawEllipse(c, 7, 7);
    p.setBrush(cyan);
    p.drawEllipse(c, 3, 3);

    // --- вертикальная скорость справа ---
    const qreal vsX = c.x() + R * 0.72;
    p.setPen(QPen(m_lightTheme ? QColor(55, 113, 115, 145) : QColor(107, 166, 170, 135), 1));
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
    p.setBrush(m_lightTheme ? QColor(235, 245, 243, 225) : QColor(3, 20, 25, 210));
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
    p.setBrush(m_lightTheme ? QColor(235, 245, 243, 235) : QColor(3, 20, 25, 220));
    p.setPen(QPen(m_lightTheme ? QColor(8, 127, 119, 110) : QColor(93, 226, 215, 90), 1));
    p.drawRoundedRect(pitchPill, pillH / 2, pillH / 2);
    p.drawRoundedRect(rollPill, pillH / 2, pillH / 2);
    p.setPen(cyan);
    p.drawText(pitchPill, Qt::AlignCenter,
               "P " + QString::number(m_pitch, 'f', 0) + QChar(176));
    p.drawText(rollPill, Qt::AlignCenter,
               "R " + QString::number(m_roll, 'f', 0) + QChar(176));
}

void AttitudeIndicator::paintOverlay(QPainter& p)
{
    const QPointF c(width() / 2.0, height() / 2.0);
    const qreal unit = qMin(width(), height());
    const qreal pxPerDeg = unit / 42.0;
    const QColor cyan("#74eee3");
    const QColor white("#efffff");
    const QColor amber("#f4b860");

    // Подвижная шкала дифферента. Чёрная подложка сохраняет читаемость
    // как на светлом, так и на тёмном видеопотоке.
    p.save();
    p.setClipRect(QRectF(width() * 0.13, height() * 0.18,
                         width() * 0.74, height() * 0.64));
    p.translate(c);
    p.rotate(-m_roll);
    p.translate(0, m_pitch * pxPerDeg);

    QFont scaleFont = font();
    scaleFont.setPixelSize(qMax(11, static_cast<int>(unit / 28.0)));
    scaleFont.setBold(true);
    p.setFont(scaleFont);

    for (int deg = -30; deg <= 30; deg += 5) {
        const qreal y = -deg * pxPerDeg;
        const bool horizon = deg == 0;
        const bool major = deg % 10 == 0;
        const bool warning = qAbs(deg) >= 20;
        const qreal innerGap = horizon ? unit * 0.055 : unit * 0.075;
        const qreal outer = horizon ? unit * 0.34 : (major ? unit * 0.22 : unit * 0.14);
        const QColor lineColor = warning ? amber : (horizon ? cyan : white);
        const qreal lineWidth = horizon ? 3.0 : (major ? 2.2 : 1.5);

        p.setPen(QPen(QColor(0, 0, 0, 180), lineWidth + 4,
                      Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(-outer, y), QPointF(-innerGap, y));
        p.drawLine(QPointF(innerGap, y), QPointF(outer, y));
        p.setPen(QPen(lineColor, lineWidth, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(-outer, y), QPointF(-innerGap, y));
        p.drawLine(QPointF(innerGap, y), QPointF(outer, y));

        if (!horizon && major) {
            const QString value = QString::number(qAbs(deg));
            const QRectF leftText(-outer - 48, y - 13, 42, 26);
            const QRectF rightText(outer + 6, y - 13, 42, 26);
            p.setPen(QPen(QColor(0, 0, 0, 210), 3));
            p.drawText(leftText, Qt::AlignRight | Qt::AlignVCenter, value);
            p.drawText(rightText, Qt::AlignLeft | Qt::AlignVCenter, value);
            p.setPen(lineColor);
            p.drawText(leftText, Qt::AlignRight | Qt::AlignVCenter, value);
            p.drawText(rightText, Qt::AlignLeft | Qt::AlignVCenter, value);
        }
    }
    p.restore();

    // Неподвижная дуга крена над центром.
    const qreal rollRadius = unit * 0.39;
    for (int angle = -45; angle <= 45; angle += 15) {
        p.save();
        p.translate(c);
        p.rotate(angle);
        const QColor tickColor = qAbs(angle) >= 30 ? amber : white;
        p.setPen(QPen(QColor(0, 0, 0, 190), 5, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(0, -rollRadius), QPointF(0, -rollRadius + 13));
        p.setPen(QPen(tickColor, 2, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(0, -rollRadius), QPointF(0, -rollRadius + 13));
        p.restore();
    }

    // Подвижный индекс текущего крена.
    p.save();
    p.translate(c);
    p.rotate(m_roll);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, 150));
    p.drawEllipse(QPointF(0, -rollRadius + 5), 12, 12);
    p.setBrush(cyan);
    p.drawPolygon(QPolygonF() << QPointF(0, -rollRadius + 14)
                  << QPointF(-7, -rollRadius + 1)
                  << QPointF(7, -rollRadius + 1));
    p.restore();

    // Центральный неподвижный символ аппарата.
    const qreal wing = unit * 0.17;
    const qreal gap = unit * 0.035;
    p.setPen(QPen(QColor(0, 0, 0, 185), 8, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(c.x() - wing, c.y()), QPointF(c.x() - gap, c.y()));
    p.drawLine(QPointF(c.x() + gap, c.y()), QPointF(c.x() + wing, c.y()));
    p.setPen(QPen(cyan, 3, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(c.x() - wing, c.y()), QPointF(c.x() - gap, c.y()));
    p.drawLine(QPointF(c.x() + gap, c.y()), QPointF(c.x() + wing, c.y()));
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, 170));
    p.drawEllipse(c, 8, 8);
    p.setBrush(cyan);
    p.drawEllipse(c, 3.5, 3.5);
}
