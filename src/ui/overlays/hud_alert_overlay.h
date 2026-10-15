#ifndef HUD_ALERT_OVERLAY_H
#define HUD_ALERT_OVERLAY_H

#include <QElapsedTimer>
#include <QHash>
#include <QString>
#include <QTimer>
#include <QWidget>

class HudAlertOverlay : public QWidget
{
public:
    enum class Level
    {
        None,
        Warning,
        Critical
    };

    explicit HudAlertOverlay(QWidget *parent = nullptr);

    void setAlert(const QString &reason, Level level, bool active);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void updateLevel();

    Level level_ = Level::None;
    QHash<QString, Level> alerts_;
    QTimer animationTimer_;
    QElapsedTimer elapsedTimer_;
};

#endif // HUD_ALERT_OVERLAY_H
