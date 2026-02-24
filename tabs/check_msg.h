#ifndef CHECK_MSG_H
#define CHECK_MSG_H

#include <QWidget>
#include <QButtonGroup>
#include <QTimer>

#include "i_user_interface_data.h"

namespace Ui {
class CheckMsg;
}

class CheckMsg : public QWidget
{
    Q_OBJECT

public:
    explicit CheckMsg(QWidget *parent = nullptr);
    ~CheckMsg();

public slots:
    void updateUi_checkMsg();
    void slot_getInterface(IUserInterfaceData *interface);
private:
    Ui::CheckMsg *ui;
    IUserInterfaceData *uv_interface;


};

#endif // CHECK_MSG_H
