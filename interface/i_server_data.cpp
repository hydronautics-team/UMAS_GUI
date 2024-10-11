#include "i_server_data.h"

IServerData::IServerData()
{

}

FromPult IServerData::generateFullMessage(int nmbAgent) {
    FromPult data;

    data.controlData = agent[nmbAgent].control;
    data.cSMode = agent[nmbAgent].cSMode;
    data.controlContoursFlags = agent[nmbAgent].controlContoursFlags;
    data.modeAUV_selection = agent[nmbAgent].modeAUV_selection;
    data.pMode = agent[nmbAgent].pMode;
    data.flagAH127C_pult = agent[nmbAgent].flagAH127C_pult;
    data.reper = agent[nmbAgent].reper;
    data.mission = agent[nmbAgent].missionListFromPult;
    data.mission_param = agent[nmbAgent].mission_param;
    data.missionControl = agent[nmbAgent].missionControl;

    agent[nmbAgent].checksum_msg_gui_send = sizeof(data);

    return data;
}

void IServerData::parseFullMessage(ToPult message, int nmbAgent) {
    qDebug() << "hello";
    agent[nmbAgent].header = message.header;

    agent[nmbAgent].auvData.modeReal = message.auvData.modeReal;
    agent[nmbAgent].auvData.controlReal = message.auvData.controlReal;
    agent[nmbAgent].auvData.modeAUV_Real = message.auvData.modeAUV_Real;
    agent[nmbAgent].auvData.signalVMA_real = message.auvData.signalVMA_real;
    agent[nmbAgent].auvData.ControlDataReal = message.auvData.ControlDataReal;

    agent[nmbAgent].imuData = message.dataAH127C;
    agent[nmbAgent].flagAH127C_bort = message.flagAH127C_bort;

    agent[nmbAgent].dataGANS = message.dataGANS;
    agent[nmbAgent].angularGPS = message.angularGPS;
    agent[nmbAgent].coordinateGPS = message.coordinateGPS;
    agent[nmbAgent].diagnostics = message.diagnostics;

    agent[nmbAgent].missionListToPult = message.missionList;
    agent[nmbAgent].missionStatus = message.missionStatus;
    agent[nmbAgent].first_point_complete = message.first_point_complete;

    agent[nmbAgent].checksum_msg_agent_send = message.checksum;
    agent[nmbAgent].checksum_msg_gui_received = sizeof(message);
}
