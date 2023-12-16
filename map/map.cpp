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
    beacon1 = new QScatterSeries();
    beacon2 = new QScatterSeries();
    beacon3 = new QScatterSeries();

    agent1Coords = new QScatterSeries();
    agent2Coords = new QScatterSeries();
    missionPlanning_goto_goal = new QScatterSeries();
    missionPlanning_goto_goal->setColor(QColor(255, 51, 153));

    circle1 = new QLineSeries();
    circle1->setColor(QColor(0, 0, 255));
    circle2 = new QLineSeries();
    circle2->setColor(QColor(0, 255, 0));
    circle3 = new QLineSeries();
    circle3->setColor(QColor(255, 165, 0));
    missionPlanning_goto_goal_radius = new QLineSeries();
    missionPlanning_goto_goal_radius->setColor(QColor(255, 51, 153));
    missionPlanning_goto_traj = new QLineSeries();
    missionPlanning_goto_traj->setColor(QColor(255, 0, 0));

    chart = new QChart();
    chart->addSeries(beacon1);
    chart->addSeries(beacon2);
    chart->addSeries(beacon3);
    chart->addSeries(circle1);
    chart->addSeries(circle2);
    chart->addSeries(circle3);
    chart->addSeries(missionPlanning_goto_goal_radius);
    chart->addSeries(missionPlanning_goto_traj);
    chart->addSeries(agent1Coords);
    chart->addSeries(agent2Coords);
    chart->addSeries(missionPlanning_goto_goal);
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
    drawCurrentCoords(agent1Coords, dataUWB.locationX, dataUWB.locationY);

    range[0] = dataUWB.distanceToBeacon[0];
    range[1] = dataUWB.distanceToBeacon[1];
    range[2] = dataUWB.distanceToBeacon[2];

    drawCircle(circle1, beacon1, x[0], y[0], range[0]);
    drawCircle(circle2, beacon2, x[1], y[1], range[1]);
    drawCircle(circle3, beacon3, x[2], y[2], range[2]);
}

void Map::updateUi_map2(DataUWB dataUWB)
{
    drawCurrentCoords(agent2Coords, dataUWB.locationX, dataUWB.locationY);
}

void Map::updateUi_missionPlanning_goto_goal(double x, double y, double r)
{
//    drawCurrentCoords(missionPlanning_goto_goal, x, y);
    drawCircle(missionPlanning_goto_goal_radius, missionPlanning_goto_goal, x, y, r);
}

void Map::updateUi_missionPlanning_goto_goal_clear()
{
    missionPlanning_goto_goal->clear();
    missionPlanning_goto_goal_radius->clear();
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
    drawCircle(circle1, beacon1, x[0], y[0], range[0]);
    drawCircle(circle2, beacon2, x[1], y[1], range[1]);
    drawCircle(circle3, beacon3, x[2], y[2], range[2]);
}

void Map::plotScaling(bool state)
{
    if (state)
        chartView->setRubberBand(QChartView::RectangleRubberBand);
    else
        chartView->setRubberBand(QChartView::NoRubberBand);

}

void Map::updateUi_missionPlanning_goto_traj_onoff()
{
    if (flag_traj)
        flag_traj = 0;
    else
        flag_traj = 1;
}

void Map::updateUi_missionPlanning_goto_traj_clear()
{
    missionPlanning_goto_traj->clear();
}

void Map::drawCircle(QLineSeries *circle, QScatterSeries *point, double x, double y, double R)
{
    circle->clear();
    point->clear();

    point->append(x,y);

    for(int i=0; i<=360; i++){
        double alpha = i*M_PI/180;
        double plot_x = x+R*cos(alpha);
        double plot_y = y+R*sin(alpha);
        circle->append(plot_x,plot_y);
    }
}

void Map::drawCurrentCoords(QScatterSeries *agentCoords, double x, double y)
{
    agentCoords->clear();
    agentCoords->append(x,y);
    if (flag_traj)
        missionPlanning_goto_traj->append(x,y);
}

Map::~Map()
{
    delete ui;
}
