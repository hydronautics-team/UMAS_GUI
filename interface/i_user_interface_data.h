#ifndef IUSERINTERFACEDATA_H
#define IUSERINTERFACEDATA_H

#include "i_basic_data.h"
#include <QDebug>


class IUserInterfaceData : public IBasicData
{
public:
    IUserInterfaceData();

    void setControlContoursFlags(e_StabilizationContours contour, bool value);
    void setCSMode(e_CSMode mode);

    ControlData     getControlData();
    DataAH127C      getImuData();
    Header          getHeader();
    AUVCurrentData  getAUVCurrentData();
    DataPressure    getDataPressure();
    DataUWB         getDataUWB();
};

#endif // IUSERINTERFACEDATA_H
