#include "map_widget.h"
#include "ui_map_widget.h"

#include <QQuickView>
#include <QQmlContext>
#include <QVBoxLayout>
#include <QQuickItem>


// Радиус Земли в метрах
constexpr double EARTH_RADIUS = 6378137.0;

MapWidget::MapWidget(QWidget *parent)
    : QWidget(parent),
      m_quickView(new QQuickView())
{
    // Устанавливаем путь к QML файлу
    m_quickView->setSource(QUrl(QStringLiteral("qrc:/map/map.qml")));

    // Встраиваем QQuickView в QWidget
    m_container = QWidget::createWindowContainer(m_quickView, this);
    m_container->setMinimumSize(800, 600);
    m_container->setFocusPolicy(Qt::TabFocus);

    // Устанавливаем layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_container);

    connect(m_quickView->rootObject(), SIGNAL(pointsRetrieved(QVariant)), this, SLOT(onPointsRetrieved(QVariant)));
    // Подключаем сигнал QML к C++ слоту
    connect(m_quickView->rootObject(), SIGNAL(pointClicked(double, double)),
            this, SLOT(onPointClicked(double, double)));

    setLayout(layout);
}

MapWidget::~MapWidget()
{
    delete m_quickView;
}

void MapWidget::addPoint(const QGeoCoordinate &coordinate)
{
    QVariantMap point;
    point["latitude"] = coordinate.latitude();
    point["longitude"] = coordinate.longitude();

    // Передаем точку в QML
    QMetaObject::invokeMethod(m_quickView->rootObject(), "addMapPoint", Q_ARG(QVariant, QVariant::fromValue(point)));
}

void MapWidget::addLine(const QVector<QPointF> &coordinates)
{
    if (coordinates.isEmpty()) {
        qDebug() << "No coordinates provided to draw a line.";
        return; // Не делаем ничего, если координат нет
    }

    QVariantList path;
    for (const QPointF &meters : coordinates) {
        // Переводим из метров в широту и долготу
        QGeoCoordinate convertedCoord = metersToLatLon(meters);

        // Создаем точку для QML
        QVariantMap point;
        point["latitude"] = convertedCoord.latitude();
        point["longitude"] = convertedCoord.longitude();
        path.append(point);

        // Логирование
        qDebug() << "Adding point in meters: X =" << meters.x() << ", Y =" << meters.y();
        qDebug() << "Converted to: latitude =" << convertedCoord.latitude() << ", longitude =" << convertedCoord.longitude();
    }

    // Передаем линию в QML
    QMetaObject::invokeMethod(m_quickView->rootObject(), "addMapLine", Q_ARG(QVariant, QVariant::fromValue(path)));

    qDebug() << "Line added with" << coordinates.size() << "points.";
}

void MapWidget::toggleAddPointMode_for_cpp(bool checked)
{
    if (checked) {
        if (m_canAddPoints != 1)
            m_canAddPoints = 1;
    } else {
        m_canAddPoints = 0;
    }

    // Вы можете также передать это состояние в QML, если это нужно
    QMetaObject::invokeMethod(m_quickView->rootObject(), "setAddPointMode", Q_ARG(QVariant, m_canAddPoints));
}

void MapWidget::toggleAddPointMode_for_marker(bool checked)
{
    if (checked) {
        if (m_canAddPoints != 2)
            m_canAddPoints = 2;
    } else {
        m_canAddPoints = 0;
    }

    // Вы можете также передать это состояние в QML, если это нужно
    QMetaObject::invokeMethod(m_quickView->rootObject(), "setAddPointMode", Q_ARG(QVariant, m_canAddPoints));
}

void MapWidget::toggleAddPointMode_for_goto_point(bool checked)
{
    if (checked) {
        if (m_canAddPoints != 3)
            m_canAddPoints = 3;
    } else {
        m_canAddPoints = 0;
    }

    // Вы можете также передать это состояние в QML, если это нужно
    QMetaObject::invokeMethod(m_quickView->rootObject(), "setAddPointMode", Q_ARG(QVariant, m_canAddPoints));
}

void MapWidget::toggleAddPointMode_for_go_circle_point(bool checked)
{
    if (checked) {
        if (m_canAddPoints != 4)
            m_canAddPoints = 4;
    } else {
        m_canAddPoints = 0;
    }

    // Вы можете также передать это состояние в QML, если это нужно
    QMetaObject::invokeMethod(m_quickView->rootObject(), "setAddPointMode", Q_ARG(QVariant, m_canAddPoints));
}


void MapWidget::clearMapItems()
{
    // Вызываем метод в QML для очистки точек и линий
    QMetaObject::invokeMethod(m_quickView->rootObject(), "clearMapItems");
}

void MapWidget::getAllPoints() {
    // Инициализируем переменную для получения значений
    QVariant returnedPoints;

    // Вызываем метод в QML для получения всех точек и сохраняем результат
    QMetaObject::invokeMethod(m_quickView->rootObject(), "getAllPoints", Q_RETURN_ARG(QVariant, returnedPoints));

    // Теперь вы можете использовать returnedPoints для дальнейшей обработки
    points = returnedPoints; // Сохраняем результат в переменную класса, если нужно
    qDebug() << "Received points:" << points; // Выводим полученные точки в консоль
}


