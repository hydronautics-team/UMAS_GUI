#include "i_user_interface_data.h"

IUserInterfaceData::IUserInterfaceData() : IBasicData()
{

}

// set-функции

void IUserInterfaceData::setFlagAH127C_pult(FlagAH127C_pult flagAH127C_pult)
{
    agent.flagAH127C_pult = flagAH127C_pult;
}

void IUserInterfaceData::setPowerMode(power_Mode mode) {
    agent.pMode = mode;
    qInfo() << "Включен" << static_cast<int> (mode) + 2 << "режим питания";
}

void IUserInterfaceData::setControlContoursFlags(e_StabilizationContours contour, bool value) {
    switch (contour) {
    case e_StabilizationContours::CONTOUR_DEPTH:
        agent.controlContoursFlags.depth = value;
        if (value)
            qInfo() << "Контур глубины замкнут";
        else
            qInfo() << "Контур глубины разомкнут";
        break;

    case e_StabilizationContours::CONTOUR_LAG:
        agent.controlContoursFlags.lag = value;
        if (value)
            qInfo() << "Контур лага замкнут";
        else
            qInfo() << "Контур лага разомкнут";
        break;

    case e_StabilizationContours::CONTOUR_MARCH:
        agent.controlContoursFlags.march = value;
        if (value)
            qInfo() << "Контур марша замкнут";
        else
            qInfo() << "Контур марша разомкнут";
        break;

    case e_StabilizationContours::CONTOUR_PITCH:
        agent.controlContoursFlags.pitch = value;
        if (value)
            qInfo() << "Контур дифферента замкнут";
        else
            qInfo() << "Контур дифферента разомкнут";
        break;

    case e_StabilizationContours::CONTOUR_ROLL:
        agent.controlContoursFlags.roll = value;
        if (value)
            qInfo() << "Контур крена замкнут";
        else
            qInfo() << "Контур крена разомкнут";
        break;

    case e_StabilizationContours::CONTOUR_YAW:
        agent.controlContoursFlags.yaw = value;
        if (value)
            qInfo() << "Контур курса замкнут";
        else
            qInfo() << "Контур курса разомкнут";
        break;
    }
}

void IUserInterfaceData::setCSMode(e_CSMode mode) {
    agent.cSMode = mode;
    if (static_cast<int>(mode))
        qInfo() << "Включен автоматизированный режим";
    else
        qInfo() << "Включен ручной режим";
}

void IUserInterfaceData::setModeSelection(bool mode) {
    agent.modeAUV_selection = mode;
    if (static_cast<int>(mode))
        qInfo() << "Установлен вывод данных на модель";
    else
        qInfo() << "Установлен вывод данных на агента";
}

// get-функции

int IUserInterfaceData::getChecksumMsgAgentSend() {
    return agent.checksum_msg_agent_send;
}

int IUserInterfaceData::getChecksumMsgGuiSend() {
    return agent.checksum_msg_gui_send;
}

int IUserInterfaceData::getChecksumMsgGuiReceived() {
    return agent.checksum_msg_gui_received;
}

ControlContoursFlags IUserInterfaceData::getControlContoursFlags()
{
    return agent.controlContoursFlags;
}

bool IUserInterfaceData::getModeSelection()
{
    return agent.modeAUV_selection;
}

FlagAH127C_bort IUserInterfaceData::getFlagAH127C_bort()
{
    return agent.flagAH127C_bort;
}

FlagAH127C_pult IUserInterfaceData::getFlagAH127C_pult()
{
    return agent.flagAH127C_pult;
}

power_Mode IUserInterfaceData::getPowerMode()
{
    return agent.pMode;
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
