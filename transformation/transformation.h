#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include "uv_state.h"
#include <QtMath>
#include <QTimer>


class Transformation : public QObject
{
    Q_OBJECT
public:
    Transformation();

    double gradToRadian(double grad);
    void integrate(double &input, double &output, double &prevOutput, double dt);
public slots:
    void getdpsi(DataAH127C imuData);
public:
    QTimer timer;
    double yaw[2];





};

#endif // TRANSFORMATION_H
