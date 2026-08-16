#ifndef ATTITUDEINDICATOR_H
#define ATTITUDEINDICATOR_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <cmath>

class AttitudeIndicator : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(double pitch READ pitch WRITE setPitch NOTIFY pitchChanged)
    Q_PROPERTY(double roll READ roll WRITE setRoll NOTIFY rollChanged)

public:
    explicit AttitudeIndicator(QWidget *parent = nullptr);
    ~AttitudeIndicator() override = default;

    double pitch() const { return m_pitch; }
    double roll() const { return m_roll; }

public slots:
    void setPitch(double pitch);
    void setRoll(double roll);
    void setAttitude(double pitch, double roll);

signals:
    void pitchChanged(double pitch);
    void rollChanged(double roll);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawBackground(QPainter &painter, const QRectF &rect);
    void drawHorizon(QPainter &painter, const QRectF &rect);
    void drawAircraftSymbol(QPainter &painter, const QRectF &rect);
    void drawPitchScale(QPainter &painter, const QRectF &rect);
    void drawRollScale(QPainter &painter, const QRectF &rect);
    void drawNumericalValues(QPainter &painter, const QRectF &rect);

    double m_pitch; // Тангаж в градусах (-90 to +90)
    double m_roll;  // Крен в градусах (-180 to +180)
    int m_radius;
    QColor m_skyColor;
    QColor m_groundColor;
    QColor m_lineColor;
    QColor m_textColor;
};

#endif // ATTITUDEINDICATOR_H
