#ifndef FULLSCREEN_VIDEO_WINDOW_H
#define FULLSCREEN_VIDEO_WINDOW_H

#include "uv_state.h"
#include <QKeyEvent>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QWidget>

class Compass;
class AttitudeIndicator;
class DepthGauge;
class BatteryStatusWidget;
class HudAlertOverlay;
class HudNotificationStack;
class QFrame;
class QPushButton;
class VideoPlayerWidget;

class FullscreenVideoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FullscreenVideoWindow(UVState *uvState, QWidget *parent = nullptr);
    ~FullscreenVideoWindow();

    void setVideoWidget(QWidget *widget);

public slots:
    void drawFrame(const QPixmap &pixmap);

signals:
    void windowClosed();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void updateHudData();
    void setSecondaryCameraEnabled(bool enabled);

private:
    UVState *uvState_;
    QLabel *fullscreenVideoLabel_;   // Холст для видео
    QWidget *videoPlayer_ = nullptr; // Указатель на видео-плеер
    QFrame *secondaryCameraFrame_;
    QLabel *secondaryCameraTitle_;
    VideoPlayerWidget *secondaryVideoPlayer_;
    QPushButton *secondaryCameraButton_;

    QLabel *motionLabel_;
    Compass *compass_;
    AttitudeIndicator *attitudeIndicator_;
    DepthGauge *depthGauge_;
    BatteryStatusWidget *batteryStatusWidget_;
    HudAlertOverlay *alertOverlay_;
    HudNotificationStack *notificationStack_;
    QLabel *orientationLabel_;
    QTimer *hudTimer_;
    bool wasStable_ = false;
};

#endif // FULLSCREEN_VIDEO_WINDOW_H
