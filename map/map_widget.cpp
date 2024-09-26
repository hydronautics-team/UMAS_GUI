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

void MapWidget::addPoint(const QGeoCoordinate &coordinate)
{
    QVariantMap point;
    point["latitude"] = coordinate.latitude();
    point["longitude"] = coordinate.longitude();

    // Передаем точку в QML
    QMetaObject::invokeMethod(m_quickView->rootObject(), "addMapPoint", Q_ARG(QVariant, QVariant::fromValue(point)));
}

void MapWidget::addLine(const QVector<QGeoCoordinate> &coordinates)
{
    QVariantList path;
    for (const QGeoCoordinate &coord : coordinates) {
        QVariantMap point;
        point["latitude"] = coord.latitude();
        point["longitude"] = coord.longitude();
        path.append(point);
    }

    // Передаем линию в QML
    QMetaObject::invokeMethod(m_quickView->rootObject(), "addMapLine", Q_ARG(QVariant, QVariant::fromValue(path)));
}

