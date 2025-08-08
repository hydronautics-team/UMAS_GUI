#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QObject>
#include <QKeyEvent>

#include "remote_control.h"

class KeyBoard : public RemoteControl
{
public:
    explicit KeyBoard(QObject *parent = nullptr);
    ~KeyBoard();


public:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void updateImpact();
};

#endif // KEYBOARD_H
