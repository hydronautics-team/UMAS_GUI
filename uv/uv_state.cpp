#include "uv_state.h"

UVState::UVState(QObject* parent)
    : QObject(parent)
{
}

void UVState::setImu(const UVState::ImuData& imu)
{
    imu_ = imu;
    emit imuUpdated(imu_);
}

void UVState::setPose(double x, double y, double z)
{
    pose_.x = x;
    pose_.y = y;
    pose_.z = z;
    emit poseUpdated(pose_);
}

void UVState::setPose(const UVState::Pose& pose)
{
    pose_ = pose;
    emit poseUpdated(pose_);
}

void UVState::setDiagnostics(const UVState::Diagnostics& diagnostics)
{
    diagnostics_ = diagnostics;
    emit diagnosticsUpdated(diagnostics_);
}

void UVState::setControlFlags(std::uint8_t flags)
{
    control_flags_ = flags;
    emit controlFlagsUpdated(control_flags_);
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


