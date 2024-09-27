#ifndef MAP_WIDGET_H
#define MAP_WIDGET_H

#include <QWidget>
#include <QGeoCoordinate>
#include <QVector>
#include <QVariantMap>
#include <cmath>
#include <QtMath>
#include <QPointF>

QT_FORWARD_DECLARE_CLASS(QQuickView)

class MapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget();
    void convertLatLonToMeters(double latitude, double longitude, double& x, double& y);
    void convertMetersToLatLon(double x, double y, double& latitude, double& longitude);

    // Перевод широты и долготы в метры
    QPointF latLonToMeters(const QGeoCoordinate& coord) const;

    // Перевод из метров обратно в широту и долготу
    QGeoCoordinate metersToLatLon(const QPointF& meters) const;

    void setOrigin(const QGeoCoordinate& newOrigin);



public slots:
    void addPoint(const QGeoCoordinate &coordinate); // Слот для добавления точки
    void addLine(const QVector<QPointF> &coordinates); // Слот для добавления линии
    void toggleAddPointMode(); // Метод для переключения режима добавления точек
    void clearMapItems(); // Метод для очистки всех точек и линий
    void getAllPoints(); // Метод для получения всех точек
    void onPointsRetrieved(const QVariant &points); // Слот для обработки полученных точек
    void onPointClicked(double latitude, double longitude); // Слот для обработки кликов


signals:
    void signal_addPointToTable(qreal x, qreal y);
    void pointClicked(double latitude, double longitude);

private:
    QQuickView *m_quickView;
    QWidget *m_container;
    bool m_canAddPoints = false; // Переменная для контроля возможности добавления точек
    QVariant points; // Переменная для хранения полученных координат

    QGeoCoordinate origin = QGeoCoordinate(55.7667, 37.6889); // базовая точка (центр карты)
    const double R = 6378137.0; // Средний радиус Земли в метрах


};

#endif // MAP_WIDGET_H

