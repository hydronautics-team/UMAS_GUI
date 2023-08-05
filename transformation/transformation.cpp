#include "transformation.h"
#include <QDebug>

Transformation::Transformation()
{
    //connect(&timer, &QTimer::timeout, this, &Transformation::getdpsi);
    timer.start(50);
    yaw[0]=yaw[1]=0;
}

double Transformation::gradToRadian(double grad) {
    return grad*3.14/180;
}

void Transformation::integrate(double &input, double &output, double &prevOutput, double dt) {
    output = prevOutput + dt*input;
    prevOutput = output;
}

void Transformation::getdpsi(DataAH127C imuData)
{
    float wx = imuData.X_rate;
    float wy = imuData.Y_rate;
    float wz = imuData.Z_rate;

    float tetta = gradToRadian(imuData.pitch);
    float gamma = gradToRadian(imuData.roll);
    double dPsi = (wy) * sin(gamma)/cos(tetta)  + cos(gamma) * wz/cos(tetta);
    qDebug()<<"dPsi = "<<dPsi <<" wz = "<<wz<<" cos (gamma) = "<<cos(gamma) <<"tan(teta) "<<tan(tetta);
    integrate(dPsi,yaw[0],yaw[1],0.05);

}

