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
    void toggleAddPointMode_for_cpp(bool checked); // Метод для переключения режима добавления точек для покрытия
    void toggleAddPointMode_for_marker(bool checked); // Метод для переключения режима добавления точек для маркера
    void toggleAddPointMode_for_goto_point(bool checked); // Метод для переключения режима добавления точек выхода в точку
    void toggleAddPointMode_for_go_circle_point(bool checked);
    void clearMapItems(); // Метод для очистки всех точек и линий
    void getAllPoints(); // Метод для получения всех точек
    void onPointsRetrieved(const QVariant &points); // Слот для обработки полученных точек
    void onPointClicked(double latitude, double longitude); // Слот для обработки кликов
    void setMarker(const QGeoCoordinate &coordinate); // Слот для установки репера
    void setCurrentPos(double latitude, double longitude); // слот для установки текущей позиции
    void setRadius_circle(const QString &text); // слот для установки радиуса для движения по окружности

signals:
    void signal_addPointToTable(qreal x, qreal y);
    void signal_addMarker_to_gui(double x, double y);
    void signal_addPoint_to_gui(double latitude, double longitude);


    void pointClicked(double latitude, double longitude);

private:
    QQuickView *m_quickView;
    QWidget *m_container;
    int m_canAddPoints = 0; // Переменная для контроля возможности добавления точек
    QVariant points; // Переменная для хранения полученных координат

    QGeoCoordinate origin = QGeoCoordinate(55.76547524252744, 37.8163097457828); // базовая точка (центр карты)
    const double R = 6378137.0; // Средний радиус Земли в метрах


};

#endif // MAP_WIDGET_H

