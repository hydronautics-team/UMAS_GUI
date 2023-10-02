#ifndef SETUP_IMU_H
#define SETUP_IMU_H

#include <QDialog>
#include "setupimu_start.h"
#include "ui_setupimu_start.h"
#include "i_user_interface_data.h"

namespace Ui {
class SetupIMU;
}

/*!
 * \brief SetupIMU class класс UI формы подготовки к настройке БСО.
 */
class SetupIMU : public QDialog
{
    Q_OBJECT

public:
    explicit SetupIMU(QWidget *parent = nullptr);
    ~SetupIMU();

public slots:
    /*!
     * \brief setupIMU_nextStep1 слот открытия окна старта настройки БСО.
     */
    void setupIMU_nextStep1();


private:
    /*!
     * \brief ui окно этой формы.
     */
    Ui::SetupIMU *ui;
};

#endif // SETUP_IMU_H
