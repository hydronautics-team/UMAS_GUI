#ifndef SETUPIMU_START_H
#define SETUPIMU_START_H

#include <QDialog>
#include <QTimer>
#include <QDebug>

#include "i_user_interface_data.h"

namespace Ui {
class SetupIMU_start;
}

/*!
 * \brief SetupIMU_start class класс настройки БСО.
 */
class SetupIMU_start : public QDialog
{
    Q_OBJECT

public:
    /*!
     * \brief SetupIMU_start конструктор, в котором задаются начальные настройки
     *  интерфейса и подключаются слоты.
     */
    explicit SetupIMU_start(QWidget *parent = nullptr);
    ~SetupIMU_start();


private slots:
    /*!
     * \brief pushButton_startTimer1 слот старта настройки и запуска таймера.
     *  Срабатывает по нажатию кнопки.
     */
    void pushButton_startTimer1();
    /*!
     * \brief pushButton_startTimer2 слот старта второго таймера.
     */
    void pushButton_startTimer2();
    /*!
     * \brief updateUi_sec_t1 слот отчета первого таймера.
     */
    void updateUi_sec_t1();
    /*!
     * \brief updateUi_sec_t2 слот отчета второго таймера и сохранение результатов.
     */
    void updateUi_sec_t2();

    /*!
     * \brief isCheckedFlagBort_start слот проверки начала настройки от БСО.
     */
    void isCheckedFlagBort_start();
    /*!
     * \brief isCheckedFlagBort_end слот проверки сохранения данных настройки.
     */
    void isCheckedFlagBort_end();

signals:
    /*!
     * \brief on_t2 сигнал запуска второго таймера.
     */
    void on_t2();

private:
    /*!
     * \brief ui указатель на форму настройки БСО.
     */
    Ui::SetupIMU_start *ui;
    /*!
     * \brief uv_interface интерфейс получения приходящих на аппарат данных
     */
    IUserInterfaceData uv_interface;
    /*!
     * \brief flagAH127C_pult флаги старта и окончания настройки.
     */
    FlagAH127C_pult flagAH127C_pult;
    /*!
     * \brief flagAH127C_bort флаги подтверждения старта и сохранения данных
     *   после окончания настройки.
     */
    FlagAH127C_bort flagAH127C_bort;

    QTimer *updateTimerSec;
    QTimer *timer_checkFlag;

    int sec = 30;

};

#endif // SETUPIMU_START_H
