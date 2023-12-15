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
    switch (static_cast<int>(mode)) {
    case 0:
        emit displayText_toConsole("Включен ручной режим");
        break;
    case 1:
        emit displayText_toConsole("Включен автоматизированный режим");
        break;
    case 2:
        emit displayText_toConsole("Включен автоматический режим");
        break;
    }
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

void IUserInterfaceData::setMissionControl(mission_Control missionControl)
{
    agent[getCurrentAgent()].missionControl = missionControl;
    switch (static_cast<int>(missionControl)) {
    case 0:
        emit displayText_toConsole("Включен режим ожидания команд в автоматическом режиме");
        break;
    case 1:
        emit displayText_toConsole("Отправлен запрос на выполнение миссии");
        break;
    case 2:
        emit displayText_toConsole("Выполнение миссии отменено");
        break;
    case 3:
        emit displayText_toConsole("Выполнение миссии приостановлено");
        break;
    }
}

void IUserInterfaceData::setID_mission_AUV(quint8 ID_mission_AUV)
{
    agent[getCurrentAgent()].ID_mission_AUV = ID_mission_AUV;
    switch (static_cast<int>(ID_mission_AUV)) {
    case 0:
        emit displayText_toConsole("Миссия завершена, флаг сброшен");
        break;
    case 1:
        emit displayText_toConsole("Запущена миссия выхода в точку");
        break;
    case 2:
        emit displayText_toConsole("Запущена миссия следования");
        break;
    case 3:
        emit displayText_toConsole("Запущена миссия движения по траектории");
        break;
    }
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

DataUWB IUserInterfaceData::getDataUWB(int selectAgent)
{
    return agent[selectAgent].dataUWB;
}

mission_Status IUserInterfaceData::getMissionStatus()
{
    return agent[getCurrentAgent()].missionStatus;
}
