#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDebug>


#include "remote_control/remote_control.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void timerUpdateImpact(int periodUpdateMsec);

private slots:
    void UpdateImpact();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
