#pragma once

#include <QWidget>

class QPainter;

class AttitudeIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit AttitudeIndicator(QWidget *parent = nullptr);
    QSize sizeHint() const override { return QSize(150, 150); }

public slots:
    void setPitch(float deg);
    void setRoll(float deg);
    void setVerticalSpeed(float mps); // + вверх
    void setAttitude(float pitchDeg, float rollDeg);
    void setLightTheme(bool light);
    void setOverlayMode(bool overlay);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void paintOverlay(QPainter &painter);

    float m_pitch = 0.0f;
    float m_roll = 0.0f;
    float m_vs = 0.0f;
    bool m_lightTheme = false;
    bool m_overlayMode = false;
};
