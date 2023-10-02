#include "database.h"

DataBase::DataBase(QString name,
                   QString tableName,
                   QString nameColumn1, QString nameColumn2, QString nameColumn3,
                   QObject *parent) : QObject(parent)
{
    nameDB = name;
    tableNameDB = tableName;
    nameColumn1DB = nameColumn1;
    nameColumn2DB = nameColumn2;
    nameColumn3DB = nameColumn3;
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
    if(!QFile("./ui_utils/" + nameDB).exists()){
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
    db.setDatabaseName("./ui_utils/" + nameDB);
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
    if(!query.exec( "CREATE TABLE " + tableNameDB + " ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    + nameColumn1DB +    " STRING    NOT NULL,"
                    + nameColumn2DB +    " FLOAT     NOT NULL,"
                    + nameColumn3DB +    " FLOAT     NOT NULL"
                    " )"
                    )){
        qDebug() << "DataBase: error of create " << tableNameDB;
        qDebug() << query.lastError().text();
        return false;
    } else {
        return true;
    }
    return false;
}

/* Метод для вставки записи в таблицу устройств
 * */
bool DataBase::inserIntoDeviceTable(QString str, float value1, float value2)
{
    QSqlQuery query;

    query.prepare("INSERT INTO " + tableNameDB + " ( " + nameColumn1DB + ", "
                  + nameColumn2DB + ", " + nameColumn3DB + " ) "
                  "VALUES (?, ?, ?)");

    query.addBindValue(str);
    query.addBindValue(value1);
    query.addBindValue(value2);
    // После чего выполняется запросом методом exec()
    if(!query.exec()){
        qDebug() << "error insert into " << tableNameDB;
        qDebug() << query.lastError().text();
        return false;
    } else {
        return true;
    }
    return false;
}

void DataBase::createTable(QTableWidget* table)
{
    table->setColumnCount(3); // Указываем число колонок
    table->setShowGrid(true); // Включаем сетку
    // Разрешаем выделение только одного элемента
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    // Разрешаем выделение построчно
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем заголовки колонок
    table->setHorizontalHeaderLabels(QStringList() << nameColumn1DB
                                                   << nameColumn2DB
                                                   << nameColumn3DB
                                     );
    // Обновляем ширину столбцов
    table->resizeColumnsToContents();
}
