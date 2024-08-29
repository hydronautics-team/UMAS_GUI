#ifndef UVSTATE_H
#define UVSTATE_H

#include <QObject>
#include <QDebug>

/*!
 * \brief e_CSMode enum класс режимов работы системы управления.
 */
enum class e_CSMode : quint8 {
    MODE_MANUAL = 0,            //! Ручной
    MODE_AUTOMATED,             //! Автоматизированный
    MODE_AUTOMATIC,             //! Автоматический
    MODE_GROUP                  //! Групповой
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
    MODE_2 = 0,     //! Включены вычислитель, wifi, uwb
    MODE_3,         //! Включены вычислитель, wifi, uwb, гидроакустика
    MODE_4,         //! Включены вычислитель, wifi, uwb, гидроакустика, ВМА
    MODE_5          //! Выключить вычислитель на 5 секунд и включить обратно
};

#pragma pack(push,1)

/*!
 * \brief The mission_Control enum - команды управления миссией
 */
enum class mission_Control : quint8
{
    MODE_IDLE = 0,  //!ожидание
    MODE_START,     //!отправка запроса на выполнение миссии
    MODE_CANCEL,    //!отмена выполнения миссии
    MODE_STOP,      //!пауза, остановить временно
    MODE_COMPLETE   //!завершить миссию
};

/*!
 * \brief The mission_Status enum состояния миссий
 */
enum class mission_Status : quint8
{
    MODE_IDLE = 0,  //!ожидание
    MODE_ERROR,     //!ошибка инициализации миссии
    MODE_RUNNING,   //!миссия запущена и выполняется
    MODE_STOPPED,   //!миссия приостановлена, на паузе
    MODE_PERFOMED,  //!миссия завершена
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
    float VMA5     = 0;
    float VMA6     = 0;
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
    quint8 ID_mission;
    mission_Status missionStatus;
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
    quint8 ID_mission_AUV;
    mission_Control missionControl;
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

    AUVCurrentData auvData;

    FlagAH127C_bort flagAH127C_bort;
    FlagAH127C_pult flagAH127C_pult;

    bool modeAUV_selection;
    e_CSMode cSMode;
    ControlContoursFlags controlContoursFlags;
    ControlData control;
    power_Mode pMode;

    quint8 ID_mission;
    mission_Status missionStatus;
    quint8 ID_mission_AUV;
    mission_Control missionControl;

    int checksum_msg_gui_send;
    int checksum_msg_agent_send;
    int checksum_msg_gui_received;
};

#endif // UVSTATE_H
