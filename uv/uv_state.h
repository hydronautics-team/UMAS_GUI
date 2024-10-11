#ifndef UVSTATE_H
#define UVSTATE_H

#include <QObject>
#include <QDebug>
#include <QTime>


/*!
 * \brief e_CSMode enum класс режимов работы системы управления.
 */
enum class e_CSMode : quint8 {
    MODE_MANUAL = 0,            //! Ручной
    MODE_AUTOMATED,             //! Автоматизированный
    MODE_AUTOMATIC,             //! Автоматический
};

/*!
 * \brief e_StabilizationContours enum класс для работы с замыканием и
 *  размыканием контуров управления.
 */
enum class e_StabilizationContours : unsigned char {
    CONTOUR_DEPTH = 0,
    CONTOUR_MARCH,
    CONTOUR_LAG,
    CONTOUR_YAW,
    CONTOUR_ROLL,
    CONTOUR_PITCH
};

/*!
 * \brief power_Mode enum класс режимов работы системы питания.
 */
enum class power_Mode : quint8
{ //режим работы
    MODE_2 = 0,     //! На ВМА не идет ШИМ
    MODE_3,         //! На ВМА идет ШИМ
};

#pragma pack(push,1)

enum class mission_List : quint8
{ //список миссий
    NO_MISSION = 0, //нет миссии
    MOVE_TO_POINT, //выход в точку
    KEEP_POS, //удержание в точке
    MOVE_CIRCLE, //движение по окружности
    MOVE_TACK //движение галсами
};

struct CoordinatePoint
{
    double x_point;
    double y_point;
};

struct MissionParam
{
    float radius;
    CoordinatePoint point_mission;
    CoordinatePoint first_point_tack;
    CoordinatePoint second_point_tack;
};

enum class mission_Control : quint8
{ //команды управления миссией
    MODE_IDLE = 0, //ожидание
    MODE_START, //отправка запроса на выполнение миссии
    MODE_COMPLETE //завершить миссию
};

enum class mission_Status : quint8
{ //состояние миссии
    MODE_IDLE = 0, //ожидание
    MODE_RUNNING, //миссия запущена и выполняется
    MODE_PERFOMED, //миссия завершена
};

/*!
 * \brief FlagAH127C_bort class структура, передаваемая на пульт.
 *  Используется для калибровки БСО.
 */
struct FlagAH127C_bort
{
    quint8 startCalibration = false;    //! Флаг подтверждает старт калибровки.
    quint8 endCalibration = false;      //! Флаг подтверждает завершение калибровки.
};

/*!
 * \brief The FlagAH127C_pult class структура, передаваемая на агента.
 *  Используется для калибровки БСО.
 */
struct FlagAH127C_pult
{
    quint8 initCalibration = false;     //! Флаг запуска калибровки.
    quint8 saveCalibration = false;     //! Флаг сохранения калибровки.
};

/*!
 * \brief ControlData class управляющие воздействия с пульта управления.
 */
struct ControlData {
    ControlData();
    float yaw;
    float pitch;
    float roll;
    float march;
    float depth;
    float lag;
};

/*!
 * \brief ControlVMA class управляющие воздействия на каждый ВМА.
 */
struct ControlVMA
{
    float VMA1     = 0;
    float VMA2     = 0;
    float VMA3     = 0;
    float VMA4     = 0;
};

/*!
 * \brief ControlContoursFlags class структура со значениями замкутости контуров
 *  (если 1, то замкнуты, 0 - разомкнуты).
 */
struct ControlContoursFlags {
    ControlContoursFlags();
    quint8 yaw;
    quint8 pitch;
    quint8 roll;
    quint8 march;
    quint8 depth;
    quint8 lag;
};

/*!
 * \brief AUVCurrentData class структура, передаваемая на пульт.
 *  Имеет текущие параметры агента.
 */
struct AUVCurrentData
{
    quint8 modeReal;                    //! Текущий режим.
    ControlContoursFlags controlReal;   //! Текущее состояние контуров.
    quint8 modeAUV_Real;                //! Текущий выбор модель/реальный НПА.
    ControlData ControlDataReal;        //! Текущие курс, дифферент, крен.
    ControlVMA signalVMA_real;          //! Управление на ВМА.
};

struct Header {
    int senderID;
    int receiverID;
    int msgSize;
};

/*!
 * \brief DataAH127C class структура данных с датчика БСО.
 *  Курс измеряется в градусах +/- 180 и т.д.
 */
struct DataAH127C {

    float yaw;
    float pitch;
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

struct DataGANS
{ //структура данных с ГАНСА

    double azimuth     = 0; // Горизонтальный угол на ответчик, град.
    double distance    = 0; // Дистанция до ответчика, м
    double dataValue   = 0; // Значение запрошенного параметра

