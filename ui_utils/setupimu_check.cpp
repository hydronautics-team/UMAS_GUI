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
                                 << "Magn_x"
                                 << "Magn_y"
                   );

    this->createPlot();


    connect(
        ui->pushButton_setupIMU_check_start, SIGNAL(clicked()),
        this, SLOT(startCheck()));

    connect(
        ui->pushButton_setupIMU_check_reset, SIGNAL(clicked()),
        this, SLOT(resetTable()));

    connect(
        ui->pushButton_setupIMU_check_stop, SIGNAL(clicked()),
        this, SLOT(stopCheck()));

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
            this, SLOT(updateUI_table()));
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

        updateUI_setPlot();
    }
}

void SetupIMU_check::updateUI_table()
{
    DataAH127C imuData = uv_interface.getImuData();
    float X_magn = imuData.X_magn;
    float Y_magn = imuData.Y_magn;

    QString currentTime = QTime::currentTime().toString("HH:mm:ss");

    db->inserIntoDeviceTable(currentTime, X_magn, Y_magn);

    QSqlQuery query("SELECT "
                    TABLENAME "." TIME ", "
                    TABLENAME "." DATAIMU_MAGN_X ", "
                    TABLENAME "." DATAIMU_MAGN_Y
                    " FROM " TABLENAME);
    query.last();
    ui->tableWidget->insertRow(i + rowCount_beforeStart);
    ui->tableWidget->setItem(i + rowCount_beforeStart, 0, new QTableWidgetItem(query.value(0).toString()));
    ui->tableWidget->setItem(i + rowCount_beforeStart, 1, new QTableWidgetItem(query.value(1).toString()));
    ui->tableWidget->setItem(i + rowCount_beforeStart, 2, new QTableWidgetItem(query.value(2).toString()));
    i++;

    ui->tableWidget->resizeColumnsToContents();
}

void SetupIMU_check::resetTable()
{
    ui->pushButton_setupIMU_check_reset->setEnabled(false);
    ui->pushButton_setupIMU_check_start->setEnabled(true);

    int rowCount = ui->tableWidget->rowCount();
    for (int r = rowCount; r >= 0; r--)
        ui->tableWidget->removeRow(r);
}

void SetupIMU_check::stopCheck()
{
    timer_setupIMU_check->stop();
    timer_updateUI_check->stop();
    ui->pushButton_setupIMU_check_reset->setEnabled(true);
    ui->pushButton_setupIMU_check_start->setEnabled(true);

    ui->pushButton_setupIMU_check_pause->setEnabled(false);
    ui->pushButton_setupIMU_check_stop->setEnabled(false);

    resetTable();
}

void SetupIMU_check::createPlot()
{
    // Построить ряд как источник данных диаграммы и добавить к нему 6 координатных точек
    series = new QSplineSeries;

    // Построить график
    chart = new QChart();
    chart->legend()->hide();  // скрыть легенду
    chart->addSeries(series);  // добавить серию на график
    chart->setTitle("Построение магнитной характеристики");  // Устанавливаем заголовок графика

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Magn x");
    axisX->setLabelFormat("%g");
    axisX->setTickCount(5);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Magn y");
    axisY->setLabelFormat("%g");
    axisY->setTickCount(5);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Создаем QChartView и устанавливаем сглаживание, заголовок, размер
    chartView = new QChartView(chart);

    // Добавляем его в горизонтальный Layout
    ui->verticalLayout_map->addWidget(chartView);
    chartView->setRenderHint(QPainter::Antialiasing);
}

void SetupIMU_check::updateUI_setPlot()
{
    ui->verticalLayout_map->removeWidget(chartView);

    series = new QSplineSeries;

    float magnX_max = 0;
    float magnY_max = 0;
    float magnX_min = 0;
    float magnY_min = 0;

    for (int r = 0; r < ui->tableWidget->rowCount(); r++)
    {
        QString magn_x = ui->tableWidget->item(r,1)->text();
        QString magn_y = ui->tableWidget->item(r,2)->text();

        if (magnX_max < magn_x.toFloat())
            magnX_max = magn_x.toFloat();
        if (magnY_max < magn_y.toFloat())
            magnY_max = magn_y.toFloat();
        if (magnX_min > magn_x.toFloat())
            magnX_min = magn_x.toFloat();
        if (magnY_min > magn_y.toFloat())
            magnY_min = magn_y.toFloat();

        series->append(magn_x.toFloat() , magn_y.toFloat());
    }

    QString magn_x = ui->tableWidget->item(0,1)->text();
    QString magn_y = ui->tableWidget->item(0,2)->text();

    series->append(magn_x.toFloat() , magn_y.toFloat());

    float bb_x = (magnX_max-magnX_min) * 0.1;
    float bb_y = (magnY_max-magnY_min) * 0.1;


    // Построить график
    chart = new QChart();
    chart->legend()->hide();  // скрыть легенду
    chart->addSeries(series);  // добавить серию на график
    chart->setTitle("Построение магнитной характеристики");  // Устанавливаем заголовок графика

        QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Magn x");
        axisX->setLabelFormat("%g");
    axisX->setTickCount(5);
        axisX->setRange(magnX_min - bb_x , magnX_max + bb_x);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Magn y");
    axisY->setLabelFormat("%g");
    axisY->setTickCount(5);
    axisY->setRange(magnY_min - bb_y, magnY_max + bb_y);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Создаем QChartView и устанавливаем сглаживание, заголовок, размер
    chartView = new QChartView(chart);

    // Добавляем его в горизонтальный Layout
    ui->verticalLayout_map->addWidget(chartView);
    chartView->setRenderHint(QPainter::Antialiasing);

}

void SetupIMU_check::createUI(const QStringList &headers)
{
    ui->tableWidget->setColumnCount(3); // Указываем число колонок
    ui->tableWidget->setShowGrid(true); // Включаем сетку
    // Разрешаем выделение только одного элемента
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    // Разрешаем выделение построчно
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем заголовки колонок
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    // Обновляем ширину столбцов
    ui->tableWidget->resizeColumnsToContents();
}
