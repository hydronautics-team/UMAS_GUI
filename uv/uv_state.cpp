#include "uv_state.h"

UVState::UVState()
{
    modeAUV_selection = true;
    cSMode = e_CSMode::MODE_MANUAL;

    controlContoursFlags.depth = 1;
    controlContoursFlags.lag = 1;
    controlContoursFlags.march = 1;
    controlContoursFlags.pitch = 1;
    controlContoursFlags.roll = 1;
    controlContoursFlags.yaw = 1;

    pMode = power_Mode::MODE_2;



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


