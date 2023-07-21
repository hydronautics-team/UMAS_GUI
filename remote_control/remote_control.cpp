#include "remote_control.h"

RemoteControl::RemoteControl(int joy_id, int update_time, QObject *parent) : QObject(parent)
{
    id = 0;
    periodUpdateMsec = 30;

    impactAxisMarch = sf::Joystick::Y;
    impactAxisDepth = sf::Joystick::Z;
    impactAxisRoll = sf::Joystick::PovX;
    impactAxisPitch = sf::Joystick::PovY;
    impactAxisYaw = sf::Joystick::R;
}


float RemoteControl::getMarch(){
    return(sf::Joystick::getAxisPosition(id, impactAxisMarch));
}

float RemoteControl::getDepth(){
    return(sf::Joystick::getAxisPosition(id, impactAxisDepth));
}

float RemoteControl::getRoll(){
    return(sf::Joystick::getAxisPosition(id, impactAxisRoll));
}

float RemoteControl::getPitch(){
    return(sf::Joystick::getAxisPosition(id, impactAxisPitch));
}

float RemoteControl::getYaw(){
    return(sf::Joystick::getAxisPosition(id, impactAxisYaw));
}

RemoteControl::~RemoteControl(){

}


