#include "remote_control.h"

RemoteControl::RemoteControl(int joy_id, int update_time, QObject *parent) : QObject(parent)
{
    id = 0;
    periodUpdateMsec = 30;

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
    interface.setMarch(sf::Joystick::getAxisPosition(id, impactAxisMarch));
}

void RemoteControl::setDepth(){
    interface.setDepth(sf::Joystick::getAxisPosition(id, impactAxisDepth));
}

void RemoteControl::setRoll(float roll){
    interface.setRoll(sf::Joystick::getAxisPosition(id, impactAxisRoll) + roll * interface.getCSMode());
}

void RemoteControl::setPitch(float pitch){
    interface.setPitch(sf::Joystick::getAxisPosition(id, impactAxisPitch) + pitch * interface.getCSMode());
}

void RemoteControl::setYaw(float yaw){
    interface.setYaw(sf::Joystick::getAxisPosition(id, impactAxisYaw) + yaw * interface.getCSMode());
}

RemoteControl::~RemoteControl(){
    qDebug()<<"RemoteControl deleted";
}


