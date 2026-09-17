#ifndef POWER_SYSTEM_H
#define POWER_SYSTEM_H

#include <QWidget>
#include <QButtonGroup>
#include <QTimer>


namespace Ui {
class PowerSystem;
}

class PowerSystem : public QWidget
{
    Q_OBJECT

public:
    explicit PowerSystem(QWidget *parent = nullptr);
    ~PowerSystem();

private:
    Ui::PowerSystem *ui;

    // таймеры
    QTimer *timer_off_powerMode_5;

    power_Mode before_powerMode;

    void setBottom_powerMode();

private slots:
    void pushButton_on_powerMode_2();
    void pushButton_on_powerMode_3();
    void pushButton_on_powerMode_5();

    void off_powerMode_5();


};

#endif // POWER_SYSTEM_H
