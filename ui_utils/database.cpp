#include "database.h"

DataBase::DataBase(QObject *parent) : QObject(parent)
{

}

DataBase::~DataBase()
{

}

/* Методы для подключения к базе данных
 * */
void DataBase::connectToDataBase()
{
    /* Перед подключением к базе данных производим проверку на её существование.
     * В зависимости от результата производим открытие базы данных или её восстановление
     * */
    if(!QFile("./ui_utils/" DATABASE_NAME).exists()){
        qDebug() << "createDeviceTable" << this->restoreDataBase();
        qDebug() << 1;
    } else {
        this->openDataBase();
        qDebug() << 2;
    }
}

/* Методы восстановления базы данных
 * */
bool DataBase::restoreDataBase()
{
    if(this->openDataBase()){
        if(!this->createDeviceTable()){
            return false;
        } else {
            return true;
        }
    } else {
        qDebug() << "Не удалось восстановить базу данных";
        return false;
    }
    return false;
}

/* Метод для открытия базы данных
 * */
bool DataBase::openDataBase()
{
    /* База данных открывается по заданному пути
     * и имени базы данных, если она существует
     * */
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./ui_utils/" DATABASE_NAME);
    if(db.open()){
        return true;
    } else {
        return false;
    }
}

/* Методы закрытия базы данных
 * */
void DataBase::closeDataBase()
{
    db.close();
}

/* Метод для создания таблицы устройств в базе данных
 * */
bool DataBase::createDeviceTable()
{
    /* В данном случае используется формирование сырого SQL-запроса
     * с последующим его выполнением.
     * */
    QSqlQuery query;
    if(!query.exec( "CREATE TABLE " TABLENAME " ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    TIME            " STRING     NOT NULL,"
                    DATAIMU_MAGN    " INTEGER     NOT NULL"
                    " )"
                    )){
        qDebug() << "DataBase: error of create " << TABLENAME;
        qDebug() << query.lastError().text();
        return false;
    } else {
        return true;
    }
    return false;
}

/* Метод для вставки записи в таблицу устройств
 * */
bool DataBase::inserIntoDeviceTable(QString time, int magn)
{
    QSqlQuery query;

    qDebug() << ">> inserIntoDeviceTable <<";

    query.prepare("INSERT INTO " TABLENAME " ( " TIME ", "
                  DATAIMU_MAGN " ) "
                  "VALUES (:Time, :Magnitometr)");

//    query.prepare("INSERT INTO IMUDATA_magn (Time, Magnitometr) "
//                  "VALUES (:Time, :Magnitometr)");

//    query.prepare("INSERT INTO " TABLENAME " (" TIME ", "
//                  DATAIMU_MAGN ") "
//                  "VALUES (:Время, :Магнитометр )");
    query.bindValue(":Time",  time);
    query.bindValue(":Magnitometr",    magn);
    // После чего выполняется запросом методом exec()
    if(!query.exec()){
        qDebug() << "error insert into " << TABLENAME;
        qDebug() << query.lastError().text();
        return false;
    } else {
        return true;
    }
    return false;
}
