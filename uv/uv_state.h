#ifndef UVSTATE_H
#define UVSTATE_H

#include <QObject>

enum class e_CSMode : quint8 { //режим работы
    MODE_MANUAL = 0, //ручной
    MODE_AUTOMATED, //автоматизированный
    MODE_AUTOMATIC, //автоматический
    MODE_GROUP //групповой
};

enum class e_StabilizationContours: unsigned char {
    CONTOUR_DEPTH = 0,
    CONTOUR_MARCH,
    CONTOUR_LAG,
    CONTOUR_YAW,
    CONTOUR_ROLL,
    CONTOUR_PITCH
};

enum class power_Mode : quint8
{ //режим работы
    MODE_2 = 0, //включены вычислитель, wifi, uwb
    MODE_3, //включены вычислитель, wifi, uwb, гидроакустика
    MODE_4, //включены вычислитель, wifi, uwb, гидроакустика, ВМА
    MODE_5 //выключить вычислитель на 5 секунд и включить обратно
};

#pragma pack(push,1)
//структура данных, которая передается из Северова в Пульт
//тут описаны данные, которые Пульт принимает от Северова

struct FlagAH127C_bort
{
    bool startCalibration = false;
    bool endCalibration = false;
};

struct FlagAH127C_pult
{
    bool initCalibration = false;
    bool saveCalibration = false;
};

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
    ControlContoursFlags();
    quint8 yaw;
    quint8 pitch;
    quint8 roll;
    quint8 march;
    quint8 depth;
    quint8 lag;
};

struct AUVCurrentData
{
    //!!тут все текущие параметры АНПА
    quint8 modeReal;//текущий режим
    ControlContoursFlags controlReal;//текущее состояние контуров
    quint8 modeAUV_Real;//текущий выбор модель/реальный НПА
    ControlData ControlDataReal;//текущие курс, дифферент, крен
    ControlData signalVMA_real;//управление на ВМА (текущие управляющие сигнлы на движители)
};

struct Header {
    int senderID;
    int receiverID;
    int msgSize;
};

struct DataAH127C { //структура данных с датчика бесплатформенной системы ориентации
 //   DataAH127C();

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
    DataPressure();
    float temperature; //Temperature returned in deg C.
    float depth; //Depth returned in meters
    float pressure; // Pressure returned in mbar or mbar*conversion rate.
};

struct DataUWB { //структура данных с сверхширокополосного модуля
    DataUWB();
    float locationX; //координата аппарата по оси X
    float locationY; //координата аппарата по оси Y
    float distanceToBeacon[4]; //расстоние до i-го маяка
    float distanceToAgent[10]; //расстояние до i-го агента
};

//struct Flags
//{

//};

struct ToPult
{
    ToPult(int auvID=0)
    {
        header.senderID = auvID;
        header.receiverID = 0;
        header.msgSize = sizeof (ToPult);
    }
    Header header;
    AUVCurrentData auvData;// данные о текущих параметрах
    DataAH127C dataAH127C;// данные с БСО
    DataPressure dataPressure; //данные с датчика давления
    DataUWB dataUWB;//данные с UWB
    FlagAH127C_bort flagAH127C_bort;
    uint checksum;
};

//структура данных, которая передается из пульта в АНПА
struct FromPult
{
    ControlData controlData; //данные, которые идут с пульта при замыканиии контуров
    e_CSMode cSMode; //режим работы
    ControlContoursFlags controlContoursFlags; //флаги замыкания контуров (если больше 0, то замкнуты
    bool modeAUV_selection;//текущий выбор модель/реальный НПА
    power_Mode pMode; //режим работы системы питания, структура с желаемыми параметрами системы питания
    FlagAH127C_pult flagAH127C_pult;
    uint checksum;
};

#pragma pack (pop)

class UVState : public QObject
{
    Q_OBJECT
public:
    UVState();
//    ~UV_State();
    Header header;
    DataAH127C imuData;
    DataPressure dataPressure;
    DataUWB dataUWB;

    AUVCurrentData auvData;

    FlagAH127C_bort flagAH127C_bort;
    FlagAH127C_pult flagAH127C_pult;

    bool modeAUV_selection;
    e_CSMode cSMode;
    ControlContoursFlags controlContoursFlags;
    ControlData control;
    power_Mode pMode;
};

#endif // UVSTATE_H
