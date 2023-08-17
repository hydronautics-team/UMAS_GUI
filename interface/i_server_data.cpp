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

    switch(data.pMode) {
    case power_Mode::MODE_2:
        qDebug() << "power_Mode::MODE_2";
        break;
    case power_Mode::MODE_3:
        qDebug() << "power_Mode::MODE_3";
        break;
    case power_Mode::MODE_4:
        qDebug() << "power_Mode::MODE_4";
        break;
    case power_Mode::MODE_5:
        qDebug() << "power_Mode::MODE_5";
        break;
    }

    return data;
}

void IServerData::parseFullMessage(ToPult message) {
    agent.header = message.header;

    agent.imuData = message.dataAH127C;
    agent.dataPressure = message.dataPressure;
    agent.dataUWB = message.dataUWB;

    agent.auvData.modeReal = message.auvData.modeReal;
    agent.auvData.controlReal = message.auvData.controlReal;
    agent.auvData.modeAUV_Real = message.auvData.modeAUV_Real;
    agent.auvData.signalVMA_real = message.auvData.ControlDataReal;
    agent.auvData.ControlDataReal = message.auvData.signalVMA_real;
}
