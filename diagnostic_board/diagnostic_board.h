#ifndef DIAGNOSTIC_BOARD_H
#define DIAGNOSTIC_BOARD_H

#include <QWidget>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class Diagnostic_board; }
QT_END_NAMESPACE

class Diagnostic_board : public QWidget
{
    Q_OBJECT

public:
    Diagnostic_board(QWidget *parent = nullptr);
    ~Diagnostic_board();

private:
    void Check_param_LIPO(float U_LIPO_1, float U_LIPO_2);
    void Chek_param_power(float U_Diagnostic_board, float U_Power_Board,
                          float Current_12, float Current_5);
    Ui::Diagnostic_board *ui;

    bool killswitch_state_ = false; 
    bool leak_state_ = false;

public slots:
    // --- НОВЫЕ РАЗДЕЛЕННЫЕ СЛОТЫ ---
    void updateBattery(float u1, float u2);
    void updateDepth(float depth);
    void updateDistance(float distance_to_bottom);
    void updateStatus(bool killswitch, bool leak);
    void updateSensors(bool pressure_sensor_ok, bool imu_ok, bool dvl_ok);

    // Старый слот оставлен для совместимости, если он где-то еще вызывается
    void Display_Update(float U_LIPO_1, float U_LIPO_2, float depth, float distance_to_bottom,
                        bool killswitch, bool leak, bool imu_ok, bool pressure_sensor_ok, bool dvl_ok);
};
#endif // DIAGNOSTIC_BOARD_H
