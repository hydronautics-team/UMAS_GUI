#ifndef BATTERY_STATUS_WIDGET_H
#define BATTERY_STATUS_WIDGET_H

#include <QWidget>

class QPainter;

class BatteryStatusWidget : public QWidget
{
public:
    explicit BatteryStatusWidget(QWidget *parent = nullptr);

    void setBatteries(float firstPercentage, bool firstValid, float secondPercentage,
                      bool secondValid);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawBattery(QPainter &painter, const QRectF &area, int number, float percentage,
                     bool valid) const;

    float firstPercentage_ = 0.0f;
    float secondPercentage_ = 0.0f;
    bool firstValid_ = false;
    bool secondValid_ = false;
};

#endif // BATTERY_STATUS_WIDGET_H
