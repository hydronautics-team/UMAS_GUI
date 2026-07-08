#include "diagnostic_board.h"
#include "./ui_diagnostic_board.h"

Diagnostic_board::Diagnostic_board(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Diagnostic_board)
{
    ui->setupUi(this);
}

Diagnostic_board::~Diagnostic_board()
{
    delete ui;
}

void Diagnostic_board::Check_param_LIPO(float U_LIPO_1, float U_LIPO_2)
{
    // --- АКБ 1 ---
    if(U_LIPO_1 <= 9.5){
        ui->label_LIPO_1->setStyleSheet("background-color: rgb(237, 51, 59); color: white;");
    } else if(U_LIPO_1 <= 10.5){ 
        ui->label_LIPO_1->setStyleSheet("background-color: rgb(248, 228, 92);");
    } else {
      ui->label_LIPO_1->setStyleSheet("background-color: rgb(87, 227, 137);");
    }

    // --- АКБ 2 ---
    if(U_LIPO_2 <= 9.5){
        ui->label_LIPO_2->setStyleSheet("background-color: rgb(237, 51, 59); color: white;");
    } else if(U_LIPO_2 <= 10.5){
        ui->label_LIPO_2->setStyleSheet("background-color: rgb(248, 228, 92);");
    } else {
      ui->label_LIPO_2->setStyleSheet("background-color: rgb(87, 227, 137);");
    }
}

void Diagnostic_board::updateBattery(float u1, float u2)
{
    ui->label_LIPO_1->setNum(u1);
    ui->label_LIPO_2->setNum(u2);
    Check_param_LIPO(u1, u2);
}

void Diagnostic_board::updateDepth(float depth)
{
    ui->label_depth->setNum(depth);
}

void Diagnostic_board::updateDistance(float distance_to_bottom)
{
    ui->label_distance->setNum(distance_to_bottom);
}

void Diagnostic_board::updateStatus(bool killswitch, bool leak)
{
    killswitch_state_ = killswitch;
    leak_state_ = leak;

    // --- Киллсвитч ---
    if (killswitch) {
        ui->label_killswitch->setText("АКТИВИРОВАН");
        ui->label_killswitch->setStyleSheet("background-color: rgb(237, 51, 59); color: white; font-weight: bold;");
    } else {
        ui->label_killswitch->setText("ВЫКЛ");
        ui->label_killswitch->setStyleSheet("background-color: rgb(87, 227, 137);");
    }

    // --- Течь ---
    if (leak) {
        ui->label_leak->setText("ТЕЧЬ!");
        ui->label_leak->setStyleSheet("background-color: rgb(237, 51, 59); color: white; font-weight: bold;");
    } else {
        ui->label_leak->setText("СУХО");
        ui->label_leak->setStyleSheet("background-color: rgb(87, 227, 137);");
    }
}

void Diagnostic_board::updateSensors(bool pressure_sensor_ok, bool imu_ok, bool dvl_ok)
{
    // Вспомогательная лямбда для обновления статусов датчиков
    auto updateSensorStatus = [](QLabel* label, bool isOk) {
        if (isOk) {
            label->setText("Штатно");
            label->setStyleSheet("background-color: rgb(87, 227, 137);");
        } else {
            label->setText("Ошибка");
            label->setStyleSheet("background-color: rgb(237, 51, 59); color: white;");
        }
    };

    updateSensorStatus(ui->label_imu_status, imu_ok);
    updateSensorStatus(ui->label_pressure_status, pressure_sensor_ok);
    updateSensorStatus(ui->label_dvl_status, dvl_ok);

    // --- Общий статус ---
    bool all_ok = imu_ok && pressure_sensor_ok && dvl_ok && !killswitch_state_ && !leak_state_;
    
    if (all_ok) {
         ui->label_system_status->setText("СИСТЕМА ИСПРАВНА");
         ui->label_system_status->setStyleSheet("background-color: rgb(87, 227, 137); font-weight: bold;");
    } else {
         ui->label_system_status->setText("ЕСТЬ НЕИСПРАВНОСТИ");
         ui->label_system_status->setStyleSheet("background-color: rgb(248, 228, 92); font-weight: bold;");
    }
}

// Старый слот для совместимости
void Diagnostic_board::Display_Update(float U_LIPO_1, float U_LIPO_2, float depth, float distance_to_bottom,
                    bool killswitch, bool leak, bool imu_ok, bool pressure_sensor_ok, bool dvl_ok)
{
    updateBattery(U_LIPO_1, U_LIPO_2);
    updateDepth(depth);
    updateDistance(distance_to_bottom);
    updateStatus(killswitch, leak);
    updateSensors(pressure_sensor_ok, imu_ok, dvl_ok);
}