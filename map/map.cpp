#include "map.h"
#include "ui_map.h"

Map::Map(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Map)
{
    ui->setupUi(this);

    auto series0 = new QScatterSeries;
    series0->setName("Агент - 1");
    series0->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series0->setMarkerSize(15.0);

    series0->append(3, 8);


    auto chart = new QChart;
    chart->addSeries(series0);
    chart->createDefaultAxes();
    chart->setDropShadowEnabled(false);

    chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

    QChartView *chartView = new QChartView(chart);

    ui->verticalLayout_map->addWidget(chartView);
}

Map::~Map()
{
    delete ui;
}
