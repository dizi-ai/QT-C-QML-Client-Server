#include "serverStuff.h"



ServerStuff::ServerStuff() : QObject(), m_nNextBlockSize(0)
{
    tcpServer = new QTcpServer();
    db.connectToDataBase();
}

QList<QTcpSocket *> ServerStuff::getClients()
{
    return clients;
}

void ServerStuff::newConnection()
{
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    qDebug() << clientSocket->peerAddress().toString() << clientSocket->peerPort();

    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    connect(clientSocket, &QTcpSocket::readyRead, this, &ServerStuff::readClient);
    connect(clientSocket, &QTcpSocket::disconnected, this, &ServerStuff::gotDisconnection);

    clients << clientSocket;
    QJsonObject jo;
    jo["message"] = "Ответ: соединение установлено";
    QJsonDocument json(jo);

    sendToClient(clientSocket, json);
}

void ServerStuff::readClient()
{
    QTcpSocket *clientSocket = (QTcpSocket*)sender();
    QDataStream in(clientSocket);
    for (;;)
    {
        if (!m_nNextBlockSize)
        {
            if (clientSocket->bytesAvailable() < sizeof(quint16)) { break; }
            in >> m_nNextBlockSize;
        }

        if (clientSocket->bytesAvailable() < m_nNextBlockSize) { break; }
        QByteArray data;
        in >> data;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        QJsonObject jsonObj = jsonDoc.object();
        QJsonDocument result;
        int type = jsonObj["request"].toInt();
        QString client_addr = clientSocket->peerAddress().toString()+":"+QString::number(clientSocket->peerPort());
        if (type == 0 || type == 1){
            QString message = jsonObj["file"].toString();
            QVariantMap tores;
            if (type == 0){
                CharCount cc;
                tores = cc.countChars(message);
            }
            else if (type == 1){
                WordsLen wl;
                tores = wl.countLens(message);
            }
            result = QJsonDocument::fromVariant(tores);
            QVariantList qvl;
            qvl << QDateTime::currentDateTime();
            qvl.append(client_addr);
            qvl.append(message.length());
            this->db.insertInto(qvl);
        }
        else {
            QVariantMap qvm = db.select(client_addr);
            qDebug() << "qvm" << qvm;
            result = QJsonDocument::fromVariant(qvm);
        }
        QJsonObject jo;
        jo["request"] = type;
        jo["result"] =result.object();
        QJsonDocument json(jo);
        qDebug() << jo["result"];
        if (sendToClient(clientSocket, json) == -1)
        {
            qDebug() << "Ошибка при отправке сообщения";
        }

        QString msg = QString("from:")+clientSocket->localAddress().toString()+":"+clientSocket->localPort() +" "+type;
        emit gotNewMesssage(msg);

        m_nNextBlockSize = 0;
    }
}

void ServerStuff::gotDisconnection()
{
    clients.removeAt(clients.indexOf((QTcpSocket*)sender()));
    emit smbDisconnected();
}

qint64 ServerStuff::sendToClient(QTcpSocket *socket, const QJsonDocument &json)
{
    QByteArray jsonData = json.toJson();
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out << quint16(0) << jsonData;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    return socket->write(arrBlock);
}

