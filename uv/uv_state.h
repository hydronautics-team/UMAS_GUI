#ifndef UVSTATE_H
#define UVSTATE_H

#include <QObject>
#include <QTime>
#include <cstdint>


/*!\brief Режимы работы системы управления. */
enum class e_CSMode : quint8 {
    MODE_MANUAL = 0,            //! Ручной
    MODE_AUTOMATED,             //! Автоматизированный
    MODE_AUTOMATIC,             //! Автоматический
};

/*!\brief Контуры стабилизации. */
enum class e_StabilizationContours : unsigned char {
    CONTOUR_DEPTH = 0,
    CONTOUR_MARCH,
    CONTOUR_LAG,
    CONTOUR_YAW,
    CONTOUR_ROLL,
    CONTOUR_PITCH
};

/*!\brief Режимы работы системы питания. */
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

/*!\brief Управляющие воздействия. */
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

/*!\brief Флаги замкнутости контуров (1 - замкнуты, 0 - разомкнуты). */
struct ControlContoursFlags {
    ControlContoursFlags();
    quint8 yaw;
    quint8 pitch;
    quint8 roll;
    quint8 march;
    quint8 depth;
    quint8 lag;
};

/*!\brief Текущие данные ПА (telemetry snapshot). */
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

// --- Legacy ---
// Раньше IMU была привязана к конкретной модели датчика (AH127C) через DataAH127C.
// Для новой архитектуры это НЕ используется. Оставлено временно для совместимости
// со старыми вкладками/модулями, которые ещё не мигрировали.
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
    UVState(QObject* parent = nullptr);

    // Обобщённые типы данных без привязки к конкретным моделям датчиков.
    struct ImuData {
        float yaw   = 0.f;
        float pitch = 0.f;
        float roll  = 0.f;

        float accel_x = 0.f;
        float accel_y = 0.f;
        float accel_z = 0.f;

        float gyro_x = 0.f;
        float gyro_y = 0.f;
        float gyro_z = 0.f;

        float mag_x = 0.f;
        float mag_y = 0.f;
        float mag_z = 0.f;

        float quat[4] = {0.f, 0.f, 0.f, 1.f};
        std::uint64_t timestamp_ms = 0;
    };

    struct Pose {
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
    };

    struct Diagnostics {
        // Поля названы под текущий UI Diagnostic_board.
        float u_lipo_1 = 0.f;
        float u_lipo_2 = 0.f;
        float u_diagnostic_board = 0.f;
        float u_power_board = 0.f;
        float current_12 = 0.f;
        float current_5 = 0.f;
        int pwm_1 = 0;
        int pwm_2 = 0;
        int pwm_3 = 0;
        int pwm_4 = 0;
        std::uint64_t timestamp_ms = 0;
    };

    ImuData imu() const { return imu_; }
    Pose pose() const { return pose_; }
    Diagnostics diagnostics() const { return diagnostics_; }
    std::uint8_t controlFlags() const { return control_flags_; }

public slots:
    void setImu(const UVState::ImuData& imu);
    void setPose(double x, double y, double z);
    void setPose(const UVState::Pose& pose);
    void setDiagnostics(const UVState::Diagnostics& diagnostics);
    void setControlFlags(std::uint8_t flags);

signals:
    void imuUpdated(const UVState::ImuData& imu);
    void poseUpdated(const UVState::Pose& pose);
    void diagnosticsUpdated(const UVState::Diagnostics& diagnostics);
    void controlFlagsUpdated(std::uint8_t flags);

private:
    ImuData imu_{};
    Pose pose_{};
    Diagnostics diagnostics_{};
    std::uint8_t control_flags_ = 0;
};

#endif // UVSTATE_H
