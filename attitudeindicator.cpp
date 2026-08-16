#include "attitudeindicator.h"
#include <QResizeEvent>
#include <QFontMetrics>
#include <QtMath>  // ← ДОБАВИТЬ для qDegreesToRadians, qCos, qSin

AttitudeIndicator::AttitudeIndicator(QWidget *parent)
    : QWidget(parent)
    , m_pitch(0.0)
    , m_roll(0.0)
    , m_radius(0)
    , m_skyColor(0, 100, 200)      // Синий - небо/вода сверху
    , m_groundColor(139, 90, 43)   // Коричневый - дно/земля снизу
    , m_lineColor(0, 255, 136)     // Ярко-зеленый - линии
    , m_textColor(224, 230, 237)   // Светло-серый - текст
{
    setMinimumSize(150, 150);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setStyleSheet("background-color: #0a0e13;");
}

void AttitudeIndicator::setPitch(double pitch)
{
    // Ограничиваем pitch от -90 до +90 градусов
    m_pitch = qBound(-90.0, pitch, 90.0);
    emit pitchChanged(m_pitch);
    update();
}

void AttitudeIndicator::setRoll(double roll)
{
    m_roll = roll;
    emit rollChanged(m_roll);
    update();
}

void AttitudeIndicator::setAttitude(double pitch, double roll)
{
    setPitch(pitch);
    setRoll(roll);
}

void AttitudeIndicator::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    int size = qMin(width(), height());
    m_radius = size / 2 - 10; // Отступ 10 пикселей
}

void AttitudeIndicator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Центрируем координаты
    painter.translate(width() / 2.0, height() / 2.0);
    
    QRectF rect(-m_radius, -m_radius, m_radius * 2, m_radius * 2);
    
    drawBackground(painter, rect);
    drawHorizon(painter, rect);
    drawPitchScale(painter, rect);
    drawRollScale(painter, rect);
    drawAircraftSymbol(painter, rect);
    drawNumericalValues(painter, rect);
}

void AttitudeIndicator::drawBackground(QPainter &painter, const QRectF &rect)
{
    // Рисуем круглую рамку
    painter.save();
    painter.setPen(QPen(m_lineColor, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(rect);
    painter.restore();
}

void AttitudeIndicator::drawHorizon(QPainter &painter, const QRectF &rect)
{
    painter.save();
    
    // Поворачиваем на угол крена
    painter.rotate(-m_roll);
    
    // Смещаем горизонт по вертикали в зависимости от тангажа
    // 10 градусов pitch = примерно 30 пикселей смещения
    double pitchOffset = m_pitch * (rect.height() / 20.0);
    
    // ИСПРАВЛЕНО: Используем QLinearGradient вместо QGradient
    QLinearGradient skyGradient(0, -rect.height(), 0, 0);
    skyGradient.setColorAt(0.0, m_skyColor.lighter(120));
    skyGradient.setColorAt(1.0, m_skyColor);
    
    QLinearGradient groundGradient(0, 0, 0, rect.height());
    groundGradient.setColorAt(0.0, m_groundColor);
    groundGradient.setColorAt(1.0, m_groundColor.darker(120));
    
    // Рисуем небо (верхняя часть)
    painter.fillRect(rect.adjusted(-10, -10, 10, pitchOffset), skyGradient);
    
    // Рисуем землю/дно (нижняя часть)
    painter.fillRect(rect.adjusted(-10, pitchOffset, 10, 10), groundGradient);
    
    // Рисуем линию горизонта
    painter.setPen(QPen(m_lineColor, 2));
    painter.drawLine(rect.left(), pitchOffset, rect.right(), pitchOffset);
    
    // Дополнительные линии на горизонте
    painter.setPen(QPen(m_lineColor, 1));
    double lineSpacing = rect.width() / 6.0;
    for (int i = -2; i <= 2; ++i) {
        if (i == 0) continue;
        double x = i * lineSpacing;
        double lineLength = (i % 2 == 0) ? 20 : 10;
        painter.drawLine(x, pitchOffset - lineLength/2, x, pitchOffset + lineLength/2);
    }
    
    painter.restore();
}

void AttitudeIndicator::drawPitchScale(QPainter &painter, const QRectF &rect)
{
    painter.save();
    painter.rotate(-m_roll);
    
    double pitchOffset = m_pitch * (rect.height() / 20.0);
    painter.setPen(QPen(m_textColor, 1));
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);
    
    // Рисуем шкалу тангажа каждые 10 градусов
    for (int pitch = -50; pitch <= 50; pitch += 10) {
        if (pitch == 0) continue;
        
        double y = pitchOffset - pitch * (rect.height() / 20.0);
        
        if (y > -rect.height() && y < rect.height()) {
            // Длина линии зависит от значения
            double lineLength = (pitch % 20 == 0) ? 40 : 25;
            
            painter.drawLine(-lineLength/2, y, lineLength/2, y);
            
            // Рисуем числовое значение
            QString text = QString::number(qAbs(pitch));
            QFontMetrics fm(font);
            double textWidth = fm.horizontalAdvance(text);
            painter.drawText(-lineLength/2 - textWidth - 5, y + 4, text);
            painter.drawText(lineLength/2 + 5, y + 4, text);
        }
    }
    
    painter.restore();
}

void AttitudeIndicator::drawRollScale(QPainter &painter, const QRectF &rect)
{
    painter.save();
    
    painter.setPen(QPen(m_lineColor, 2));
    
    // Рисуем дугу шкалы крена вверху
    double arcRadius = rect.width() / 2 - 5;
    int startAngle = 150 * 16; // В градусах * 16
    int spanAngle = 60 * 16;
    
    painter.drawArc(rect.adjusted(5, 5, -5, -5), startAngle, spanAngle);
    
    // Рисуем метки крена каждые 30 градусов
    painter.setPen(QPen(m_textColor, 1));
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);
    
    for (int roll = -60; roll <= 60; roll += 30) {
        if (roll == 0) continue;
        
        // ИСПРАВЛЕНО: Используем qDegreesToRadians из QtMath
        double angleRad = qDegreesToRadians(90.0 + roll);
        double x = arcRadius * qCos(angleRad);
        double y = -arcRadius * qSin(angleRad);
        
        // Рисуем метку
        double tickLength = 8;
        painter.drawLine(x, y - tickLength/2, x, y + tickLength/2);
        
        // Рисуем число
        QString text = QString::number(qAbs(roll));
        QFontMetrics fm(font);
        double textWidth = fm.horizontalAdvance(text);
        painter.drawText(x - textWidth/2, y - 15, text);
    }
    
    // Центральная метка (0 градусов)
    painter.drawLine(0, -arcRadius + 5, 0, -arcRadius - 5);
    
    painter.restore();
}

