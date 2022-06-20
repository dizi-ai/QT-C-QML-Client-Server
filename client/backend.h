#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QStringList>
#include <QTextTable>
#include "client.h"

class Backend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool currentStatus READ getStatus NOTIFY statusChanged)
    Q_PROPERTY(QString filepath READ getFilePath WRITE setFilePath)

public:
    explicit Backend(QObject *parent = nullptr);
    bool getStatus();
    QString getFilePath();

signals:
    void statusChanged(QString newStatus);
    void someError(QString err);
    void someMessage(QString msg);
    void someStatistic(QString msg);


public slots:
    void setStatus(bool newStatus);
    void receivedSomething(QString msg);
    void gotError(QAbstractSocket::SocketError err);
    void sendClicked(int req);
    void connectClicked();
    void disconnectClicked();
    void setFilePath(const QString& filepath);
    void getRequests();
    void getCharCount();
    void getWordsLen();


private:
    Client* client;
    QString filepath = "";
};

#endif // BACKEND_H
