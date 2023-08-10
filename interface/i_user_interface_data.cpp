#include "i_user_interface_data.h"

IUserInterfaceData::IUserInterfaceData() : IBasicData()
{

}


void IUserInterfaceData::setControlContoursFlags(e_StabilizationContours contour, bool value) {
    switch (contour) {
    case e_StabilizationContours::CONTOUR_DEPTH:
        agent.controlContoursFlags.depth = value;
        break;

    case e_StabilizationContours::CONTOUR_LAG:
        agent.controlContoursFlags.lag = value;
        break;

    case e_StabilizationContours::CONTOUR_MARCH:
        agent.controlContoursFlags.march = value;
        break;

    case e_StabilizationContours::CONTOUR_PITCH:
        agent.controlContoursFlags.pitch = value;
        break;

    case e_StabilizationContours::CONTOUR_ROLL:
        agent.controlContoursFlags.roll = value;
        break;

    case e_StabilizationContours::CONTOUR_YAW:
        agent.controlContoursFlags.yaw = value;
        break;
    }
}

void IUserInterfaceData::setCSMode(e_CSMode mode) {
    agent.cSMode = mode;
}

void IUserInterfaceData::setModeSelection(bool mode) {
    agent.modeAUV_selection = mode;
}

bool IUserInterfaceData::getCSMode()
{
    if (agent.cSMode == e_CSMode::MODE_AUTOMATED)
        return true;
    else
        return false;
}

ControlData IUserInterfaceData::getControlData() {
    return agent.control;
}

DataAH127C IUserInterfaceData::getImuData() {
    return agent.imuData;
}

Header IUserInterfaceData::getHeader() {
    return agent.header;
}

AUVCurrentData IUserInterfaceData::getAUVCurrentData() {
    return agent.auvData;
}

DataPressure IUserInterfaceData::getDataPressure() {
    return agent.dataPressure;
}

DataUWB IUserInterfaceData::getDataUWB() {
    return agent.dataUWB;
 }