void AttitudeIndicator::drawAircraftSymbol(QPainter &painter, const QRectF &rect)
{
    painter.save();
    
    painter.setPen(QPen(m_lineColor, 3));
    painter.setBrush(Qt::NoBrush);
    
    // Рисуем символ аппарата (фиксированный в центре)
    double symbolSize = rect.width() / 4;
    
    // Горизонтальная линия (крылья/корпус)
    painter.drawLine(-symbolSize, 0, symbolSize, 0);
    
    // Вертикальная линия (центр)
    painter.drawLine(0, -symbolSize/3, 0, symbolSize/3);
    
    // Треугольник/ромб в центре
    QPainterPath path;
    path.moveTo(0, -symbolSize/2);
    path.lineTo(symbolSize/3, 0);
    path.lineTo(0, symbolSize/4);
    path.lineTo(-symbolSize/3, 0);
    path.closeSubpath();
    
    painter.setBrush(m_lineColor);
    painter.setPen(QPen(m_lineColor, 1));
    painter.drawPath(path);
    
    painter.restore();
}

void AttitudeIndicator::drawNumericalValues(QPainter &painter, const QRectF &rect)
{
    painter.save();
    
    QFont font = painter.font();
    font.setPointSize(9);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(QPen(m_textColor, 1));
    
    // Рисуем числовые значения pitch и roll внизу
    QString pitchText = QString("P: %1°").arg(m_pitch, 5, 'f', 1);
    QString rollText = QString("R: %1°").arg(m_roll, 5, 'f', 1);
    
    QFontMetrics fm(font);
    double pitchWidth = fm.horizontalAdvance(pitchText);
    double rollWidth = fm.horizontalAdvance(rollText);
    
    // PITCH слева внизу
    painter.drawText(-rect.width()/2 + 5, rect.height()/2 - 5, pitchText);
    
    // ROLL справа внизу
    painter.drawText(rect.width()/2 - rollWidth - 5, rect.height()/2 - 5, rollText);
    
    painter.restore();
}
