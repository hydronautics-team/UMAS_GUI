#include "map_widget.h"
#include "ui_map_widget.h"

#include <QQuickView>
#include <QQmlContext>
#include <QVBoxLayout>
#include <QQuickItem>

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

    setLayout(layout);
}

MapWidget::~MapWidget()
{
    delete m_quickView;
}

void MapWidget::addPoint(double latitude, double longitude)
{
    QVariantMap params;
    params["latitude"] = latitude;
    params["longitude"] = longitude;

    QMetaObject::invokeMethod(m_quickView->rootObject(), "addMapMarker", Q_ARG(QVariant, QVariant::fromValue(params)));
}

void MapWidget::addLine(const QVector<QGeoCoordinate> &points)
{
    QVariantList path;
    for (const auto &point : points) {
        QVariantMap coords;
        coords["latitude"] = point.latitude();
        coords["longitude"] = point.longitude();
        path.append(coords);
    }

    QMetaObject::invokeMethod(m_quickView->rootObject(), "addMapLine", Q_ARG(QVariant, QVariant::fromValue(path)));
}
