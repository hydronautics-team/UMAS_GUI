#include "joy_stick.h"

JoyStick::JoyStick(QObject *parent)
{
    id = 0;
    int periodUpdateMsec = 20;

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &JoyStick::updateImpact);
    updateTimer->start(periodUpdateMsec);

    impactAxisMarch = sf::Joystick::Y;
    impactAxisDepth = sf::Joystick::Z;
    impactAxisRoll = sf::Joystick::PovX;
    impactAxisPitch = sf::Joystick::PovY;
    impactAxisYaw = sf::Joystick::R;
    impactAxisLag = sf::Joystick::X;
}

JoyStick::~JoyStick()
{
}

void JoyStick::updateImpact() {
    sf::Joystick::update();
    DataAH127C imuData = getImuData();

    if (sf::Joystick::isConnected(id)) {
        setMarch(-sf::Joystick::getAxisPosition(id, impactAxisMarch)/2);
        setLag(sf::Joystick::getAxisPosition(id, impactAxisLag)/2);
//        setDepth(3*sf::Joystick::getAxisPosition(id, impactAxisDepth)/4);
        setRoll(sf::Joystick::getAxisPosition(id, impactAxisRoll)/4);
        setPitch(sf::Joystick::getAxisPosition(id, impactAxisPitch)/10);
        setYaw((sf::Joystick::getAxisPosition(id, impactAxisYaw)/4));
    }
}

