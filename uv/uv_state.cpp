#include "uv_state.h"

UVState::UVState()
{
    modeAUV_selection = true;
}

ControlData::ControlData() {
    yaw = 0;
    pitch = 0;
    roll = 0;
    march = 0;
    depth = 0;
    lag = 0;
}

ControlContoursFlags::ControlContoursFlags() {
    yaw = 1;
    pitch = 1;
    roll = 1;
    march = 1;
    depth = 1;
    lag = 1;
}

DataPressure::DataPressure() { //структура данных с датчика давления
    temperature = 0; //Temperature returned in deg C.
    depth = 0; //Depth returned in meters
    pressure = 0; // Pressure returned in mbar or mbar*conversion rate.
}

//DataUWB::DataUWB() {
//    locationX = 0;
//    locationY = 0;

//    for (int i = 0; i < 5; i++)
//        distanceToBeacon [i] = 0;

//    for (int i = 0; i < 11; i++)
//        distanceToAgent [i] = 0;
//}
