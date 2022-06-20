#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QTcpSocket>
#include <QDataStream>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantMap>

class Client : public QObject
{
    Q_OBJECT

public:
    Client(const QString host, int port);

    QTcpSocket *tcpSocket;
    QMap<QString, QMap<QString,QString>> userRequests;
    QMap<QString, int> wordsLen;
    QMap<QString, int> charCount;
    bool getStatus();
    void setResult(QMap<QString, QString>& result, bool what);
public slots:
    void closeConnection();
    void connect2host();

signals:
    void statusChanged(bool);
    void hasReadSome(QString msg);
    void updateCharCount(QMap<QChar, int> qvm);
    void updateWordLen(QMap<int, int> qvm);

private slots:
    void readyRead();
    void connected();
    void connectionTimeout();

private:
    QString host;
    int port;
    bool status;
    quint16 m_nNextBlockSize;
    QTimer *timeoutTimer;
};

#endif // CLIENT_H
