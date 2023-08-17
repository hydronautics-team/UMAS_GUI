#ifndef SETUPIMU_START_H
#define SETUPIMU_START_H

#include <QDialog>
#include <QTimer>
#include <QDebug>

namespace Ui {
class SetupIMU_start;
}

class SetupIMU_start : public QDialog
{
    Q_OBJECT

public:
    explicit SetupIMU_start(QWidget *parent = nullptr);
    ~SetupIMU_start();

private slots:
    void pushButton_startTimer1();
    void pushButton_startTimer2();
    void updateUi_sec_t1();
    void updateUi_sec_t2();


signals:
    void on_t2();

private:
    Ui::SetupIMU_start *ui;
    QTimer *updateTimerSec;

    int sec = 30;

};

#endif // SETUPIMU_START_H
