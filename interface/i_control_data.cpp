#include "i_control_data.h"

IControlData::IControlData() : IBasicData()
{
}

void IControlData::setControlData(ControlData data)
{
    agent[getCurrentAgent()].control = data;
}

void IControlData::setMarch(double march)
{
    agent[getCurrentAgent()].control.march = march;
}

void IControlData::setLag(double lag)
{
    agent[getCurrentAgent()].control.lag = lag;
}

void IControlData::setDepth(double depth)
{
    agent[getCurrentAgent()].control.depth = depth;
}

void IControlData::setRoll(double roll)
{
    agent[getCurrentAgent()].control.roll = roll;
}

void IControlData::setPitch(double pitch)
{
    agent[getCurrentAgent()].control.pitch = pitch;
}

void IControlData::setYaw(double yaw)
{
    agent[getCurrentAgent()].control.yaw = yaw;
}

void IControlData::setGripping(quint8 gripping)
{
    agent[getCurrentAgent()].control.gripping = gripping;
}

void IControlData::setOpening(quint8 opening)
{
    agent[getCurrentAgent()].control.opening = opening;
}

void IControlData::setRotmanlf(quint8 rotmanlf)
{
    agent[getCurrentAgent()].control.rotmanlf = rotmanlf;
}

void IControlData::setRotmanrt(quint8 rotmanrt)
{
    agent[getCurrentAgent()].control.rotmanrt = rotmanrt;
}

void IControlData::setPowoff(quint8 powoff)
{
    agent[getCurrentAgent()].control.powoff = powoff;
}

int IControlData::getCurrentAgent()
{
    return currentAgent;
}

bool IControlData::getCSMode()
{
    if (agent[getCurrentAgent()].cSMode == e_CSMode::MODE_AUTOMATED)
        return true;
    else
        return false;
}

DataAH127C IControlData::getImuData()
{
    return agent[getCurrentAgent()].imuData;
}

ControlData IControlData::getControlData()
{
    return agent[getCurrentAgent()].control;
}
