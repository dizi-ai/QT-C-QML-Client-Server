#include "client.h"

Client::Client(const QString hostAddress, int portNumber) : QObject(), m_nNextBlockSize(0)
{
    status = false;
    tcpSocket = new QTcpSocket();

    host = hostAddress;
    port = portNumber;

    timeoutTimer = new QTimer();
    timeoutTimer->setSingleShot(true);
    connect(timeoutTimer, &QTimer::timeout, this, &Client::connectionTimeout);

    connect(tcpSocket, &QTcpSocket::disconnected, this, &Client::closeConnection);
}

void Client::connect2host()
{
    timeoutTimer->start(3000);

    tcpSocket->connectToHost(host, port);
    connect(tcpSocket, &QTcpSocket::connected, this, &Client::connected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Client::readyRead);
}

void Client::connectionTimeout()
{
    if(tcpSocket->state() == QAbstractSocket::ConnectingState)
    {
        tcpSocket->abort();
        emit tcpSocket->QAbstractSocket::errorOccurred(QAbstractSocket::SocketTimeoutError);
    }
}

void Client::connected()
{
    status = true;
    emit statusChanged(status);
}

bool Client::getStatus() {return status;}

void Client::setResult(QMap<QString, QString>& result, bool what)
{
    QMap<QString, int> res;
    for (auto it = result.begin(); it != result.end(); it++){
        res[it.key()] = it.value().toInt();
    }
    if (what==true)
        this->charCount = res;
    else
        this->wordsLen = res;
}

void Client::readyRead()
{
    QDataStream in(tcpSocket);
    for (;;)
    {
        if (!m_nNextBlockSize)
        {
            if (tcpSocket->bytesAvailable() < sizeof(quint16)) { break; }
            in >> m_nNextBlockSize;
        }

        if (tcpSocket->bytesAvailable() < m_nNextBlockSize) { break; }
        QByteArray data;
        in >> data;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        QJsonObject jsonObj = jsonDoc.object();


        if (jsonObj.contains("message"))
        {
            QString str = jsonObj["message"].toString();
            if (str=="0") {
            str = "Connection closed";
            closeConnection();
            }
            emit hasReadSome(str);
        }
        else {
            if (jsonObj["request"] == 0 || jsonObj["request"] == 1)
            {
                QVariantMap map = jsonObj["result"].toObject().toVariantMap();
                QMap<QString, QString> map2;
                for (auto it = map.begin(); it != map.end(); ++it)
                {
                    map2[it.key()] = it.value().toString();
                }
                if (jsonObj["request"] == 0)
                {
                    setResult(map2, true);
                    emit hasReadSome("Получен ответ на запрос о количестве каждого символа в файле");
                }
                else
                {
                    setResult(map2, false);
                    emit hasReadSome("Получен ответ на запрос о длине слов в файле");
                }
            }
            else {
                QVariantMap temp = jsonObj["result"].toVariant().toMap();
                QMap<QString, QMap<QString,QString>> map;
                int i = 0;
                for (auto it = temp.begin(); it != temp.end(); ++it, ++i)
                {
                    QStringList list = it.value().toString().split(' ');
                    QMap<QString, QString> map2;
                    map2["data"] = list[0];
                    map2["addr"] = list[1];
                    map2["bytes"] = list[2];
                    map[QString::number(i)] = map2;
                }
                this->userRequests = map;
                emit hasReadSome("Получен ответ на запрос об отправленных запросах");
            }
        }
        m_nNextBlockSize = 0;
    }
}

void Client::closeConnection()
{
    timeoutTimer->stop();

    disconnect(tcpSocket, &QTcpSocket::connected, 0, 0);
    disconnect(tcpSocket, &QTcpSocket::readyRead, 0, 0);

    bool shouldEmit = false;
    switch (tcpSocket->state())
    {
        case 0:
            tcpSocket->disconnectFromHost();
            shouldEmit = true;
            break;
        case 2:
            tcpSocket->abort();
            shouldEmit = true;
            break;
        default:
            tcpSocket->abort();
    }

    if (shouldEmit)
    {
        status = false;
        emit statusChanged(status);
    }
}
