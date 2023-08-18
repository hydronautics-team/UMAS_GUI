#ifndef IUSERINTERFACEDATA_H
#define IUSERINTERFACEDATA_H

#include "i_basic_data.h"
#include <QDebug>


class IUserInterfaceData : public IBasicData
{
public:
    IUserInterfaceData();

    void setFlagAH127C_pult(FlagAH127C_pult flagAH127C_pult);
    void setPowerMode(power_Mode mode);
    void setControlContoursFlags(e_StabilizationContours contour, bool value);
    void setCSMode(e_CSMode mode);
    void setModeSelection(bool mode);

    FlagAH127C_bort getFlagAH127C_bort();
    power_Mode      getPowerMode();
    bool            getCSMode();
    ControlData     getControlData();
    DataAH127C      getImuData();
    Header          getHeader();
    AUVCurrentData  getAUVCurrentData();
    DataPressure    getDataPressure();
    DataUWB         getDataUWB();
};

#endif // IUSERINTERFACEDATA_H