void MapWidget::onPointsRetrieved(const QVariant &points) {
    QVariantList pointList = points.toList();
    for (const QVariant &point : pointList) {
        double latitude = point.toMap().value("latitude").toDouble();
        double longitude = point.toMap().value("longitude").toDouble();

        // Переводим в метры
        double x, y;
        convertLatLonToMeters(latitude, longitude, x, y);
        // Выводим результат
        qDebug() << "Point in meters: X = " << x << ", Y = " << y;
    }
}

void MapWidget::onPointClicked(double latitude, double longitude) {
    if (m_canAddPoints == 1)
    {
        // Преобразуем широту и долготу в объект QGeoCoordinate
        QGeoCoordinate coord(latitude, longitude);

        // Используем метод latLonToMeters для преобразования в метры
        QPointF meters = latLonToMeters(coord);

        // Получаем координаты X и Y
        double x = meters.x();
        double y = meters.y();

        // Логируем координаты или выполняем дальнейшую обработку
        qDebug() << "Clicked point: Latitude =" << latitude << ", Longitude =" << longitude;
        qDebug() << "Converted to meters: X =" << x << ", Y =" << y;

        // Дополнительно можно использовать сигнал для передачи координат в другие части программы
        emit signal_addPointToTable(x, y);
    } else if (m_canAddPoints == 2) {
        signal_addMarker_to_gui(latitude, longitude);
    } else if (m_canAddPoints == 3 || m_canAddPoints == 4) {
        signal_addPoint_to_gui(latitude, longitude);
    }
}



// Преобразование широты и долготы в метры
void MapWidget::convertLatLonToMeters(double latitude, double longitude, double& x, double& y) {
    // Преобразуем широту и долготу в радианы
    double latRad = latitude * M_PI / 180.0;
    double lonRad = longitude * M_PI / 180.0;

    // Центрируем на нулевом меридиане и экваторе
    x = lonRad * EARTH_RADIUS * cos(latRad);
    y = latRad * EARTH_RADIUS;
}

void MapWidget::convertMetersToLatLon(double x, double y, double& latitude, double& longitude) {
    // Преобразование из метров обратно в радианы
    double latRad = y / EARTH_RADIUS;
    double lonRad = x / (EARTH_RADIUS * cos(latRad));

    // Преобразование радианов в градусы
    latitude = latRad * 180.0 / M_PI;
    longitude = lonRad * 180.0 / M_PI;
}

// Перевод широты и долготы в метры
QPointF MapWidget::latLonToMeters(const QGeoCoordinate& coord) const {
    double lat0 = qDegreesToRadians(origin.latitude());
    double lon0 = qDegreesToRadians(origin.longitude());

    double lat = qDegreesToRadians(coord.latitude());
    double lon = qDegreesToRadians(coord.longitude());

    // Вычисление изменения координат
    double x = (lon - lon0) * R * qCos(lat0);
    double y = (lat - lat0) * R;

    return QPointF(x, y);
}

// Перевод из метров обратно в широту и долготу
QGeoCoordinate MapWidget::metersToLatLon(const QPointF& meters) const {
    double lat0 = qDegreesToRadians(origin.latitude());
    double lon0 = qDegreesToRadians(origin.longitude());

    // Обратное преобразование
    double deltaLat = meters.y() / R;
    double deltaLon = meters.x() / (R * qCos(lat0));

    double lat = lat0 + deltaLat;
    double lon = lon0 + deltaLon;

    return QGeoCoordinate(qRadiansToDegrees(lat), qRadiansToDegrees(lon));
}

// Метод для установки начальной точки
void MapWidget::setOrigin(const QGeoCoordinate& newOrigin) {
    origin = newOrigin;
}

void MapWidget::setMarker(const QGeoCoordinate &coordinate) {
    QVariantMap marker;
    marker["latitude"] = coordinate.latitude();
    marker["longitude"] = coordinate.longitude();

    // Передаем координаты нового маркера в QML
    QMetaObject::invokeMethod(m_quickView->rootObject(), "setMarker", Q_ARG(QVariant, QVariant::fromValue(marker)));

    setOrigin(coordinate);
}

void MapWidget::setCurrentPos(double latitude, double longitude) {
    // Вызываем метод в QML для обновления позиции агента
    QMetaObject::invokeMethod(m_quickView->rootObject(), "addAgentPosition",
                              Q_ARG(QVariant, latitude),
                              Q_ARG(QVariant, longitude));
}

// слот для установки радиуса для движения по окружности
void MapWidget::setRadius_circle(const QString &text)
{
    // Преобразуем строку в double
    double radius = text.toDouble();  // Преобразуем строку в double

    // Вызываем метод в QML для установки радиуса окружности
    QMetaObject::invokeMethod(m_quickView->rootObject(), "setCircleRadius",
                              Q_ARG(QVariant, radius));
}
