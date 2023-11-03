#include "i_control_data.h"

IControlData::IControlData() :
    IBasicData() {

}

void IControlData::setControlData(ControlData data) {
    agent.control = data;
}

void IControlData::setMarch(double march) {
    agent.control.march = march;
}

void IControlData::setLag(double lag) {
    agent.control.lag = lag;
}

void IControlData::setDepth(double depth) {
    agent.control.depth = depth;
}

void IControlData::setRoll(double roll) {
    agent.control.roll = roll;
}

void IControlData::setPitch(double pitch) {
    agent.control.pitch = pitch;
}

void IControlData::setYaw(double yaw) {
    agent.control.yaw = yaw;
}

bool IControlData::getCSMode()
{
    if (agent.cSMode == e_CSMode::MODE_AUTOMATED)
        return true;
    else
        return false;
}

DataAH127C IControlData::getImuData() {
    return agent.imuData;
}

ControlData IControlData::getControlData()
{
    return agent.control;
}
