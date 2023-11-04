#include "i_user_interface_data.h"

IUserInterfaceData::IUserInterfaceData() : IBasicData()
{

}

// set-функции

void IUserInterfaceData::setCurrentAgent(int newAgent)
{
    currentAgent = newAgent;
}

void IUserInterfaceData::setFlagAH127C_pult(FlagAH127C_pult flagAH127C_pult)
{
    agent[getCurrentAgent()].flagAH127C_pult = flagAH127C_pult;
}

void IUserInterfaceData::setPowerMode(power_Mode mode) {
    agent[getCurrentAgent()].pMode = mode;
    displayText_toConsole("Включен " + QString::number(static_cast<int>(mode) + 2) + " режим питания");
}

void IUserInterfaceData::setControlContoursFlags(e_StabilizationContours contour, bool value) {
    switch (contour) {
    case e_StabilizationContours::CONTOUR_DEPTH:
        agent[getCurrentAgent()].controlContoursFlags.depth = value;
        if (value)
            emit displayText_toConsole("Контур глубины замкнут");
        else
            emit displayText_toConsole("Контур глубины разомкнут");
        break;

    case e_StabilizationContours::CONTOUR_LAG:
        agent[getCurrentAgent()].controlContoursFlags.lag = value;
        if (value){
            emit displayText_toConsole("Контур лага замкнут");
        }else
            emit displayText_toConsole("Контур лага разомкнут");
        break;

    case e_StabilizationContours::CONTOUR_MARCH:
        agent[getCurrentAgent()].controlContoursFlags.march = value;
        if (value)
            emit displayText_toConsole("Контур марша замкнут");
        else
            emit displayText_toConsole("Контур марша разомкнут");
        break;

    case e_StabilizationContours::CONTOUR_PITCH:
        agent[getCurrentAgent()].controlContoursFlags.pitch = value;
        if (value)
            emit displayText_toConsole("Контур дифферента замкнут");
        else
            emit displayText_toConsole("Контур дифферента разомкнут");
        break;

    case e_StabilizationContours::CONTOUR_ROLL:
        agent[getCurrentAgent()].controlContoursFlags.roll = value;
        if (value)
            emit displayText_toConsole("Контур крена замкнут");
        else
            emit displayText_toConsole("Контур крена разомкнут");
        break;

    case e_StabilizationContours::CONTOUR_YAW:
        agent[getCurrentAgent()].controlContoursFlags.yaw = value;
        if (value)
            emit displayText_toConsole("Контур курса замкнут");
        else
            emit displayText_toConsole("Контур курса разомкнут");
        break;
    }
}

void IUserInterfaceData::setCSMode(e_CSMode mode) {
    agent[getCurrentAgent()].cSMode = mode;
    if (static_cast<int>(mode))
        emit displayText_toConsole("Включен автоматизированный режим");
    else
        emit displayText_toConsole("Включен ручной режим");
}

void IUserInterfaceData::setModeSelection(bool mode) {
    agent[getCurrentAgent()].modeAUV_selection = mode;
    if (static_cast<int>(mode))
        emit displayText_toConsole("Установлен вывод данных на модель");
    else
        emit displayText_toConsole("Установлен вывод данных на агента");
}

void IUserInterfaceData::setDataPultUWB(PultUWB pultUWB)
{
    agent[getCurrentAgent()].pultUWB = pultUWB;
}

// get-функции

int IUserInterfaceData::getCurrentAgent()
{
    return currentAgent;
}

int IUserInterfaceData::getChecksumMsgAgentSend() {
    return agent[getCurrentAgent()].checksum_msg_agent_send;
}

int IUserInterfaceData::getChecksumMsgGuiSend() {
    return agent[getCurrentAgent()].checksum_msg_gui_send;
}

int IUserInterfaceData::getChecksumMsgGuiReceived() {
    return agent[getCurrentAgent()].checksum_msg_gui_received;
}

ControlContoursFlags IUserInterfaceData::getControlContoursFlags()
{
    return agent[getCurrentAgent()].controlContoursFlags;
}

bool IUserInterfaceData::getModeSelection()
{
    return agent[getCurrentAgent()].modeAUV_selection;
}

FlagAH127C_bort IUserInterfaceData::getFlagAH127C_bort()
{
    return agent[getCurrentAgent()].flagAH127C_bort;
}

FlagAH127C_pult IUserInterfaceData::getFlagAH127C_pult()
{
    return agent[getCurrentAgent()].flagAH127C_pult;
}

power_Mode IUserInterfaceData::getPowerMode()
{
    return agent[getCurrentAgent()].pMode;
}

bool IUserInterfaceData::getCSMode()
{
    if (agent[getCurrentAgent()].cSMode == e_CSMode::MODE_AUTOMATED)
        return true;
    else
        return false;
}

ControlData IUserInterfaceData::getControlData() {
    return agent[getCurrentAgent()].control;
}

DataAH127C IUserInterfaceData::getImuData() {
    return agent[getCurrentAgent()].imuData;
}

Header IUserInterfaceData::getHeader() {
    return agent[getCurrentAgent()].header;
}

AUVCurrentData IUserInterfaceData::getAUVCurrentData() {
    return agent[getCurrentAgent()].auvData;
}

DataPressure IUserInterfaceData::getDataPressure() {
    return agent[getCurrentAgent()].dataPressure;
}

DataUWB IUserInterfaceData::getDataUWB() {
    return agent[getCurrentAgent()].dataUWB;
}
