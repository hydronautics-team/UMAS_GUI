#ifndef FULLSCREEN_VIDEO_WINDOW_H
#define FULLSCREEN_VIDEO_WINDOW_H

#include <QWidget>
#include <QLabel>
#include <QKeyEvent>
#include <QTimer>
#include <QPixmap>
#include "uv_state.h"

class FullscreenVideoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FullscreenVideoWindow(UVState* uvState, QWidget* parent = nullptr);
    ~FullscreenVideoWindow();

    void setVideoWidget(QWidget* widget);

public slots:
    void drawFrame(const QPixmap& pixmap);

signals:
    void windowClosed();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void updateHudData();

private:
    UVState* uvState_;
    QLabel*  fullscreenVideoLabel_; // Холст для видео
    QWidget* videoPlayer_ = nullptr; // Указатель на видео-плеер

    QLabel*  depthLabel_;
    QLabel*  yawLabel_;
    QLabel*  alarmLabel_;
    QTimer*  hudTimer_;
};

#endif // FULLSCREEN_VIDEO_WINDOW_H