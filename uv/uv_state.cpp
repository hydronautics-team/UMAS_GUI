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

DataAH127C::DataAH127C() {
    yaw = 0;
    pitch = 0;
    roll = 0;

    ax = 0;
    ay = 0;
    az = 0;

    wx = 0;
    wy = 0;
    wz = 0;

    mx = 0;
    my = 0;
    mz = 0;

    for (int i = 0; i < 4; i++)
        quat [i] = 0;
}
