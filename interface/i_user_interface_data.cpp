#include "i_user_interface_data.h"

IUserInterfaceData::IUserInterfaceData() : IBasicData()
{

}

ControlData IUserInterfaceData::getControlData() {
    return agent.control;
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

//DataAH127C IUserInterfaceData::getImuData() {
//    DataAH127C data;

//    data = agent.imuData;

//    return data;
//}
