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

public slots:
    void Display_Update(float U_LIPO_1, float U_LIPO_2, float U_Diagnostic_board, float U_Power_Board,
                        float Current_12, float Current_5, int PWM_1, int PWM_2, int PWM_3, int PWM_4);
};
#endif // DIAGNOSTIC_BOARD_H
