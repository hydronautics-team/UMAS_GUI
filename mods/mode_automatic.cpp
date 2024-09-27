#include "mode_automatic.h"
#include "ui_mode_automatic.h"

ModeAutomatic::ModeAutomatic(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModeAutomatic)
{
    ui->setupUi(this);
    setBottom_modeAutomatic();
}

void ModeAutomatic::setBottom_modeAutomatic()
{
    //! выход из автоматического режима
    connect(
        ui->pushButton_after, SIGNAL(clicked()),
        this, SLOT(test_automatic_after()));

    setMission_control();
    setMission_goTo();
    setMission_go_trajectory();
    setMission_go_following();
    setMission_cpp();
}

void ModeAutomatic::setMission_control()
{
    connect(
        ui->pushButton_missionControl_modeIdle, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionControl_modeIdle);
    connect(
        ui->pushButton_missionControl_modeStart, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionControl_modeStart);
    connect(
        ui->pushButton_missionControl_modeCancel, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionControl_modeCancel);
    connect(
        ui->pushButton_missionControl_modeStop, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionControl_modeStop);
    connect(
        ui->pushButton_missionControl_modeComplete, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionControl_modeComplete);
}

void ModeAutomatic::setMission_goTo()
{
    connect(
        ui->pushButton_missionPlanning_goto, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionPlanning_goto);
    connect(
        ui->pushButton_missionPlanning_goto_update, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionPlanning_goto_update);;
    connect(
        ui->pushButton_missionPlanning_goto_back, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionPlanning_goto_back);
}

void ModeAutomatic::setMission_go_trajectory()
{
    connect(
        ui->pushButton_missionPlanning_go_trajectory_update, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionPlanning_go_trajectory_update);
    connect(
        ui->pushButton_missionPlanning_go_trajectory_back, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionPlanning_go_trajectory_back);
    connect(
        ui->pushButton_missionPlanning_go_trajectory, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionPlanning_go_trajectory);
}

void ModeAutomatic::setMission_go_following()
{
    connect(
        ui->pushButton_missionPlanning_following, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionPlanning_following);
}


//cpp start

void ModeAutomatic::setMission_cpp()
{
    ui->tableWidget_missionPlanning_cpp_zonaResearch->setColumnCount(2);
    ui->tableWidget_missionPlanning_cpp_zonaResearch->setHorizontalHeaderLabels(QStringList() << "X" << "Y");
    ui->pushButton_missionPlanning_cpp_on_off->setCheckable(true);

    connect(
        ui->pushButton_missionPlanning_cpp_make, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionPlanning_cpp_make);
    connect(
        ui->pushButton_missionPlanning_cpp_make_clean, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionPlanning_cpp_make_clean);
    connect(
        ui->pushButton_missionPlanning_cpp_back, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionPlanning_cpp_back);
    connect(
        ui->pushButton_missionPlanning_cpp, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionPlanning_goto_back);
    connect(
        ui->pushButton_missionPlanning_cpp, &QPushButton::clicked,
                this, &ModeAutomatic::slot_pushButton_missionPlanning_cpp);
}

void ModeAutomatic::addPointToTable(qreal x, qreal y) {
    int row = ui->tableWidget_missionPlanning_cpp_zonaResearch->rowCount();
    ui->tableWidget_missionPlanning_cpp_zonaResearch->insertRow(row);
    ui->tableWidget_missionPlanning_cpp_zonaResearch->setItem(row, 0, new QTableWidgetItem(QString::number(x)));
    ui->tableWidget_missionPlanning_cpp_zonaResearch->setItem(row, 1, new QTableWidgetItem(QString::number(y)));
}

//void ModeAutomatic::slot_pushButton_missionPlanning_cpp_make() {
//    QLineSeries *lineSeries = new QLineSeries();
//    int rowCount = ui->tableWidget_missionPlanning_cpp_zonaResearch->rowCount();

//    QStringList pointList;

//    for (int i = 0; i < rowCount; ++i) {
//        qreal x = ui->tableWidget_missionPlanning_cpp_zonaResearch->item(i, 0)->text().toDouble();
//        qreal y = ui->tableWidget_missionPlanning_cpp_zonaResearch->item(i, 1)->text().toDouble();
//        lineSeries->append(x, y);

//        pointList.append(QString::number(x) + "," + QString::number(y));
//    }

//    if (rowCount > 0) {
//        // Замкнуть фигуру, соединяя последнюю точку с первой
//        qreal firstX = ui->tableWidget_missionPlanning_cpp_zonaResearch->item(0, 0)->text().toDouble();
//        qreal firstY = ui->tableWidget_missionPlanning_cpp_zonaResearch->item(0, 1)->text().toDouble();
//        lineSeries->append(firstX, firstY);
//    }
//    emit requestUpdateChart(lineSeries);

//    QString pointsString = pointList.join(" ");

//    // Получение значения из doubleSpinBox
//    double distanceTack = ui->doubleSpinBox_missionPlanning_cpp_distanceTack->value();
//    QString distanceTackStr = QString::number(distanceTack);

