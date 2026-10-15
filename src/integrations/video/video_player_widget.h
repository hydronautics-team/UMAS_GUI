#ifndef VIDEO_PLAYER_WIDGET_H
#define VIDEO_PLAYER_WIDGET_H

#include <QLabel>
#include <QPixmap>
#include <QWidget>
#include <gst/app/gstappsink.h>
#include <gst/gst.h>

class VideoPlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPlayerWidget(QWidget *parent = nullptr);
    ~VideoPlayerWidget();

    void startStream(const QString &portStr);
    void stopStream();
    void setFullscreenPresentation(bool fullscreen);

signals:
    void frameReceived(const QPixmap &pixmap);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    static GstFlowReturn newSampleCallback(GstAppSink *appsink, gpointer userData);

    QLabel *titleLabel_;
    QLabel *imageLabel_;
    QLabel *statusLabel_;
    QPixmap currentPixmap_;

    GstElement *gstPipeline_ = nullptr;
};

#endif // VIDEO_PLAYER_WIDGET_H
