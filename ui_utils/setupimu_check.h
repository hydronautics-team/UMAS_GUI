#ifndef SETUPIMU_CHECK_H
#define SETUPIMU_CHECK_H

#include <QDialog>
#include <QString>
#include <QTimer>
#include <QTime>


#include <QSqlQuery>


#include <QtCharts/QScatterSeries>
#include <QtCharts/QLegendMarker>
#include <QtCharts/QChartView>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QAbstractSeries>
#include <QtCharts/QChart>
#include <QtWidgets/QGraphicsView>

#include <QValueAxis>


#include <QtCharts/QSplineSeries>

#include <QDebug>

#include "database.h"
#include "i_user_interface_data.h"

namespace Ui {
class SetupIMU_check;
}

class SetupIMU_check : public QDialog
{
    Q_OBJECT

public:
    explicit SetupIMU_check(QWidget *parent = nullptr);
    ~SetupIMU_check();


private slots:
    void startCheck();
    void timer_setupIMU_check_timeStart();
    void updateUI_table();
    void resetTable();
    void stopCheck();


private:
    Ui::SetupIMU_check  *ui;

    QTimer *timer_setupIMU_check;
    QTimer *timer_updateUI_check;

    float time;
    float periodicity;
    int count;
    int i;
    int rowCount_beforeStart;

    IUserInterfaceData uv_interface;

    DataBase *db;

    QChart *chart;
    QChartView *chartView;
    QSplineSeries *series;


    void createPlot();
    void createUI(const QStringList &headers);
    void updateUI_setPlot();
};

#endif // SETUPIMU_CHECK_H
