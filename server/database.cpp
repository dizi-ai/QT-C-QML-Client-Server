#include "database.h"

DataBase::DataBase(QObject *parent) : QObject(parent){}
DataBase::~DataBase(){}

void DataBase::connectToDataBase()
{
    QString path = QCoreApplication::applicationDirPath();
    path.append("/" DATABASE_NAME);
    qDebug() << path;
    if(!QFile(path).exists()){
        this->restoreDataBase();
    } else {
        this->openDataBase();
    }
}

bool DataBase::restoreDataBase()
{
    if(this->openDataBase()){
        return (this->createTable()) ? true : false;
    } 
    else {
        qDebug() << "Failed to restore the database";
        return false;
    }
    return false;
}

bool DataBase::openDataBase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName(DATABASE_HOSTNAME);
    QString path = QCoreApplication::applicationDirPath();
    path.append("/" DATABASE_NAME);
    db.setDatabaseName(path);
    if(db.open()){
        return true;
    } else {
        return false;
    }
}

void DataBase::closeDataBase()
{
    db.close();
}

bool DataBase::createTable()
{
    QSqlQuery query;
    if(!query.exec("CREATE TABLE IF NOT EXISTS REQUESTS  ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "date DATETIME , "
                  "from_address TEXT, "
                  "bytes_read INTEGER);")){
        qDebug() << "DataBase: error of create table";
        qDebug() << query.lastError().text();
        return false;
    } 
    else {
        return true;
    }
    return false;
}

bool DataBase::insertInto(const QVariantList &data) {
    QSqlQuery query;
    query.prepare("INSERT INTO REQUESTS ( date, from_address, bytes_read ) "
                  "VALUES (:F, :S, :T)");
    query.bindValue(":F",       data[0].toString());
    query.bindValue(":S",       data[1].toString());
    query.bindValue(":T",       data[2].toString());

    if(!query.exec()){
        qDebug() << "error insert into table";
        qDebug() << query.lastError().text();
        return false;
    }
    else {
        return true;
    }
    return false;
}

QVariantMap DataBase::select(const QString& address)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM REQUESTS WHERE from_address = :address");
    query.bindValue(":address", address);
    if(!query.exec()){
        qDebug() << "error select from table";
        qDebug() << query.lastError().text();
        return QVariantMap();
    } 
    else {
        QMap<int, QString> tores;
        QVariantList qvl;
        int c = 0;
        while(query.next()){
            QString temp = query.value(1).toString() + " ";
            temp += query.value(2).toString() + " ";
            temp += query.value(3).toString();
            tores[c] = temp;
            c++;
        }
        QVariantMap toret;
        QMapIterator<int, QString> i(tores);
        while (i.hasNext()) {
            i.next();
            toret.insert(QString(i.key()), i.value());
        }
        return toret;
    }
}
