#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QFile>
#include <QDate>
#include <QDebug>

#include <QTableWidget>


/* Директивы имен таблицы, полей таблицы и базы данных */

#define DATABASE_NAME       "dataIMU_magn.db"

#define TABLENAME           "IMUDATA_magn"
#define TIME                "Time"
#define DATAIMU_MAGN_X      "Magn_x"
#define DATAIMU_MAGN_Y      "Magn_y"

class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QString name,
                      QString tableName,
                      QString nameColumn1, QString nameColumn2, QString nameColumn3,
                      QObject *parent = 0);
    ~DataBase();
    /* Методы для непосредственной работы с классом
     * Подключение к базе данных и вставка записей в таблицу
     * */
    void connectToDataBase();
    bool inserIntoDeviceTable(QString time, float magn_x, float magn_y);
    void createTable(QTableWidget* table);


private:
    // Сам объект базы данных, с которым будет производиться работа
    QSqlDatabase    db;

    QString nameDB;
    QString tableNameDB;
    QString nameColumn1DB;
    QString nameColumn2DB;
    QString nameColumn3DB;

private:
    /* Внутренние методы для работы с базой данных
     * */
    bool openDataBase();
    bool restoreDataBase();
    void closeDataBase();
    bool createDeviceTable();
};

#endif // DATABASE_H
