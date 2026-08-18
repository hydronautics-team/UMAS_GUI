#pragma once

#include <QWidget>

class Compass : public QWidget
{
    Q_OBJECT
public:
    explicit Compass(QWidget *parent = nullptr);
    QSize sizeHint() const override { return QSize(150, 150); }

public slots:
    void setYaw(float yawDeg);
    void setTargetBearing(float bearingDeg, bool valid = true); // пеленг на дом/цель
    void setCog(float cogDeg, bool valid = true);               // путевой курс

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    float m_yaw = 0.0f;
    float m_targetBearing = 0.0f;
    float m_cog = 0.0f;
    bool  m_targetValid = false;
    bool  m_cogValid = false;
};
