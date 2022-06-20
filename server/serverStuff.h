#ifndef SERVERSTUFF_H
#define SERVERSTUFF_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantMap>
#include "database.h"
#include "charcount.h"
#include "wordslen.h"

class ServerStuff : public QObject
{
    Q_OBJECT

public:
    ServerStuff();
    QTcpServer *tcpServer;
    QList<QTcpSocket *> getClients();
    DataBase db;

public slots:
    virtual void newConnection();
    void readClient();
    void gotDisconnection();
    qint64 sendToClient(QTcpSocket *socket, const QJsonDocument &json);

signals:
    void gotNewMesssage(QString msg);
    void smbDisconnected();

private:
    quint16 m_nNextBlockSize;
    QList<QTcpSocket*> clients;
};

#endif // SERVERSTUFF_H
