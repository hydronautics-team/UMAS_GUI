#ifndef IUSERINTERFACEDATA_H
#define IUSERINTERFACEDATA_H

#include "i_basic_data.h"
#include <QDebug>


class IUserInterfaceData : public IBasicData
{
public:
    IUserInterfaceData();

    ControlData getControlData();
};

#endif // IUSERINTERFACEDATA_H
