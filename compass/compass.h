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
    /*!
     * \brief setYaw метод установки текущего значения компаса.
     * \param yawNew текущее значение компаса.
     */
    void setYaw(double yawNew);
    /*!
     * \brief setYawDesirable метод установки управляющего значения компаса.
     * \param yawDesirableNew управляющего значения компаса
     * \param YawFromIMU текущее значение курса.
     * \param mode ручной или
     */
    void setYawDesirable(double yawDesirableNew, double YawFromIMU, bool mode);

protected:
    void paintEvent(QPaintEvent *e);

private:
    double yaw;
    double yawDesirable;
    QPoint arrowCompass[6] = {
        QPoint(0, 50),
        QPoint(0, -50),
        QPoint(3, -40),
        QPoint(0, -50),
        QPoint(-3, -40),
        QPoint(0, -50),
    };
    QPoint arrowDesirable[4] = {
        QPoint(0, -70),
        QPoint(5, -85),
        QPoint(-5, -85),
        QPoint(0, -70)
    };
};

#endif // COMPASS_H
