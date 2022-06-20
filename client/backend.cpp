#include "backend.h"

Backend::Backend(QObject *parent) : QObject(parent)
{
    client = new Client("localhost", 6547);
    connect(client, &Client::hasReadSome, this, &Backend::receivedSomething);
    connect(client, &Client::statusChanged, this, &Backend::setStatus);
    connect(client->tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(gotError(QAbstractSocket::SocketError)));
}

void Backend::setFilePath(const QString& filepath)
{
    this->filepath = filepath;
}

QString Backend::getFilePath(){
    return this->filepath;
}

void parseOut(QStringList& list)
{
    int max = 0;
    for (int i = 0; i < list.size(); i++)
    {
        if (list[i].length() > max)
            max = list[i].length();
    }
    for (int i = 0; i < list.size(); i++)
    {
        list[i] += QString(max - list[i].length(), ' ');
    }
}


void Backend::getRequests()
{
    
    QStringList list1;
    QStringList list2;
    QStringList list3;
    QStringList list4;
    list1.append("№");
    list2.append("Дата");
    list3.append("Адрес");
    list4.append(" Кол-во байт");
    for (auto it = this->client->userRequests.begin(); it != this->client->userRequests.end(); it++){
        list1.append(it.key());
        list2.append(it.value()["data"]);
        list3.append(it.value()["addr"]);
        list4.append(it.value()["bytes"]);
    }
    parseOut(list1);
    parseOut(list2);
    parseOut(list3);
    for (int i = 0; i < list1.size(); i++)
    {
        emit someStatistic(list1[i] + " | " + list2[i] + " | " + list3[i] + " | " + list4[i]);
    }

}

void Backend::getCharCount()
{
    QStringList list1;
    QStringList list2;
    list1.append("Символ");
    list2.append("Кол-во");
    for (auto it = this->client->charCount.begin(); it!=this->client->charCount.end();it++){
        list1.append(it.key());
        list2.append(QString::number(it.value()));
    }
    parseOut(list1);
    for (int i = 0; i < list1.size(); i++)
    {
        emit someStatistic(list1[i] + " | " + list2[i]);
    }
}

void Backend::getWordsLen()
{
    QStringList list1;
    QStringList list2;
    list1.append("Длина слова");
    list2.append("Кол-во слов");
    for (auto it = this->client->wordsLen.begin(); it!=this->client->wordsLen.end();it++){
        list1.append(it.key());
        list2.append(QString::number(it.value()));
    }
    parseOut(list1);
    for (int i = 0; i < list1.size(); i++)
    {
        emit someStatistic(list1[i] + " | " + list2[i]);
    }
}



bool Backend::getStatus()
{
    return client->getStatus();
}

void Backend::setStatus(bool newStatus)
{
    if (newStatus)
        { emit statusChanged("CONNECTED"); }
    else
        { emit statusChanged("DISCONNECTED"); }
}

void Backend::receivedSomething(QString msg)
{
    emit someMessage(msg);
}

void Backend::gotError(QAbstractSocket::SocketError err)
{
    QString strError = "unknown";
    switch (err)
    {
        case 0:
            strError = "Connection was refused";
            break;
        case 1:
            strError = "Remote host closed the connection";
            break;
        case 2:
            strError = "Host address was not found";
            break;
        case 5:
            strError = "Connection timed out";
            break;
        default:
            strError = "Unknown error";
    }

    emit someError(strError);
}

void Backend::connectClicked()
{
    client->connect2host();
}

void Backend::sendClicked(int req)
{
    QJsonObject json;
    json["request"] = req;
    if (req == 0 || req == 1){
        QFile file(filepath);
        if (!file.open(QIODevice::ReadOnly))
        {
            emit someError("Could not open file");
            return;
        }
        QString fileContent = file.readAll();
        json["file"] = fileContent;
    }
    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson();
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out << quint16(0) << jsonData;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    client->tcpSocket->write(arrBlock);
}

void Backend::disconnectClicked()
{
    client->closeConnection();
}
