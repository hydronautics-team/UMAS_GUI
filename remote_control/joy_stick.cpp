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
    
    // Инициализация состояния кнопок
    prevButtonAState = false;
    prevButtonBState = false;
    prevButtonXState = false;
    prevButtonYState = false;
    prevStartButtonState = false;
    prevBackButtonState = false;
}

JoyStick::~JoyStick()
{
}

void JoyStick::updateImpact() {
    sf::Joystick::update();
    DataAH127C imuData = getImuData();

    if (sf::Joystick::isConnected(id)) {

        
        // Проверка состояния кнопок
        bool currentButtonAState = sf::Joystick::isButtonPressed(id, BUTTON_A);
        bool currentButtonBState = sf::Joystick::isButtonPressed(id, BUTTON_B);
        bool currentButtonXState = sf::Joystick::isButtonPressed(id, BUTTON_X);
        bool currentButtonYState = sf::Joystick::isButtonPressed(id, BUTTON_Y);
        bool currentStartButtonState = sf::Joystick::isButtonPressed(id, START_BUTTON);
        bool currentBackButtonState = sf::Joystick::isButtonPressed(id, BACK_BUTTON);
        
        
        if (currentButtonAState && !prevButtonAState) {
            emit buttonAPressed();  // Испускаем сигнал при нажатии кнопки A
            qDebug() << "Кнопка A нажата на геймпаде";
        }
        
 
        if (currentButtonBState && !prevButtonBState) {
            emit buttonBPressed();
            qDebug() << "Кнопка B нажата на геймпаде";
        }
        
        if (currentButtonXState && !prevButtonXState) {
            emit buttonXPressed();
            qDebug() << "Кнопка X нажата на геймпаде";
        }
        
        if (currentButtonYState && !prevButtonYState) {
            emit buttonYPressed();
            qDebug() << "Кнопка Y нажата на геймпаде";
        }
        
        if (currentStartButtonState && !prevStartButtonState) {
            emit startButtonPressed();
            qDebug() << "Кнопка Start нажата на геймпаде";
        }
        
        if (currentBackButtonState && !prevBackButtonState) {
            emit backButtonPressed();
            qDebug() << "Кнопка Back нажата на геймпаде";
        }
        
        // Сохраняем текущее состояние для следующего цикла
        prevButtonAState = currentButtonAState;
        prevButtonBState = currentButtonBState;
        prevButtonXState = currentButtonXState;
        prevButtonYState = currentButtonYState;
        prevStartButtonState = currentStartButtonState;
        prevBackButtonState = currentBackButtonState;
    }
}