//    qDebug() << "Python start:";
//    qDebug() << "Generated points string:" << pointsString;
//    qDebug() << "Distance Tack:" << distanceTackStr;



//    // Создание и настройка QProcess
//    QProcess process;
//    QString program = "python3";
//    QStringList arguments;
//    arguments << "execute_algorithm.py" << pointsString << distanceTackStr;

//    process.setProgram(program);
//    process.setArguments(arguments);
//    process.setWorkingDirectory("/home/shakuevda/Desktop/pult/UMAS_GUI"); // Укажите путь к директории скрипта

//    process.start();

//    // Проверка запуска процесса
//    if (!process.waitForStarted()) {
//        qDebug() << "Failed to start process:" << process.errorString();
//        return;
//    }

//    // Ожидание завершения процесса
//    process.waitForFinished();

//    // Чтение стандартного вывода и ошибок
//    QString output(process.readAllStandardOutput());
//    QString error(process.readAllStandardError());

//    if (!error.isEmpty()) {
//        qDebug() << "Python error:" << error;
//    } else {
//        qDebug() << "Python output:" << output;

//        // Парсинг результата и создание QLineSeries
//        QLineSeries *resultSeries = new QLineSeries();
//        output.remove('[').remove(']').replace("(", "").replace(")", "").replace(" ", "");
//        QStringList pointStrings = output.split(',');

//        for (int i = 0; i < pointStrings.size(); i += 2) {
//            qreal x = pointStrings[i].toDouble();
//            qreal y = pointStrings[i + 1].toDouble();
//            resultSeries->append(x, y);
//        }

//        emit plotLineSeries(resultSeries); // Отправка сигнала с QLineSeries
//    }
//    qDebug() << "Python finish";

//}

void ModeAutomatic::slot_pushButton_missionPlanning_cpp_make() {
    QVector<QPointF> coordinates; // Вектор для хранения исходных координат в метрах
    int rowCount = ui->tableWidget_missionPlanning_cpp_zonaResearch->rowCount();

    QStringList pointList;

    for (int i = 0; i < rowCount; ++i) {
        qreal x = ui->tableWidget_missionPlanning_cpp_zonaResearch->item(i, 0)->text().toDouble();
        qreal y = ui->tableWidget_missionPlanning_cpp_zonaResearch->item(i, 1)->text().toDouble();

        // Добавление координат в вектор
        coordinates.append(QPointF(x, y)); // Координаты в метрах

        pointList.append(QString::number(x) + "," + QString::number(y));
    }

    // Замкнуть фигуру, соединяя последнюю точку с первой
    if (rowCount > 0) {
        qreal firstX = ui->tableWidget_missionPlanning_cpp_zonaResearch->item(0, 0)->text().toDouble();
        qreal firstY = ui->tableWidget_missionPlanning_cpp_zonaResearch->item(0, 1)->text().toDouble();
        coordinates.append(QPointF(firstX, firstY)); // Добавление первой точки в конец
    }

    // Эмитируем сигнал для добавления линии на карте с координатами в метрах
    emit requestAddLine(coordinates); // Эмитируем сигнал с координатами в метрах

    QString pointsString = pointList.join(" ");

    // Получение значения из doubleSpinBox
    double distanceTack = ui->doubleSpinBox_missionPlanning_cpp_distanceTack->value();
    QString distanceTackStr = QString::number(distanceTack);

    qDebug() << "Python start:";
    qDebug() << "Generated points string:" << pointsString;
    qDebug() << "Distance Tack:" << distanceTackStr;

    // Создание и настройка QProcess
    QProcess process;
    QString program = "python3";
    QStringList arguments;
    arguments << "execute_algorithm.py" << pointsString << distanceTackStr;

    process.setProgram(program);
    process.setArguments(arguments);
    process.setWorkingDirectory("/home/shakuevda/Desktop/pult/UMAS_GUI"); // Укажите путь к директории скрипта

    process.start();

    // Проверка запуска процесса
    if (!process.waitForStarted()) {
        qDebug() << "Failed to start process:" << process.errorString();
        return;
    }

    // Ожидание завершения процесса
    process.waitForFinished();

    // Чтение стандартного вывода и ошибок
    QString output(process.readAllStandardOutput());
    QString error(process.readAllStandardError());

    if (!error.isEmpty()) {
        qDebug() << "Python error:" << error;
    } else {
        qDebug() << "Python output:" << output;

        // Парсинг результата и создание вектора координат
        QVector<QPointF> resultCoordinates; // Вектор для хранения результата
        output.remove('[').remove(']').replace("(", "").replace(")", "").replace(" ", "");
        QStringList pointStrings = output.split(',');

        for (int i = 0; i < pointStrings.size(); i += 2) {
            qreal x = pointStrings[i].toDouble();
            qreal y = pointStrings[i + 1].toDouble();
            resultCoordinates.append(QPointF(x, y)); // Добавление координат в метрах
        }

        emit requestAddLine(resultCoordinates); // Эмитируем сигнал с результатом
    }
    qDebug() << "Python finish";
}



void ModeAutomatic::slot_pushButton_missionPlanning_cpp_make_clean() {
    ui->tableWidget_missionPlanning_cpp_zonaResearch->clearContents();
    ui->tableWidget_missionPlanning_cpp_zonaResearch->setRowCount(0);
    emit requestClearLines();
}

