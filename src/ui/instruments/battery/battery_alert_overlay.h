#ifndef BATTERY_ALERT_OVERLAY_H
#define BATTERY_ALERT_OVERLAY_H

#include <QElapsedTimer>
#include <QTimer>
#include <QWidget>

class BatteryAlertOverlay : public QWidget
{
public:
    enum class Level
    {
        None,
        Warning,
        Critical
    };

    explicit BatteryAlertOverlay(QWidget *parent = nullptr);

    void setLevel(Level level);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Level level_ = Level::None;
    QTimer animationTimer_;
    QElapsedTimer elapsedTimer_;
};

#endif // BATTERY_ALERT_OVERLAY_H
