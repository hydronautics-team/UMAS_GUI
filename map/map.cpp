#include "map.h"
#include "ui_map.h"
#include "math.h"
#include <QDebug>

Map::Map(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Map)
{
    ui->setupUi(this);

    db = new DataBase("list_uwb.db", "Location_UWB", "Color", "x", "y");
    db->connectToDataBase();
    db->createTable(ui->tableWidget_listUWB);

    range[0] = 1;
    range[1] = 1;
    range[2] = 1;

    createPlot();

    connect(ui->pushButton_addUWB, &QPushButton::clicked,
            this, &Map::addRowUWB);
    connect(ui->pushButton_updateLocationUWB, &QPushButton::clicked,
            this, &Map::updateLocationUWB);
}

void Map::createPlot()
{
    beacon = new QScatterSeries();

    agentCoords = new QScatterSeries();

    circle1 = new QLineSeries();
    circle1->setColor(QColor(0, 0, 255));
    circle2 = new QLineSeries();
    circle2->setColor(QColor(0, 255, 0));
    circle3 = new QLineSeries();
    circle3->setColor(QColor(255, 165, 0));

    chart = new QChart();
    chart->addSeries(beacon);
    chart->addSeries(circle1);
    chart->addSeries(circle2);
    chart->addSeries(circle3);
    chart->addSeries(agentCoords);
    chart->createDefaultAxes();
    chart->axes(Qt::Vertical).first()->setRange(-50,100);
    chart->axes(Qt::Vertical).first()->setTitleText("Y, см");
    chart->axes(Qt::Horizontal).first()->setRange(-50,150);
    chart->axes(Qt::Horizontal).first()->setTitleText("X, см");
    chartView = new QChartView(chart);

    ui->verticalLayout_map->addWidget(chartView);

    connect(ui->pushButton_scaling, &QPushButton::toggled,
            this, &Map::plotScaling);
}

void Map::updateUi_map(DataUWB dataUWB)
{
    drawCurrentCoords(dataUWB.locationX, dataUWB.locationY);

    range[0] = dataUWB.distanceToBeacon[0];
    range[1] = dataUWB.distanceToBeacon[1];
    range[2] = dataUWB.distanceToBeacon[2];

    drawCircle(circle1, 0, range[0]);
    drawCircle(circle2, 1, range[1]);
    drawCircle(circle3, 2, range[2]);
}

void Map::addRowUWB()

{
    int countRows = ui->tableWidget_listUWB->rowCount();
    if (countRows == 2)
        ui->pushButton_addUWB->setEnabled(false);
    ui->tableWidget_listUWB->insertRow(countRows);
    ui->tableWidget_listUWB->setItem(countRows, 0, new QTableWidgetItem(color.at(countRows)));
    ui->tableWidget_listUWB->resizeColumnsToContents();
}

void Map::updateLocationUWB()
{
    int countRows = ui->tableWidget_listUWB->rowCount();
    for (int row = 0; row < countRows; row++)
    {
        QTableWidgetItem *item_color = ui->tableWidget_listUWB->item(row, 0);
        QTableWidgetItem *item_x = ui->tableWidget_listUWB->item(row, 1);
        QTableWidgetItem *item_y = ui->tableWidget_listUWB->item(row, 2);
        if (item_x && item_y) {
            x[row] = item_x->text().toInt(nullptr, 10);
            y[row] = item_y->text().toInt(nullptr, 10);
            db->inserIntoDeviceTable(item_color->text(), x[row], y[row]);
        }
    }
    emit sendLocationUWB(&x[0],&y[0]);
    beacon->clear();
    drawCircle(circle1, 0, range[0]);
    drawCircle(circle2, 1, range[1]);
    drawCircle(circle3, 2, range[2]);
}

void Map::plotScaling(bool state)
{
    if (state)
        chartView->setRubberBand(QChartView::RectangleRubberBand);
    else
        chartView->setRubberBand(QChartView::NoRubberBand);

}

void Map::drawCircle(QLineSeries *circle, int index, double R)
{
    circle->clear();

    beacon->append(x[index],y[index]);

    for(int i=0; i<=360; i++){
        double alpha = i*M_PI/180;
        double plot_x = x[index]+R*cos(alpha);
        double plot_y = y[index]+R*sin(alpha);
        circle->append(plot_x,plot_y);
    }
}

void Map::drawCurrentCoords(double x, double y)
{
    agentCoords->clear();
    agentCoords->append(x,y);
}

Map::~Map()
{
    delete ui;
}