    double temperature      = 0; // Температура воды, °С
    double depth            = 0; // Глубина базовой станции от поверхности, м

    double roll_GANS  = 0; // Крен, °. 0 - вертикальное положение, 0..+90 - поворот на правый борт, 0..-90 - поворот на левый борт
    double pitch_GANS = 0; // Дифферент, °. 0 - вертикальное положение, 0..+90 - крен на нос, 0..-90 - крен на корму
};

struct GPS_angular
{
    QTime time_UTC;    // Время UTC
    double yaw = 0;        // Курс (рысканье)
    double pitch;      // Килевая качка
    double roll;       // Бортовая качка
    char dataType;  // Тип данных (N - курс от GPS, G - гиро курс)
};

struct GPS_coordinate
{
    QTime time;           // UTC Время обсервации
    double latitude;      // Широта
    char latHemisphere;   // Полушарие (N/S)
    double longitude;     // Долгота
    char lonHemisphere;   // Полушарие (E/W)
    int quality;          // Индикатор качества обсервации
    int satellitesUsed;   // Количество спутников
    double hdop;          // Величина горизонтального геометрического фактора (HDOP)
    double altitude;      // Высота антенны над уровнем моря (геоидом)
    char altitudeUnit;    // Единица измерения высоты (м)
    double geoidHeight;   // Превышение геоида над эллипсоидом WGS84
    char geoidUnit;       // Единица измерения превышения геоида (м)
    double dgpsAge = 0;       // Возраст дифференциальной поправки
    int dgpsStationId = 0;    // Идентификатор ККС
};

struct Diagnostic {
    quint16 voltage_bat;
    quint16 voltage_5v;
    quint16 voltage_12v;
    quint16 voltage_4;
    quint16 current_1;
    quint16 current_2;
    quint16 current_3;
    quint16 current_4;
    quint8 PMW1;
    quint8 PMW2;
    quint8 PMW3;
    quint8 PMW4;
};


/*!
 * \brief ToPult class структура данных, принимаемых на пульте.
 */
struct ToPult
{
    ToPult(int auvID=0)
    {
        header.senderID = auvID;
        header.receiverID = 0;
        header.msgSize = sizeof (ToPult);
    }
    Header header;
    AUVCurrentData auvData;             //! Данные о текущих параметрах
    DataAH127C dataAH127C;              //! Данные с БСО
    FlagAH127C_bort flagAH127C_bort;    //! Флаги для настрой
    DataGANS dataGANS;
    GPS_angular angularGPS;
    GPS_coordinate coordinateGPS;
    Diagnostic diagnostics;
    mission_List missionList = mission_List::NO_MISSION; //выбор миссии
    mission_Status missionStatus = mission_Status::MODE_IDLE; //состояние выполнения миссии
    quint8 first_point_complete; //флаг прохождения точки для движения галсами
    uint checksum;
};

/*!
 * \brief FromPult class структура данных, передаваемая из пульта на агент.
 */
struct FromPult
{
    ControlData controlData;                        //! Данные, которые идут с пульта при замыканиии контуров
    e_CSMode cSMode;                                //! Режим работы
    ControlContoursFlags controlContoursFlags;      //! Флаги замыкания контуров (1 - замкнуты)
    quint8 modeAUV_selection;                       //! Текущий выбор модель/реальный НПА
    power_Mode pMode;                               //! Режим работы системы питания
    FlagAH127C_pult flagAH127C_pult;
    CoordinatePoint reper; //координаты выставленного на карте репера
    mission_List mission = mission_List::NO_MISSION; //выбор миссии
    MissionParam mission_param; //параметры для задания миссии
    mission_Control missionControl = mission_Control::MODE_IDLE; //команды запуска миссии
    uint checksum;
};

#pragma pack (pop)

/*!
 * \brief UVState class класс всех возможных состояний при работе с ПА.
 */
class UVState : public QObject
{
    Q_OBJECT
public:
    UVState();
    Header header;
    DataAH127C imuData;
    DataGANS dataGANS;
    GPS_angular angularGPS;
    GPS_coordinate coordinateGPS;
    Diagnostic diagnostics;

    AUVCurrentData auvData;

    FlagAH127C_bort flagAH127C_bort;
    FlagAH127C_pult flagAH127C_pult;

    bool modeAUV_selection;
    e_CSMode cSMode;
    ControlContoursFlags controlContoursFlags;
    ControlData control;
    power_Mode pMode;

    mission_List missionListToPult;
    mission_List missionListFromPult;
    mission_Status missionStatus;
    quint8 first_point_complete;
    CoordinatePoint reper;
    MissionParam mission_param;
    mission_Control missionControl;


    int checksum_msg_gui_send;
    int checksum_msg_agent_send;
    int checksum_msg_gui_received;
};

#endif // UVSTATE_H
