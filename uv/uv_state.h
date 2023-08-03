#ifndef UVSTATE_H
#define UVSTATE_H

#include <QObject>

enum class e_CSMode : quint8 { //режим работы
    MODE_MANUAL = 0, //ручной
    MODE_AUTOMATED, //автоматизированный
    MODE_AUTOMATIC, //автоматический
    MODE_GROUP //групповой
};

#pragma pack(push,1)
//структура данных, которая передается из Северова в Пульт
//тут описаны данные, которые Пульт принимает от Северова

struct ControlData { //данные, которые идут с пульта в СУ
    ControlData();
    float yaw;
    float pitch;
    float roll;
    float march;
    float depth;
    float lag;
};

struct ControlContoursFlags { //флаги замыкания контуров (если 1, то замкнуты, 0 - разомкнуты)
    quint8 yaw;
    quint8 pitch;
    quint8 roll;
    quint8 march;
    quint8 depth;
    quint8 lag;
};

//struct AUVCurrentData {
//    //!!тут все текущие параметры АНПА
//    //текущий режим
//    //текущее состояние контуров
//    //текущий выбор модель/реальный НПА
//    //текущие курс, дифферент, крен
//    //управление на ВМА (текущие управляющие сигнлы на движители)
//};

struct PowerSystemData {
    //состояние системы питания
};

struct Header {
    int send;
    int receiv;
    int msg;
};

struct DataAH127C { //структура данных с датчика бесплатформенной системы ориентации
    float yaw; //курс градусы +/- 180
    float pitch; //...
    float roll;

    float X_accel;
    float Y_accel;
    float Z_accel;

    float X_rate;
    float Y_rate;
    float Z_rate;

    float X_magn;
    float Y_magn;
    float Z_magn;

    float quat [4];
};

struct DataPressure { //структура данных с датчика давления
    float temperature; //Temperature returned in deg C.
    float depth; //Depth returned in meters
    float pressure; // Pressure returned in mbar or mbar*conversion rate.
};

struct DataUWB { //структура данных с сверхширокополосного модуля
    float locationX; //координата аппарата по оси X
    float locationY; //координата аппарата по оси Y
    float distanceToBeacon[4]; //расстоние до i-го маяка
    float distanceToAgent[10]; //расстояние до i-го агента
};

//struct Flags
//{

//};

struct ToPult {
    ToPult(int auvID){
//        header.senderID = auvID;
//        header.receiverID = 0;
//        header.msgSize = sizeof (ToPult);
    }
    Header header;
//    AUVCurrentData auvData;
    DataAH127C dataAH127C;// данные с БСО
    DataPressure dataPressure; //данные с датчика давления
    DataUWB dataUWB;//данные с UWB
    quint16 checksum;
};

//структура данных, которая передается из пульта в АНПА
struct FromPult {
    // эти структуры и енумы реализованы в uv_state
    ControlData controlData; //данные, которые идут с пульта при замыканиии контуров
    e_CSMode cSMode; //режим работы
    ControlContoursFlags controlContoursFlags; //флаги замыкания контуров (если больше 0, то замкнуты
    PowerSystemData desiredPowerState; //структура с желаемыми параметрами системы питания
    quint16 checksum;
};

#pragma pack (pop)

class UVState : public QObject
{
    Q_OBJECT
public:
    UVState();
//    ~UV_State();

    ControlData control;

};

#endif // UVSTATE_H
