#include "i_server_data.h"

IServerData::IServerData()
{

}

FromPult IServerData::generateFullMessage() {
    FromPult data;

    data.controlData = agent.control;
    data.cSMode = agent.cSMode;
    data.controlContoursFlags = agent.controlContoursFlags;
    data.modeAUV_selection = agent.modeAUV_selection;
    data.pMode = agent.pMode;
    data.flagAH127C_pult = agent.flagAH127C_pult;

    agent.checksum_msg_gui_send = sizeof(data);

    return data;
}

void IServerData::parseFullMessage(ToPult message) {
    agent.header = message.header;

    agent.imuData = message.dataAH127C;
    agent.dataPressure = message.dataPressure;
    agent.dataUWB = message.dataUWB;
    agent.flagAH127C_bort = message.flagAH127C_bort;

    agent.auvData.modeReal = message.auvData.modeReal;
    agent.auvData.controlReal = message.auvData.controlReal;
    agent.auvData.modeAUV_Real = message.auvData.modeAUV_Real;
    agent.auvData.signalVMA_real = message.auvData.signalVMA_real;
    agent.auvData.ControlDataReal = message.auvData.ControlDataReal;

    agent.checksum_msg_agent_send = message.checksum;
    agent.checksum_msg_gui_received = sizeof(message);
}
