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
        setMarch(1);
        break;
    case Qt::Key_L:
        setMarch(-1);
        break;
    case Qt::Key_W:
        setPitch(1);
        break;
    case Qt::Key_S:
        setPitch(-1);
        break;
    case Qt::Key_A:
        setYaw(1);
        break;
    case Qt::Key_D:
        setYaw(-1);
        break;
    case Qt::Key_C:
        setDepth(1);
        break;
    case Qt::Key_V:
        setDepth(-1);
        break;
    case Qt::Key_Q:
        setRoll(1);
        break;
    case Qt::Key_E:
        setRoll(-1);
        break;
    case Qt::Key_K:
        setLag(1);
        break;
    case Qt::Key_Semicolon:
        setLag(-1);
        break;
    }
}

void KeyBoard::keyReleaseEvent(QKeyEvent *event)
{
        switch (event->key()) {
        case Qt::Key_O:
            setMarch(0);
            break;
        case Qt::Key_L:
            setMarch(0);
            break;
        case Qt::Key_W:
            setPitch(0);
            break;
        case Qt::Key_S:
            setPitch(0);
            break;
        case Qt::Key_A:
            setYaw(0);
            break;
        case Qt::Key_D:
            setYaw(0);
            break;
        case Qt::Key_C:
            setDepth(0);
            break;
        case Qt::Key_V:
            setDepth(0);
            break;
        case Qt::Key_Q:
            setRoll(0);
            break;
        case Qt::Key_E:
            setRoll(0);
            break;
        case Qt::Key_K:
            setLag(0);
            break;
        case Qt::Key_Semicolon:
            setLag(0);
            break;
        }
}
