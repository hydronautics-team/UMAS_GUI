#ifndef IUSERINTERFACEDATA_H
#define IUSERINTERFACEDATA_H

#include "i_basic_data.h"
#include <QDebug>

class IUserInterfaceData : public IBasicData
{
    Q_OBJECT
signals:
    void displayText_toConsole(QString str);

public:
    IUserInterfaceData();

    void setFlagAH127C_pult(FlagAH127C_pult flagAH127C_pult);
    void setPowerMode(power_Mode mode);
    void setControlContoursFlags(e_StabilizationContours contour, bool value);
    void setCSMode(e_CSMode mode);
    void setModeSelection(bool mode);

    int                     getChecksumMsgAgentSend();
    int                     getChecksumMsgGuiSend();
    int                     getChecksumMsgGuiReceived();
    ControlContoursFlags    getControlContoursFlags();
    bool                    getModeSelection();
    FlagAH127C_bort         getFlagAH127C_bort();
    FlagAH127C_pult         getFlagAH127C_pult();
    power_Mode              getPowerMode();
    bool                    getCSMode();
    ControlData             getControlData();
    DataAH127C              getImuData();
    Header                  getHeader();
    AUVCurrentData          getAUVCurrentData();
    DataPressure            getDataPressure();
    DataUWB                 getDataUWB();
};

#endif // IUSERINTERFACEDATA_H
