#ifndef MAP_H
#define MAP_H

#include <QObject>

#include <QtCharts/QScatterSeries>
#include <QtCharts/QLegendMarker>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtCore/QtMath>

class Map : public QObject
{
    Q_OBJECT
public:
    explicit Map(QObject *parent = nullptr);

signals:

};

#endif // MAP_H
