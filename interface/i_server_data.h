#ifndef ISERVERDATA_H
#define ISERVERDATA_H

#include <QDataStream>
#include <QByteArray>
#include <QDebug>

#include "i_basic_data.h"

class IServerData : public IBasicData
{
public:
    IServerData();

    FromPult generateFullMessage();

    void parseFullMessage(ToPult message);
};

#endif // ISERVERDATA_H
