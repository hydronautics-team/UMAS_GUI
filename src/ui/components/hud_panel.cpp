#include "hud_panel.h"

#include <QLinearGradient>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>

HudPanel::HudPanel(QWidget *parent) : QFrame(parent)
{
    setAttribute(Qt::WA_StyledBackground, false);
    setFrameShape(QFrame::NoFrame);
}

bool HudPanel::accentVisible() const { return accentVisible_; }

void HudPanel::setAccentVisible(bool visible)
{
    if (accentVisible_ == visible)
        return;
    accentVisible_ = visible;
    update();
}

void HudPanel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    constexpr qreal chamfer = 11.0;
    const QRectF bounds = rect().adjusted(0.5, 0.5, -0.5, -0.5);

    QPainterPath panelPath;
    panelPath.moveTo(bounds.left() + chamfer, bounds.top());
    panelPath.lineTo(bounds.right() - chamfer * 1.6, bounds.top());
    panelPath.lineTo(bounds.right(), bounds.top() + chamfer);
    panelPath.lineTo(bounds.right(), bounds.bottom() - chamfer);
    panelPath.lineTo(bounds.right() - chamfer, bounds.bottom());
    panelPath.lineTo(bounds.left() + chamfer, bounds.bottom());
    panelPath.lineTo(bounds.left(), bounds.bottom() - chamfer);
    panelPath.lineTo(bounds.left(), bounds.top() + chamfer);
    panelPath.closeSubpath();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QLinearGradient background(bounds.topLeft(), bounds.bottomLeft());
    background.setColorAt(0.0, QColor(QStringLiteral("#001522")));
    background.setColorAt(1.0, QColor(QStringLiteral("#000C17")));
    painter.fillPath(panelPath, background);

    painter.setPen(QPen(QColor(QStringLiteral("#07516B")), 1.0));
    painter.drawPath(panelPath);

    if (!accentVisible_)
        return;

    painter.setPen(QPen(QColor(QStringLiteral("#00D9E6")), 2.0, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(QPointF(bounds.left() + 18.0, bounds.top()),
                     QPointF(bounds.left() + 48.0, bounds.top()));
    painter.drawLine(QPointF(bounds.right() - 54.0, bounds.bottom()),
                     QPointF(bounds.right() - 27.0, bounds.bottom()));
}
