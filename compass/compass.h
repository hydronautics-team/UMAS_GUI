#ifndef COMPASS_H
#define COMPASS_H

#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <cmath>
#include <ui_compass.h>


class Compass : public QFrame, private Ui::Compass{
Q_OBJECT

public:
    explicit Compass(QWidget *parent = nullptr);

public slots:
    void setYaw(double yawNew);

protected:
    void paintEvent(QPaintEvent *e);

private:
    double yaw;
    QPoint arrowCompass[6] = {
        QPoint(0, 50),
        QPoint(0, -50),
        QPoint(3, -40),
        QPoint(0, -50),
        QPoint(-3, -40),
        QPoint(0, -50),
    };
    QPoint arrowDesirable[3] = {
        QPoint(0, -70),
        QPoint(5, -85),
        QPoint(-5, -85),
        //QPoint(0, -50)
    };
};

#endif // COMPASS_H
