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
/* **brief**
 * Функия сравнивает значение полученное с платы диагностики с заданными полками напряжения на АКБ и
 * в соответствии с состоянием окрашивает цветом ячейку со значение напряжение
 */
void Diagnostic_board::Check_param_LIPO(float U_LIPO_1, float U_LIPO_2)
{
    if(U_LIPO_1 <= 9.5){
        ui->label_LIPO_1->setStyleSheet("background-color: rgb(237, 51, 59)");
    } else if(U_LIPO_1 <= 10){
        ui->label_LIPO_1->setStyleSheet("background-color: rgb(248, 228, 92)");
    } else {
      ui->label_LIPO_1->setStyleSheet("background-color: rgb(87, 227, 137)");
    }

    if(U_LIPO_2 <= 9.5){
        ui->label_LIPO_2->setStyleSheet("background-color: rgb(237, 51, 59)");
    } else if(U_LIPO_2 <= 10){
        ui->label_LIPO_2->setStyleSheet("background-color: rgb(248, 228, 92)");
    } else {
      ui->label_LIPO_2->setStyleSheet("background-color: rgb(87, 227, 137)");
    }
}
/* **brief**
 * Функция принимает значения напряжения и тока с платы питания и сигнализирует, окрашивание в красный либо зеленый цвет
 * о критических значениях этих параметров
 * */
void Diagnostic_board::Chek_param_power(float U_Diagnostic_board, float U_Power_Board, float Current_12, float Current_5)
{
    if(U_Diagnostic_board < 4.6 || U_Diagnostic_board > 5.3){
      ui->label_U_DIAGNOSTIC_BOARD->setStyleSheet("background-color: rgb(237, 51, 59)");
    } else {
        ui->label_U_DIAGNOSTIC_BOARD->setStyleSheet("background-color: rgb(87, 227, 137)");
    }

    if(U_Power_Board < 4.8 || U_Diagnostic_board > 5.2){
      ui->label_U_POWER_BOARD->setStyleSheet("background-color: rgb(237, 51, 59)");
    } else {
        ui->label_U_POWER_BOARD->setStyleSheet("background-color: rgb(87, 227, 137)");
    }

    if(Current_12 <= 0 || Current_12 > 2){
      ui->label_CURRENT_12->setStyleSheet("background-color: rgb(237, 51, 59)");
    } else {
        ui->label_CURRENT_12->setStyleSheet("background-color: rgb(87, 227, 137)");
    }

    if(Current_5 <= 0 || Current_5 > 1.5){
      ui->label_CURRENT_5->setStyleSheet("background-color: rgb(237, 51, 59)");
    } else {
        ui->label_CURRENT_5->setStyleSheet("background-color: rgb(87, 227, 137)");
    }

}


/* **brief**
 * Функия выводит на экран значения с платы диагностики
 */
void Diagnostic_board::Display_Update(float U_LIPO_1, float U_LIPO_2, float U_Diagnostic_board, float U_Power_Board, float Current_12,
                                      float Current_5, int PWM_1, int PWM_2, int PWM_3, int PWM_4)
{
    ui->label_LIPO_1->setNum(U_LIPO_1);
    ui->label_LIPO_2->setNum(U_LIPO_2);
    ui->label_U_DIAGNOSTIC_BOARD->setNum(U_Diagnostic_board);
    ui->label_U_POWER_BOARD->setNum(U_Power_Board);
    ui->label_CURRENT_12->setNum(Current_12);
    ui->label_CURRENT_5->setNum(Current_5);
    ui->progressBar_PWM_1->setValue(PWM_1);
    ui->progressBar_PWM_2->setValue(PWM_2);
    ui->progressBar_PWM_3->setValue(PWM_3);
    ui->progressBar_PWM_4->setValue(PWM_4);
    Check_param_LIPO(U_LIPO_1, U_LIPO_2);
    Chek_param_power(U_Diagnostic_board, U_Power_Board, Current_12, Current_5);


}

