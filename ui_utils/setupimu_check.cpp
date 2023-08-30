#include "setupimu_check.h"
#include "ui_setupimu_check.h"


SetupIMU_check::SetupIMU_check(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupIMU_check)
{
    ui->setupUi(this);

    ui->pushButton_setupIMU_check_stop->setEnabled(false);
    ui->pushButton_setupIMU_check_pause->setEnabled(false);

    db = new DataBase();
    db->connectToDataBase();

    /* Наполним базу данных записями */
    this->createUI(QStringList() << "Time"
                                 << "Magnitometr"
                   );



    connect(
        ui->pushButton_setupIMU_check_start, SIGNAL(clicked()),
        this, SLOT(startCheck()));

    connect(
        ui->pushButton_setupIMU_check_reset, SIGNAL(clicked()),
        this, SLOT(resetTable()));

}

SetupIMU_check::~SetupIMU_check()
{
    delete ui;
}

void SetupIMU_check::startCheck()
{
    time        = ui->spinBox_time->value();
    periodicity = ui->doubleSpinBox_periodicity->value();
    i = 0;


    if (time && periodicity)
    {
        ui->pushButton_setupIMU_check_start->setEnabled(false);
        ui->pushButton_setupIMU_check_stop->setEnabled(true);
        ui->pushButton_setupIMU_check_pause->setEnabled(true);

        rowCount_beforeStart = ui->tableWidget->rowCount();

        timer_updateUI_check = new QTimer(this);
        connect(
            timer_updateUI_check, SIGNAL(timeout()),
            this, SLOT(updateUI_grafic()));
        timer_updateUI_check->start(periodicity*1000);

        ui->label_setupIMU_check_timer->setNum(time);

        timer_setupIMU_check = new QTimer(this);
        connect(
            timer_setupIMU_check, SIGNAL(timeout()),
            this, SLOT(timer_setupIMU_check_timeStart()));
        timer_setupIMU_check->start(1000);


    }
    else
        qInfo() << "Необходимо выставить длительность и периодичность записи";
}

void SetupIMU_check::timer_setupIMU_check_timeStart()
{
    ui->label_setupIMU_check_timer->setNum(--time);
    if (time == 0) {
        timer_setupIMU_check->stop();
        timer_updateUI_check->stop();
        ui->pushButton_setupIMU_check_reset->setEnabled(true);
        ui->pushButton_setupIMU_check_start->setEnabled(true);

        ui->pushButton_setupIMU_check_pause->setEnabled(false);
        ui->pushButton_setupIMU_check_stop->setEnabled(false);

    }
}

void SetupIMU_check::updateUI_grafic()
{
    DataAH127C imuData = uv_interface.getImuData();
    float X_magn = imuData.X_magn;
    float Y_magn = imuData.Y_magn;

    QString currentTime = QTime::currentTime().toString("HH:mm:ss");



    db->inserIntoDeviceTable(currentTime, i);

    QSqlQuery query("SELECT "
                    TABLENAME "." TIME ", "
                    TABLENAME "." DATAIMU_MAGN
                    " FROM " TABLENAME);
    query.last();
    ui->tableWidget->insertRow(i + rowCount_beforeStart);
    ui->tableWidget->setItem(i + rowCount_beforeStart, 0, new QTableWidgetItem(query.value(0).toString()));
    ui->tableWidget->setItem(i + rowCount_beforeStart, 1, new QTableWidgetItem(query.value(1).toString()));
    i++;
}

void SetupIMU_check::resetTable()
{
//    ui->pushButton_setupIMU_check_reset->setEnabled(false);
    ui->pushButton_setupIMU_check_start->setEnabled(true);

    int rowCount = ui->tableWidget->rowCount();
    qDebug() << rowCount;
    for (int r = rowCount; r >= 0; r--) {
        ui->tableWidget->removeRow(r);
        qDebug() << r;
    }
}

void SetupIMU_check::createUI(const QStringList &headers)
{
    ui->tableWidget->setColumnCount(2); // Указываем число колонок
    ui->tableWidget->setShowGrid(true); // Включаем сетку
    // Разрешаем выделение только одного элемента
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    // Разрешаем выделение построчно
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем заголовки колонок
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    // Растягиваем последнюю колонку на всё доступное пространство
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    QSqlQuery query("SELECT "
                    TABLENAME "." TIME ", "
                    TABLENAME "." DATAIMU_MAGN
                    " FROM " TABLENAME);

    /* Выполняем заполнение QTableWidget записями с помощью цикла
     * */
    for(int i = 0; query.next(); i++){
        // Вставляем строку
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i,0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableWidget->setItem(i,1, new QTableWidgetItem(query.value(1).toString()));
    }

    // Ресайзим колонки по содержимому
    ui->tableWidget->resizeColumnsToContents();
}
