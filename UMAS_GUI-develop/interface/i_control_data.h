#ifndef ICONTROLDATA_H
#define ICONTROLDATA_H

#include "i_basic_data.h"

/*!
 * \brief IControlData class класс для получения и установки данных связанных
 *  с управляющими воздействиями и пультом управления.
 */
class IControlData : public IBasicData
{
public:
    IControlData();



    void            setControlData(ControlData data);

    void            setMarch(double march);
    void            setLag(double lag);
    void            setDepth(double depth);
    void            setRoll(double roll);
    void            setPitch(double pitch);
    void            setYaw(double yaw);

    void            setGripping(quint8 gripping);
    void            setRotman(quint8 rotman);
    void            setOffPower(quint8 offPower);

    int             getCurrentAgent();
    bool            getCSMode();
    DataAH127C      getImuData();
    ControlData     getControlData();
};

#endif // ICONTROLDATA_H
