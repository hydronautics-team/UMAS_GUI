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

/*!
 * \brief Map class класс отображения данных об UWB и агентов.
 */
class Map : public QWidget
{
    Q_OBJECT

public:
    explicit Map(QWidget *parent = nullptr);
    ~Map();

    /*!
     * \brief drawCircle метод рисования расстояние от модуля до агента.
     * \param circle окружность.
     * \param R расстояние от модуля до агетна.
     */
    void drawCircle(QLineSeries *circle, QScatterSeries *point, double x, double y, double R);
    /*!
     * \brief drawCurrentCoords метод отображения агента.
     * \param x координата агента по оси X.
     * \param y координата агента по оси Y.
     */
    void drawCurrentCoords(QScatterSeries *agentCoords, double x, double y);

protected:
    /*!
     * \brief x координаты модулей по оси X.
     */
    double x[3];
    /*!
     * \brief y координаты модулей по оси Y.
     */
    double y[3];
    /*!
     * \brief range текущее расстояние от агетна до модуля.
     */
    double range[3];

    /*!
     * \brief ui указатель на форму map.ui
     */
    Ui::Map *ui;

    QChartView *chartView = nullptr;
    QChart *chart = nullptr;
    QScatterSeries *beacon1 = nullptr;
    QScatterSeries *beacon2 = nullptr;
    QScatterSeries *beacon3 = nullptr;
    QScatterSeries *agent1Coords = nullptr;
    QScatterSeries *agent2Coords = nullptr;
    QScatterSeries *missionPlanning_goto_goal = nullptr;
    QLineSeries *missionPlanning_goto_goal_radius = nullptr;
    QLineSeries *missionPlanning_goto_traj = nullptr;
    QLineSeries *circle1 = nullptr;
    QLineSeries *circle2 = nullptr;
    QLineSeries *circle3 = nullptr;
    int flag_traj=0;

    QStringList color = { "Синий", "Зеленый", "Оранжевый" };

private:
    DataBase *db;

    void createPlot();

public slots:
    /*!
     * \brief updateUi_map слот обновления данных о расположении UWB и агента.
     * \param dataUWB структура с информацией об расположении агента.
     */
    void updateUi_map(DataUWB dataUWB);
    void updateUi_map2(DataUWB dataUWB);
    void updateUi_missionPlanning_goto_goal(double x, double y, double r);
    void updateUi_missionPlanning_goto_goal_clear();

    /*!
     * \brief addRowUWB слот добавления строк и цветов модулей в таблицу.
     */
    void addRowUWB();
    /*!
     * \brief updateLocationUWB метод обновления по кнопке данных о UWB модулях.
     */
    void updateLocationUWB();
    /*!
     * \brief plotScaling слот переключения. масштабирования графика.
     * \param state состояние нажатия кнопки.
     */
    void plotScaling(bool state);

    void updateUi_missionPlanning_goto_traj_onoff();
    void updateUi_missionPlanning_goto_traj_clear();


signals:
    /*!
     * \brief sendLocationUWB сигнал отправки расположения UWB модулей
     * \param x координаты модулей по оси X.
     * \param y координаты модулей по оси Y.
     */
    void sendLocationUWB(double *x, double *y);
};

#endif // MAP_H
