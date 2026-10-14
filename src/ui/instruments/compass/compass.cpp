#include "compass.h"

#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>
#include <QtMath>
#include <cmath>

static float norm360(float a)
{
    a = std::fmod(a, 360.0f);
    if (a < 0.0f) a += 360.0f;
    return a;
}

Compass::Compass(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(120, 120);
}

void Compass::setYaw(float yawDeg)           { m_yaw = norm360(yawDeg); update(); }
void Compass::setTargetBearing(float b, bool v) { m_targetBearing = norm360(b); m_targetValid = v; update(); }
void Compass::setCog(float c, bool v)        { m_cog = norm360(c); m_cogValid = v; update(); }
void Compass::setLightTheme(bool light)      { m_lightTheme = light; update(); }

void Compass::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    const int side = qMin(width(), height());
    p.translate((width() - side) / 2, (height() - side) / 2);
    const QPointF c(side / 2.0, side / 2.0);
    const qreal R = side / 2.0 - qMax(5, side / 35);
    const QColor cyan(m_lightTheme ? "#087f77" : "#74eee3");
    const QColor teal(m_lightTheme ? "#15a99f" : "#20c7b8");
    const QColor muted(m_lightTheme ? "#557b7d" : "#6ba6aa");
    const QColor amber(m_lightTheme ? "#c87916" : "#f4b860");

    // Мягкое внешнее свечение отделяет прибор от панели.
    for (int i = 5; i >= 1; --i) {
        QColor glow = teal;
        glow.setAlpha(7 + (5 - i) * 4);
        p.setPen(QPen(glow, i * 2.0));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(c, R - 2, R - 2);
    }

    // Глубокий радиальный циферблат с подсвеченным центром.
    QRadialGradient dial(c - QPointF(R * 0.16, R * 0.20), R * 1.25);
    dial.setColorAt(0.0, QColor(m_lightTheme ? "#f0f7f5" : "#124653"));
    dial.setColorAt(0.48, QColor(m_lightTheme ? "#dbecea" : "#082d37"));
    dial.setColorAt(1.0, QColor(m_lightTheme ? "#bfd8d5" : "#031419"));
    p.setPen(QPen(QColor(m_lightTheme ? "#78aaa7" : "#287584"), qMax<qreal>(1.5, side / 90.0)));
    p.setBrush(dial);
    p.drawEllipse(c, R, R);

    QPainterPath faceClip;
    faceClip.addEllipse(c, R - 3, R - 3);
    p.save();
    p.setClipPath(faceClip);

    // Радиолокационная сетка и тонкие осевые лучи.
    for (qreal k : {0.24, 0.48, 0.72}) {
        p.setPen(QPen(m_lightTheme ? QColor(8, 127, 119, k == 0.72 ? 52 : 32)
                                   : QColor(93, 226, 215, k == 0.72 ? 34 : 22), 1));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(c, R * k, R * k);
    }
    p.setPen(QPen(m_lightTheme ? QColor(8, 127, 119, 34) : QColor(93, 226, 215, 22),
                  1, Qt::DashLine));
    for (int deg = 0; deg < 360; deg += 45)
        p.drawLine(c, c + QPointF(qSin(qDegreesToRadians(static_cast<double>(deg))),
                                 -qCos(qDegreesToRadians(static_cast<double>(deg)))) * R);

    // Динамичный сектор обзора по направлению движения.
    QPainterPath cone;
    cone.moveTo(c);
    cone.lineTo(c + QPointF(-R * 0.25, -R * 0.82));
    cone.arcTo(QRectF(c.x() - R * 0.86, c.y() - R * 0.86, R * 1.72, R * 1.72), 107, -34);
    cone.closeSubpath();
    p.fillPath(cone, m_lightTheme ? QColor(21, 169, 159, 24) : QColor(32, 199, 184, 13));
    p.restore();

    // Сегментированный технологичный безель.
    p.setBrush(Qt::NoBrush);
    for (int deg = 0; deg < 360; deg += 15) {
        const bool cardinal = (deg % 90 == 0);
        QColor segment = cardinal ? cyan : QColor(m_lightTheme ? "#78aaa7" : "#226674");
        segment.setAlpha(cardinal ? 220 : 125);
        p.setPen(QPen(segment, cardinal ? 3.0 : 1.5, Qt::SolidLine, Qt::RoundCap));
        const int start = (90 - deg - 5) * 16;
        p.drawArc(QRectF(c.x() - R + 3, c.y() - R + 3, 2 * (R - 3), 2 * (R - 3)), start, 8 * 16);
    }

    // точка на экране по относительному углу (0 = вверх, по часовой)
    auto screenPt = [&](float relDeg, qreal radius) {
        const qreal a = qDegreesToRadians(static_cast<double>(relDeg));
        return QPointF(c.x() + radius * qSin(a), c.y() - radius * qCos(a));
    };

    // Вращающаяся роза: аппарат всегда смотрит вверх.
    p.save();
    p.translate(c);
    p.rotate(-m_yaw);
    for (int deg = 0; deg < 360; deg += 5) {
        const bool major = (deg % 30 == 0);
        const bool medium = (deg % 10 == 0);
        p.save();
        p.rotate(deg);
        p.setPen(QPen(major ? cyan : (medium ? muted : QColor(m_lightTheme ? "#8aafad" : "#275761")),
                      major ? 2.1 : 1.0, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(0, -R + 10),
                   QPointF(0, -R + (major ? R * 0.17 : medium ? R * 0.125 : R * 0.09)));
        p.restore();
    }
    QFont f = font();
    f.setPixelSize(qMax(8, side / 17));
    f.setBold(true);
    f.setLetterSpacing(QFont::AbsoluteSpacing, 0.6);
    p.setFont(f);
    for (int deg = 0; deg < 360; deg += 30) {
        p.save();
        p.rotate(deg);
        p.translate(0, -R * 0.69);
        p.rotate(m_yaw - deg); // текст всегда горизонтально
        QString t;
        switch (deg) {
        case 0:   t = QStringLiteral("С"); break;
        case 90:  t = QStringLiteral("В"); break;
        case 180: t = QStringLiteral("Ю"); break;
        case 270: t = QStringLiteral("З"); break;
        default:  t = QString::number(deg); break;
        }
        p.setPen(deg % 90 == 0 ? cyan : muted);
        p.drawText(QRectF(-24, -12, 48, 24), Qt::AlignCenter, t);
        p.restore();
    }
    p.restore();

    // Центральная визирная линия и прицел.
    p.setPen(QPen(m_lightTheme ? QColor(8, 127, 119, 95) : QColor(116, 238, 227, 82), 1));
    p.drawLine(screenPt(0, R * 0.18), screenPt(0, R * 0.57));
    p.setPen(QPen(m_lightTheme ? QColor(8, 127, 119, 165) : QColor(116, 238, 227, 150), 1));
    p.setBrush(QColor(m_lightTheme ? "#e6f1ef" : "#061a21"));
    p.drawEllipse(c, R * 0.075, R * 0.075);
    p.setBrush(cyan);
    p.setPen(Qt::NoPen);
    p.drawEllipse(c, qMax<qreal>(1.8, side / 70.0), qMax<qreal>(1.8, side / 70.0));

    // --- путевой курс (COG), янтарная стрелка ---
    if (m_cogValid) {
        const float rel = norm360(m_cog - m_yaw);
        const qreal a = qDegreesToRadians(static_cast<double>(rel));
        const QPointF dir(qSin(a), -qCos(a));
        const QPointF perp(dir.y(), -dir.x());
        const QPointF tip = screenPt(rel, R * 0.72);

        p.setPen(QPen(m_lightTheme ? QColor(200, 121, 22, 85) : QColor(244, 184, 96, 80),
                      6, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(screenPt(rel, R * 0.22), tip);
        p.setPen(QPen(amber, 2, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(screenPt(rel, R * 0.25), tip);
        p.setBrush(amber);
        p.setPen(Qt::NoPen);
        p.drawPolygon(QPolygonF() << tip
                      << tip - dir * 9 + perp * 4.5
                      << tip - dir * 9 - perp * 4.5);
    }

    // --- пеленг на дом, зелёная стрелка у обода ---
    if (m_targetValid) {
        const float rel = norm360(m_targetBearing - m_yaw);
        p.setPen(QPen(m_lightTheme ? QColor(8, 127, 119, 75) : QColor(93, 226, 215, 70),
                      7, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(screenPt(rel, R * 0.78), screenPt(rel, R * 0.92));
        p.setPen(QPen(teal, 2));
        p.drawLine(screenPt(rel, R * 0.80), screenPt(rel, R * 0.93));
        p.save();
        p.translate(screenPt(rel, R * 0.87));
        p.rotate(rel);
        p.setBrush(cyan);
        p.setPen(Qt::NoPen);
        p.drawPolygon(QPolygonF() << QPointF(0, -8) << QPointF(5.5, 3) << QPointF(0, 0) << QPointF(-5.5, 3));
        p.restore();
    }

    // Неподвижный индекс курса с мягким свечением.
    p.setPen(Qt::NoPen);
    p.setBrush(m_lightTheme ? QColor(8, 127, 119, 60) : QColor(116, 238, 227, 55));
    p.drawEllipse(screenPt(0, R - 13), 11, 11);
    p.setBrush(cyan);
    p.drawPolygon(QPolygonF() << QPointF(c.x(), c.y() - R + 19)
                  << QPointF(c.x() - 7, c.y() - R + 7)
                  << QPointF(c.x() + 7, c.y() - R + 7));

    // Цифровая капсула курса.
    const QRectF capsule(c.x() - R * 0.31, c.y() + R * 0.18, R * 0.62, R * 0.27);
    p.setBrush(m_lightTheme ? QColor(235, 245, 243, 235) : QColor(3, 20, 25, 225));
    p.setPen(QPen(m_lightTheme ? QColor(8, 127, 119, 130) : QColor(93, 226, 215, 115), 1));
    p.drawRoundedRect(capsule, capsule.height() / 2, capsule.height() / 2);
    QFont fd = font();
    fd.setPixelSize(qMax(11, side / 9));
    fd.setBold(true);
    fd.setLetterSpacing(QFont::AbsoluteSpacing, 1.0);
    p.setFont(fd);
    p.setPen(cyan);
    const int heading = static_cast<int>(qRound(norm360(m_yaw))) % 360;
    p.drawText(capsule, Qt::AlignCenter,
               QStringLiteral("%1°").arg(heading, 3, 10, QLatin1Char('0')));
}
