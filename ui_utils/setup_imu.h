#ifndef SETUP_IMU_H
#define SETUP_IMU_H

#include <QDialog>
#include "setupimu_start.h"
#include "ui_setupimu_start.h"

namespace Ui {
class SetupIMU;
}

class SetupIMU : public QDialog
{
    Q_OBJECT

public:
    explicit SetupIMU(QWidget *parent = nullptr);
    ~SetupIMU();

public slots:
    void setupIMU_nextStep1();


private:
    Ui::SetupIMU *ui;
};

#endif // SETUP_IMU_H
