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
    displayText_toConsole("Включен " + QString::number(static_cast<int>(mode) + 2) + " режим питания");
}

void IUserInterfaceData::setControlContoursFlags(e_StabilizationContours contour, bool value) {
    switch (contour) {
    case e_StabilizationContours::CONTOUR_DEPTH:
        agent.controlContoursFlags.depth = value;
        if (value)
            emit displayText_toConsole("Контур глубины замкнут");
        else
            emit displayText_toConsole("Контур глубины разомкнут");
        break;

    case e_StabilizationContours::CONTOUR_LAG:
        agent.controlContoursFlags.lag = value;
        if (value){
            emit displayText_toConsole("Контур лага замкнут");
        }else
            emit displayText_toConsole("Контур лага разомкнут");
        break;

    case e_StabilizationContours::CONTOUR_MARCH:
        agent.controlContoursFlags.march = value;
        if (value)
            emit displayText_toConsole("Контур марша замкнут");
        else
            emit displayText_toConsole("Контур марша разомкнут");
        break;

    case e_StabilizationContours::CONTOUR_PITCH:
        agent.controlContoursFlags.pitch = value;
        if (value)
            emit displayText_toConsole("Контур дифферента замкнут");
        else
            emit displayText_toConsole("Контур дифферента разомкнут");
        break;

    case e_StabilizationContours::CONTOUR_ROLL:
        agent.controlContoursFlags.roll = value;
        if (value)
            emit displayText_toConsole("Контур крена замкнут");
        else
            emit displayText_toConsole("Контур крена разомкнут");
        break;

    case e_StabilizationContours::CONTOUR_YAW:
        agent.controlContoursFlags.yaw = value;
        if (value)
            emit displayText_toConsole("Контур курса замкнут");
        else
            emit displayText_toConsole("Контур курса разомкнут");
        break;
    }
}

void IUserInterfaceData::setCSMode(e_CSMode mode) {
    agent.cSMode = mode;
    if (static_cast<int>(mode))
        emit displayText_toConsole("Включен автоматизированный режим");
    else
        emit displayText_toConsole("Включен ручной режим");
}

void IUserInterfaceData::setModeSelection(bool mode) {
    agent.modeAUV_selection = mode;
    if (static_cast<int>(mode))
        emit displayText_toConsole("Установлен вывод данных на модель");
    else
        emit displayText_toConsole("Установлен вывод данных на агента");
}

void IUserInterfaceData::setDataPultUWB(PultUWB pultUWB)
{
    agent.pultUWB = pultUWB;
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
