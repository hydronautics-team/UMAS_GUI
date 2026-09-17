#include "compass.h"

#include <QPainter>
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

void Compass::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int side = qMin(width(), height());
    p.translate((width() - side) / 2, (height() - side) / 2);
    const QPointF c(side / 2.0, side / 2.0);
    const qreal R = side / 2.0 - 4;

    // циферблат
    p.setPen(QPen(QColor("#2d4052"), 2));
    p.setBrush(QColor("#12314E"));
    p.drawEllipse(c, R, R);

    // точка на экране по относительному углу (0 = вверх, по часовой)
    auto screenPt = [&](float relDeg, qreal radius) {
        const qreal a = qDegreesToRadians(static_cast<double>(relDeg));
        return QPointF(c.x() + radius * qSin(a), c.y() - radius * qCos(a));
    };

    // --- вращающаяся роза (heading-up) ---
    p.save();
    p.translate(c);
    p.rotate(-m_yaw);
    for (int deg = 0; deg < 360; deg += 10) {
        const bool major = (deg % 30 == 0);
        p.save();
        p.rotate(deg);
        p.setPen(QPen(major ? QColor("#8fa9c0") : QColor("#41608a"), major ? 2 : 1));
        p.drawLine(QPointF(0, -R + 3), QPointF(0, -R + (major ? 13 : 8)));
        p.restore();
    }
    QFont f = font();
    f.setPixelSize(qMax(9, side / 16));
    f.setBold(true);
    p.setFont(f);
    for (int deg = 0; deg < 360; deg += 30) {
        p.save();
        p.rotate(deg);
        p.translate(0, -R + 24);
        p.rotate(m_yaw - deg); // текст всегда горизонтально
        QString t;
        switch (deg) {
        case 0:   t = QStringLiteral("С"); break;
        case 90:  t = QStringLiteral("В"); break;
        case 180: t = QStringLiteral("Ю"); break;
        case 270: t = QStringLiteral("З"); break;
        default:  t = QString::number(deg); break;
        }
        p.setPen(QColor(deg % 90 == 0 ? "#00bcd4" : "#8fa9c0"));
        p.drawText(QRectF(-24, -12, 48, 24), Qt::AlignCenter, t);
        p.restore();
    }
    p.restore();

    // --- путевой курс (COG), янтарная стрелка ---
    if (m_cogValid) {
        const float rel = norm360(m_cog - m_yaw);
        const qreal a = qDegreesToRadians(static_cast<double>(rel));
        const QPointF dir(qSin(a), -qCos(a));
        const QPointF perp(dir.y(), -dir.x());
        const QPointF tip = screenPt(rel, R * 0.72);

        p.setPen(QPen(QColor("#ffb347"), 2));
        p.drawLine(screenPt(rel, R * 0.25), tip);
        p.setBrush(QColor("#ffb347"));
        p.setPen(Qt::NoPen);
        p.drawPolygon(QPolygonF() << tip
                      << tip - dir * 9 + perp * 4.5
                      << tip - dir * 9 - perp * 4.5);
    }

    // --- пеленг на дом, зелёная стрелка у обода ---
    if (m_targetValid) {
        const float rel = norm360(m_targetBearing - m_yaw);
        p.setPen(QPen(QColor("#00ff88"), 2));
        p.drawLine(screenPt(rel, R * 0.80), screenPt(rel, R * 0.93));
        p.save();
        p.translate(screenPt(rel, R * 0.87));
        p.rotate(rel);
        p.setBrush(QColor("#00ff88"));
        p.setPen(Qt::NoPen);
        p.drawPolygon(QPolygonF() << QPointF(0, -8) << QPointF(5.5, 3) << QPointF(0, 0) << QPointF(-5.5, 3));
        p.restore();
    }

    // --- неподвижный индекс (lubber line) ---
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#e0e6ed"));
    p.drawPolygon(QPolygonF() << QPointF(c.x(), c.y() - R + 15)
                  << QPointF(c.x() - 6, c.y() - R + 2)
                  << QPointF(c.x() + 6, c.y() - R + 2));

    // --- цифровой курс в центре ---
    p.setBrush(QColor("#0a0e13"));
    p.setPen(Qt::NoPen);
    p.drawEllipse(c, R * 0.30, R * 0.30);
    QFont fd = font();
    fd.setPixelSize(qMax(12, side / 8));
    fd.setBold(true);
    p.setFont(fd);
    p.setPen(QColor("#00ff88"));
    p.drawText(QRectF(c.x() - R * 0.3, c.y() - R * 0.3, R * 0.6, R * 0.6),
               Qt::AlignCenter,
               QString::number(static_cast<int>(qRound(norm360(m_yaw)))) + QChar(176));
}
