#ifndef IBASICDATA_H
#define IBASICDATA_H

#include <QObject>
#include <QDebug>

#include "uv_state.h"

class IBasicData : public QObject
{
    Q_OBJECT
public:
    explicit IBasicData(QObject *parent = nullptr);

protected:
    // Static UV_State variable, which we will be accessing
    static UVState agent;
};

#endif // IBASICDATA_H