void ModeAutomatic::slot_pushButton_missionPlanning_cpp_back()
{
    ui->stackedWidget_missionPlanning->setCurrentIndex(0);
}

void ModeAutomatic::slot_pushButton_missionPlanning_cpp()
{
    ui->stackedWidget_missionPlanning->setCurrentIndex(3);
    displayText_toConsole("Задайте параметры для покрытия области");
}

//cpp finish

void ModeAutomatic::test_automatic_after()
{
    set_stackedWidget_mode(0);
    ui->stackedWidget_missionPlanning->setCurrentIndex(0);
}

void ModeAutomatic::slot_pushButton_missionControl_modeIdle()
{
    uv_interface.setMissionControl(mission_Control::MODE_IDLE);
}
void ModeAutomatic::slot_pushButton_missionControl_modeStart()
{
    uv_interface.setMissionControl(mission_Control::MODE_START);
}
void ModeAutomatic::slot_pushButton_missionControl_modeCancel()
{
    uv_interface.setMissionControl(mission_Control::MODE_CANCEL);
}
void ModeAutomatic::slot_pushButton_missionControl_modeStop()
{
    uv_interface.setMissionControl(mission_Control::MODE_STOP);
}
void ModeAutomatic::slot_pushButton_missionControl_modeComplete()
{
    uv_interface.setMissionControl(mission_Control::MODE_COMPLETE);
    uv_interface.setID_mission_AUV(0);
}

void ModeAutomatic::slot_pushButton_missionPlanning_goto()
{
    uv_interface.setID_mission_AUV(1);
    ui->stackedWidget_missionPlanning->setCurrentIndex(2);
    displayText_toConsole("Задайте параметры для выхода в точку");
}

void ModeAutomatic::slot_pushButton_missionPlanning_goto_update()
{
    double x = ui->doubleSpinBox_missionPlanning_goto_x->value();
    double y = ui->doubleSpinBox_missionPlanning_goto_y->value();
    double r = ui->doubleSpinBox_missionPlanning_goto_r->value();

    emit signal_pushButton_missionPlanning_goto_updateMap(x,y,r,0);
    displayText_toConsole("Установлена координата для выхода в точку и"
                " радиус удержания позиции");
}

void ModeAutomatic::slot_pushButton_missionPlanning_goto_back()
{
    ui->stackedWidget_missionPlanning->setCurrentIndex(0);
}

void ModeAutomatic::slot_pushButton_missionPlanning_go_trajectory_update()
{
    double x1 = ui->doubleSpinBox_missionPlanning_go_trajectory_x_1->value();
    double y1 = ui->doubleSpinBox_missionPlanning_go_trajectory_y_1->value();
    double r1 = ui->doubleSpinBox_missionPlanning_go_trajectory_r_1->value();

    double x2 = ui->doubleSpinBox_missionPlanning_go_trajectory_x_2->value();
    double y2 = ui->doubleSpinBox_missionPlanning_go_trajectory_y_2->value();
    double r2 = ui->doubleSpinBox_missionPlanning_go_trajectory_r_2->value();

    double x3 = ui->doubleSpinBox_missionPlanning_go_trajectory_x_3->value();
    double y3 = ui->doubleSpinBox_missionPlanning_go_trajectory_y_3->value();
    double r3 = ui->doubleSpinBox_missionPlanning_go_trajectory_r_3->value();

    emit signal_pushButton_missionPlanning_go_trajectory_updateMap(x1,y1,r1,0);
    emit signal_pushButton_missionPlanning_go_trajectory_updateMap(x2,y2,r2,0);
    emit signal_pushButton_missionPlanning_go_trajectory_updateMap(x3,y3,r3,0);
    displayText_toConsole("Установлены координаты для движения по траектории и"
                " радиус удержания позиций");
}

void ModeAutomatic::slot_pushButton_missionPlanning_go_trajectory_back()
{
    ui->stackedWidget_missionPlanning->setCurrentIndex(0);
}

void ModeAutomatic::slot_pushButton_missionPlanning_following()
{
    uv_interface.setID_mission_AUV(2);
}

void ModeAutomatic::slot_pushButton_missionPlanning_go_trajectory()
{
    uv_interface.setID_mission_AUV(3);
    ui->stackedWidget_missionPlanning->setCurrentIndex(1);
}

void ModeAutomatic::updateUi_DataMission()
{
    int missionStatus = static_cast<int>(uv_interface.getMissionStatus());
    switch (missionStatus) {
    case 0:
        ui->label_missonStatus->setText("ожидание");
        break;
    case 1:
        ui->label_missonStatus->setText("ошибка инициализации миссии");
        break;
    case 2:
        ui->label_missonStatus->setText("миссия запущена и выполняется");
        break;
    case 3:
        ui->label_missonStatus->setText("миссия приостановлена, на паузе");
        break;
    case 4:
        ui->label_missonStatus->setText("миссия завершена");
        break;
    }
}

ModeAutomatic::~ModeAutomatic()
{
    delete ui;
}
