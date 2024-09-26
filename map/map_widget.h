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
    void addPoint(const QGeoCoordinate &coordinate); // Слот для добавления точки
    void addLine(const QVector<QGeoCoordinate> &coordinates); // Слот для добавления линии


private:
    QQuickView *m_quickView;
    QWidget *m_container;
};

#endif // MAP_WIDGET_H

