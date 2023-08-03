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

    if (sf::Joystick::isConnected(id)) {
        setMarch();
        setDepth();
        setRoll();
        setPitch();
        setYaw();
    }
}

void RemoteControl::setMarch(){
    interface.setMarch(sf::Joystick::getAxisPosition(id, impactAxisMarch));
}

void RemoteControl::setDepth(){
    interface.setDepth(sf::Joystick::getAxisPosition(id, impactAxisDepth));
}

void RemoteControl::setRoll(){
    interface.setRoll(sf::Joystick::getAxisPosition(id, impactAxisRoll));
}

void RemoteControl::setPitch(){
    interface.setPitch(sf::Joystick::getAxisPosition(id, impactAxisPitch));
}

void RemoteControl::setYaw(){
    interface.setYaw(sf::Joystick::getAxisPosition(id, impactAxisYaw));
}

RemoteControl::~RemoteControl(){
    qDebug()<<"RemoteControl deleted";
}


