#ifndef DATABASE_H
#define DATABASE_H
#include <QCoreApplication>
#include <QObject>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QMap>
#include <QFile>
#include <QDate>
#include <QDebug>

#define DATABASE_HOSTNAME   "TestDataBase"
#define DATABASE_NAME       "Test.db"

class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = 0);
    ~DataBase();
    void connectToDataBase();

private:
    QSqlDatabase    db;

private:
    bool openDataBase();
    bool restoreDataBase();
    void closeDataBase();
    bool createTable();

public slots:
    bool insertInto(const QVariantList &data);
    QVariantMap select(const QString& address);
};
#endif // DATABASE_H
