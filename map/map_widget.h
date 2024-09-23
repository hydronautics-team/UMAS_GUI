#ifndef MAP_WIDGET_H
#define MAP_WIDGET_H

#include <QWidget>
#include <QGeoCoordinate>
#include <QVector>
#include <QVariantMap>

QT_FORWARD_DECLARE_CLASS(QQuickView)

class MapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget();

public slots:
    void addPoint(double latitude, double longitude);
    void addLine(const QVector<QGeoCoordinate> &points);

private:
    QQuickView *m_quickView;
    QWidget *m_container;
};

#endif // MAP_WIDGET_H

