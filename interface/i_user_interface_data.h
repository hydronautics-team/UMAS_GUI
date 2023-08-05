#ifndef IUSERINTERFACEDATA_H
#define IUSERINTERFACEDATA_H

#include "i_basic_data.h"
#include <QDebug>


class IUserInterfaceData : public IBasicData
{
public:
    IUserInterfaceData();

    ControlData getControlData();
    void setControlContoursFlags(e_StabilizationContours contour, bool value);
    void setCSMode(e_CSMode mode);
//    ImuData getImuData();
};

#endif // IUSERINTERFACEDATA_H
