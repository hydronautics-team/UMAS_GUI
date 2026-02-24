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
    connect(
        ui->pushButton_missionPlanning_keepPos, &QPushButton::toggled,
        this, &ModeAutomatic::slot_pushButton_missionPlanning_keepPos);

    setMission_control();
    setMission_goTo();
    setMission_go_circle();
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
        ui->pushButton_missionControl_modeComplete, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionControl_modeComplete);
}

void ModeAutomatic::setMission_go_circle()
{
    connect(
        ui->pushButton_missionPlanning_go_circle, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionPlanning_go_circle);
    connect(
        ui->pushButton_missionPlanning_go_circle_back, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionPlanning_back);
    connect(
        ui->pushButton_missionPlanning_go_circle_update, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionPlanning_go_circle_update);
}

void ModeAutomatic::setMission_goTo()
{
    connect(
        ui->pushButton_missionPlanning_goto, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionPlanning_goto);
    connect(
        ui->pushButton_missionPlanning_goto_back, &QPushButton::clicked,
        this, &ModeAutomatic::slot_pushButton_missionPlanning_back);
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
        this, &ModeAutomatic::slot_pushButton_missionPlanning_back);
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
void ModeAutomatic::slot_pushButton_missionPlanning_cpp()
{
    ui->stackedWidget_missionPlanning->setCurrentIndex(2);
    displayText_toConsole("Задайте параметры для покрытия области");
    uv_interface->setMissionFromPult(mission_List::MOVE_TACK);
}

//cpp finish

void ModeAutomatic::test_automatic_after()
{
    set_stackedWidget_mode(0);
    ui->stackedWidget_missionPlanning->setCurrentIndex(0);
}

void ModeAutomatic::slot_pushButton_missionControl_modeIdle()
{
    uv_interface->setMissionControl(mission_Control::MODE_IDLE);
}
void ModeAutomatic::slot_pushButton_missionControl_modeStart()
{
    uv_interface->setMissionControl(mission_Control::MODE_START);
}

void ModeAutomatic::slot_pushButton_missionControl_modeComplete()
{
    uv_interface->setMissionControl(mission_Control::MODE_COMPLETE);
}

void ModeAutomatic::slot_pushButton_missionPlanning_goto()
{
    ui->stackedWidget_missionPlanning->setCurrentIndex(1);
    displayText_toConsole("Задайте параметры для выхода в точку");
    uv_interface->setMissionFromPult(mission_List::MOVE_TO_POINT);
}

void ModeAutomatic::slot_pushButton_missionPlanning_go_circle()
{
    ui->stackedWidget_missionPlanning->setCurrentIndex(3);
    displayText_toConsole("Задайте параметры для движения по окружности");
    uv_interface->setMissionFromPult(mission_List::MOVE_CIRCLE);
}

void ModeAutomatic::slot_pushButton_missionPlanning_back()
{
    ui->stackedWidget_missionPlanning->setCurrentIndex(0);
    uv_interface->setMissionFromPult(mission_List::NO_MISSION);
}


void ModeAutomatic::updateUi_DataMission()
{
    int missionStatus = static_cast<int>(uv_interface->getMissionStatus());
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
    switch (uv_interface->getMissionFromPult()) {
    case mission_List::NO_MISSION:
        ui->label_missonCurrent->setText("NO_MISSION");
        break;
    case mission_List::MOVE_TO_POINT:
        ui->label_missonCurrent->setText("MOVE_TO_POINT");
        break;
    case mission_List::KEEP_POS:
        ui->label_missonCurrent->setText("KEEP_POS");
        break;
    case mission_List::MOVE_CIRCLE:
        ui->label_missonCurrent->setText("MOVE_CIRCLE");
        break;
    case mission_List::MOVE_TACK:
        ui->label_missonCurrent->setText("MOVE_TACK");
        break;
    }
    switch (uv_interface->getMissionListToPult()) {
    case mission_List::NO_MISSION:
        ui->label_missonCurrent_toPult->setText("NO_MISSION");
        break;
    case mission_List::MOVE_TO_POINT:
        ui->label_missonCurrent_toPult->setText("MOVE_TO_POINT");
        break;
    case mission_List::KEEP_POS:
        ui->label_missonCurrent_toPult->setText("KEEP_POS");
        break;
    case mission_List::MOVE_CIRCLE:
        ui->label_missonCurrent_toPult->setText("MOVE_CIRCLE");
        break;
    case mission_List::MOVE_TACK:
        ui->label_missonCurrent_toPult->setText("MOVE_TACK");
        break;
    }
}

void ModeAutomatic::slot_pushButton_missionPlanning_keepPos(bool checked)
{
    if (checked){
        uv_interface->setMissionFromPult(mission_List::KEEP_POS);
        MissionParam msg;
        QString text_radius = ui->lineEdit_missionPlanning_keepPos_radius->text();
        float value_radius = text_radius.toFloat();
        msg.radius = value_radius;
        uv_interface->setMissionParam(msg);
    }
    else
        uv_interface->setMissionFromPult(mission_List::NO_MISSION);
}

void ModeAutomatic::slot_getInterface(IUserInterfaceData *interface)
{
    uv_interface = interface;
}

void ModeAutomatic::slot_addPoint_to_gui(double latitude, double longitude)
{
    MissionParam msg;
    qDebug() << static_cast<int>(uv_interface->getMissionFromPult());
    switch (uv_interface->getMissionFromPult()) {
    case mission_List::MOVE_TO_POINT:
        qDebug() << "hello";
        ui->lineEdit_missionPlanning_goto_latitude->setText(QString::number(latitude, 'd', 14));  // Широта
        ui->lineEdit_missionPlanning_goto_longitude->setText(QString::number(longitude, 'd', 14)); // Долгота
        break;
    case mission_List::MOVE_CIRCLE:
        ui->lineEdit_missionPlanning_go_circle_latitude->setText(QString::number(latitude, 'd', 14));  // Широта
        ui->lineEdit_missionPlanning_go_circle_longitude->setText(QString::number(longitude, 'd', 14)); // Долгота
        QString text_radius = ui->lineEdit_missionPlanning_go_circle_radius->text();
        float value_radius = text_radius.toFloat();
        msg.radius = value_radius;
        break;
    }

    msg.point_mission.x_point = latitude;
    msg.point_mission.y_point = longitude;
    uv_interface->setMissionParam(msg);
}

void ModeAutomatic::slot_pushButton_missionPlanning_go_circle_update()
{
    MissionParam msg;
    QString latitude = ui->lineEdit_missionPlanning_go_circle_latitude->text();  // Широта
    QString longitude = ui->lineEdit_missionPlanning_go_circle_longitude->text(); // Долгота
    QString text_radius = ui->lineEdit_missionPlanning_go_circle_radius->text();
    float value_radius = text_radius.toFloat();
    double latitude_d = latitude.toDouble();
    double longitude_d = longitude.toDouble();
    msg.radius = value_radius;
    msg.point_mission.x_point = latitude_d;
    msg.point_mission.y_point = longitude_d;
    uv_interface->setMissionParam(msg);
    // Создаем QString для логирования
    QString logMessage = QString("Mission planning updated: Latitude: %1, Longitude: %2, Radius: %3")
            .arg(QString::number(latitude_d, 'f', 14))
            .arg(QString::number(longitude_d, 'f', 14))
            .arg(QString::number(value_radius, 'f', 2));

    displayText_toConsole(logMessage);
}

ModeAutomatic::~ModeAutomatic()
{
    delete ui;
}
