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

/*!
 * \brief DataBase class вспомогательный класс для работы с таблицами и базами данных.
 */
class DataBase : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief DataBase конструктор класса, определяет структуру ДБ.
     * \param name название файла ".db".
     * \param tableName название таблицы.
     * \param nameColumn1 название первой колонки.
     * \param nameColumn2 название второй колонки.
     * \param nameColumn3 название третей колонки.
     */
    explicit DataBase(QString name,
                      QString tableName,
                      QString nameColumn1, QString nameColumn2, QString nameColumn3,
                      QObject *parent = 0);
    ~DataBase();

    /*!
     * \brief connectToDataBase метод подключение к ДБ.
     */
    void connectToDataBase();
    /*!
     * \brief inserIntoDeviceTable метод заполнения ДБ
     * \param str строка для первой колонки
     * \param value1 значение для второй колонки.
     * \param value2 значение для третей колонки.
     * \return false в случае неудачи.
     */
    bool inserIntoDeviceTable(QString str, float value1, float value2);
    /*!
     * \brief createTable метод созания таблицы в QTableWidget.
     * \param table указатель на QTableWidget.
     */
    void createTable(QTableWidget* table);


private:
    /*!
     * \brief db объект базы данных, с которым будет производиться работа
     */
    QSqlDatabase    db;

    QString nameDB;
    QString tableNameDB;
    QString nameColumn1DB;
    QString nameColumn2DB;
    QString nameColumn3DB;

private:
    /*!
     * \brief openDataBase метод открытия ДБ.
     * \return false в случае неудачи.
     */
    bool openDataBase();
    /*!
     * \brief restoreDataBase метод восстановления ДБ.
     * \return false в случае неудачи.
     */
    bool restoreDataBase();
    /*!
     * \brief closeDataBase метод закрытия ДБ.
     */
    void closeDataBase();
    /*!
     * \brief createDeviceTable метод создания таблицы в ДБ.
     * \return false в случае неудачи.
     */
    bool createDeviceTable();
};

#endif // DATABASE_H
