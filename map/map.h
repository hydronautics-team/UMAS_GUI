#ifndef MAP_H
#define MAP_H

#include <QWidget>

#include <QtCharts/QScatterSeries>
#include <QtCharts/QLegendMarker>
#include <QtCharts/QChartView>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QAbstractSeries>
#include <QtCharts/QChart>
#include <QtWidgets/QGraphicsView>

namespace Ui {
class Map;
}

class Map : public QWidget
{
    Q_OBJECT

public:
    explicit Map(QWidget *parent = nullptr);
    ~Map();

private:
    Ui::Map *ui;
};

#endif // MAP_H
