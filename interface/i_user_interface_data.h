#ifndef IUSERINTERFACEDATA_H
#define IUSERINTERFACEDATA_H

#include "i_basic_data.h"
#include <QDebug>


/*!
 * \brief IUserInterfaceData class класс для взаимодействия глобальной переменной
 *  состояний с главной формой.
 */
class IUserInterfaceData : public IBasicData
{
    Q_OBJECT
signals:
    void displayText_toConsole(QString str);

public:
    IUserInterfaceData();

    void    setCurrentAgent(int newAgent);
    void    setFlagAH127C_pult(FlagAH127C_pult flagAH127C_pult);
    void    setPowerMode(power_Mode mode);
    void    setControlContoursFlags(e_StabilizationContours contour, bool value);
    void    setCSMode(e_CSMode mode);
    void    setModeSelection(bool mode);
    void    setMissionControl(mission_Control missionControl);
    void    setID_mission_AUV(quint8 ID_mission_AUV);
    void    setReper(CoordinatePoint reper);
    void    setMissionFromPult(mission_List missionListFromPult);
    void    setMissionParam(MissionParam mission_param);



    int                     getCurrentAgent();
    int                     getChecksumMsgAgentSend();
    int                     getChecksumMsgGuiSend();
    int                     getChecksumMsgGuiReceived();
    ControlContoursFlags    getControlContoursFlags();
    bool                    getModeSelection();
    FlagAH127C_bort         getFlagAH127C_bort();
    FlagAH127C_pult         getFlagAH127C_pult();
    power_Mode              getPowerMode();
    e_CSMode                getCSMode();
    ControlData             getControlData();
    DataAH127C              getImuData();
    Header                  getHeader();
    AUVCurrentData          getAUVCurrentData();
    mission_Status          getMissionStatus();
    DataGANS                getDataGANS();
    GPS_angular             getAngularGPS();
    GPS_coordinate          getCoordinateGPS();
    Diagnostic              getDiagnostics();
    mission_List            getMissionListToPult();
    mission_Status          getissionStatus();
    quint8                  getFirstPointComplete();
    mission_List            getMissionFromPult();



};

#endif // IUSERINTERFACEDATA_H
