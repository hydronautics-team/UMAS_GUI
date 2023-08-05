#include "uv_state.h"

UVState::UVState()
{

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
    yaw = 0;
    pitch = 0;
    roll = 0;
    march = 0;
    depth = 0;
    lag = 0;
}

//DataAH127C::DataAH127C() {
//    yaw = 0;
//    pitch = 0;
//    roll = 0;

//    X_accel = 0;
//    Y_accel = 0;
//    Z_accel = 0;

//    X_rate = 0;
//    Y_rate = 0;
//    Z_rate = 0;

//    X_magn = 0;
//    Y_magn = 0;
//    Z_magn = 0;

//    for (int i = 0; i < 4; i++)
//        quat [i] = 0;
//}

DataPressure::DataPressure() { //структура данных с датчика давления
    temperature = 0; //Temperature returned in deg C.
    depth = 0; //Depth returned in meters
    pressure = 0; // Pressure returned in mbar or mbar*conversion rate.
}

DataUWB::DataUWB() {
    locationX = 0;
    locationY = 0;

    for (int i = 0; i < 5; i++)
        distanceToBeacon [i] = 0;

    for (int i = 0; i < 11; i++)
        distanceToAgent [i] = 0;
}
