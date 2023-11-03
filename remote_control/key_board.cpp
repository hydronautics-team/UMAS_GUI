#include "key_board.h"

KeyBoard::KeyBoard(QObject *parent)
{

}

KeyBoard::~KeyBoard()
{
}

void KeyBoard::keyPressEvent(QKeyEvent *event)
{
    ControlData control = getControlData();
    switch (event->key()) {
    case Qt::Key_O:
        setMarch(control.march + 10);
        break;
    case Qt::Key_L:
        setMarch(control.march - 10);
        break;
    case Qt::Key_W:
        setPitch(control.pitch + 10);
        break;
    case Qt::Key_S:
        setPitch(control.pitch - 10);
        break;
    case Qt::Key_A:
        qDebug()<<"key A pressed";
        setYaw(control.yaw + 10);
        break;
    case Qt::Key_D:
        setYaw(control.yaw - 10);
        break;
    case Qt::Key_C:
        setDepth(control.depth + 10);
        break;
    case Qt::Key_V:
        setDepth(control.depth - 10);
        break;
    case Qt::Key_Q:
        setRoll(control.roll + 10);
        break;
    case Qt::Key_E:
        setRoll(control.roll - 10);
        break;
    case Qt::Key_K:
        setLag(control.lag + 10);
        break;
    case Qt::Key_Semicolon:
        setLag(control.lag - 10);
        break;
    }
}
