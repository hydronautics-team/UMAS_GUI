#include "remote_control.h"

RemoteControl::RemoteControl(int joy_id, int update_time, QObject *parent) : QObject(parent)
{
    id = 0;
    periodUpdateMsec = 20;

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &RemoteControl::updateImpact);
    updateTimer->start(periodUpdateMsec);

    impactAxisMarch = sf::Joystick::Y;
    impactAxisDepth = sf::Joystick::Z;
    impactAxisRoll = sf::Joystick::PovX;
    impactAxisPitch = sf::Joystick::PovY;
    impactAxisYaw = sf::Joystick::R;
}

void RemoteControl::updateImpact() {
    sf::Joystick::update();
    DataAH127C imuData = interface.getImuData();

    if (sf::Joystick::isConnected(id)) {
        setMarch();
        setDepth();
        setRoll(imuData.roll);
        setPitch(imuData.pitch);
        setYaw(imuData.yaw);
    }
}

void RemoteControl::setMarch(){
    interface.setMarch(-sf::Joystick::getAxisPosition(id, impactAxisMarch)/2);
}

void RemoteControl::setDepth(){
    interface.setDepth(3*sf::Joystick::getAxisPosition(id, impactAxisDepth)/4);
}

void RemoteControl::setRoll(float roll){
    interface.setRoll(sf::Joystick::getAxisPosition(id, impactAxisRoll) + roll * interface.getCSMode());
}

void RemoteControl::setPitch(float pitch){
    interface.setPitch((sf::Joystick::getAxisPosition(id, impactAxisPitch)/10 + pitch * interface.getCSMode()));
}

void RemoteControl::setYaw(float yaw){
    interface.setYaw(-(sf::Joystick::getAxisPosition(id, impactAxisYaw)/3 + yaw * interface.getCSMode()));
}

RemoteControl::~RemoteControl(){
}


