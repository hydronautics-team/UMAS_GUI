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
    } else {
        this->openDataBase();
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
                    DATAIMU_MAGN_X    " FLOAT     NOT NULL,"
                    DATAIMU_MAGN_Y    " FLOAT     NOT NULL"
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
bool DataBase::inserIntoDeviceTable(QString time, float magn_x, float magn_y)
{
    QSqlQuery query;

    query.prepare("INSERT INTO " TABLENAME " ( " TIME ", "
                  DATAIMU_MAGN_X ", " DATAIMU_MAGN_Y " ) "
                  "VALUES (:Time, :Magn_x, :Magn_y)");

    query.bindValue(":Time",  time);
    query.bindValue(":Magn_x",    magn_x);
    query.bindValue(":Magn_y",    magn_y);
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
