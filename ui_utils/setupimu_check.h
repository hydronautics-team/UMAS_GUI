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

/*!
 * \brief SetupIMU_check class класс проверки настройки БСО.
 */
class SetupIMU_check : public QDialog
{
    Q_OBJECT

public:
    explicit SetupIMU_check(QWidget *parent = nullptr);
    ~SetupIMU_check();


private slots:
    /*!
     * \brief startCheck слот старта проверки.
     */
    void startCheck();
    /*!
     * \brief timer_setupIMU_check_timeStart слот окончания записи данных и
     *  и последующего построения графиков
     */
    void timer_setupIMU_check_timeStart();
    /*!
     * \brief updateUI_table слот заполнения таблицы.
     */
    void updateUI_table();
    /*!
     * \brief resetTable слот очистки таблицы.
     */
    void resetTable();
    /*!
     * \brief stopCheck слот окончания проверки и стерания данных.
     */
    void stopCheck();


private:
    /*!
     * \brief ui указатель на форму проверки настройки БСО.
     */
    Ui::SetupIMU_check  *ui;

    /*!
     * \brief timer_setupIMU_check таймер окончания записи данных и
     *  и последующего построения графиков
     */
    QTimer *timer_setupIMU_check;
    /*!
     * \brief timer_updateUI_check таймер заполнения таблицы.
     */
    QTimer *timer_updateUI_check;

    /*!
     * \brief time длительность записи.
     */
    float time;
    /*!
     * \brief periodicity периодичность записи данныз
     */
    float periodicity;
    /*!
     * \brief количество строк в таблице.
     */
    int i;
    /*!
     * \brief rowCount_beforeStart количество строк в таблице до начала записи.
     */
    int rowCount_beforeStart;

    /*!
     * \brief uv_interface получения данных
     */
    IUserInterfaceData uv_interface;

    /*!
     * \brief db база данных для хранения значений магнитометров.
     */
    DataBase *db;

    QChart *chart;
    QChartView *chartView;
    QSplineSeries *series;

    /*!
     * \brief createPlot метод построение графика.
     */
    void createPlot();
    /*!
     * \brief updateUI_setPlot метод повторного построение графика.
     */
    void updateUI_setPlot();
};

#endif // SETUPIMU_CHECK_H
