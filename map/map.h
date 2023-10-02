#ifndef MAP_H
#define MAP_H

#include <QWidget>
#include <QStringList>

#include <QtCharts/QScatterSeries>
#include <QtCharts/QLegendMarker>
#include <QtCharts/QChartView>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QAbstractSeries>
#include <QtCharts/QChart>
#include <QtWidgets/QGraphicsView>
#include <QLineSeries>
#include <QColor>

#include "uv_state.h"
#include "database.h"

namespace Ui {
class Map;
}
//using namespace QtCharts;

class Map : public QWidget
{
    Q_OBJECT

public:
    explicit Map(QWidget *parent = nullptr);
    ~Map();

    void drawCircle(QLineSeries *circle, int index, double R);
    void drawCurrentCoords(double x, double y);

protected:
    double x[3];
    double y[3];
    double range[3];



    Ui::Map *ui;
    QChartView *chartView = nullptr;
    QChart *chart = nullptr;
    QScatterSeries *beacon = nullptr;
    QScatterSeries *agentCoords = nullptr;
    QLineSeries *circle1 = nullptr;
    QLineSeries *circle2 = nullptr;
    QLineSeries *circle3 = nullptr;

    QStringList color = { "Синий", "Зеленый", "Оранжевый" };

private:
    DataBase *db;

    void createPlot();

public slots:
    void updateUi_map(DataUWB dataUWB);

    void addRowUWB();
    void updateLocationUWB();
    void plotScaling(bool state);

signals:
    void sendLocationUWB(double *x, double *y);
};

#endif // MAP_H
