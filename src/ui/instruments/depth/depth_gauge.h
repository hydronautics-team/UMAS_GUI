#ifndef DEPTH_GAUGE_H
#define DEPTH_GAUGE_H

#include <QWidget>

class DepthGauge : public QWidget
{
public:
    explicit DepthGauge(QWidget *parent = nullptr);

    void setTelemetry(float depthMeters, float verticalSpeed, bool valid = true);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    float depthMeters_ = 0.0f;
    float verticalSpeed_ = 0.0f;
    bool valid_ = false;
};

#endif // DEPTH_GAUGE_H
