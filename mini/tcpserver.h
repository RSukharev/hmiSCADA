#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>

struct TcpData {
    QString m_type;
    QByteArray m_value;
};

class TcpServer : public QObject
{
    Q_OBJECT

public:
    TcpServer();
    void setPort(quint16 port);
    TcpData getReceived() const { return m_received; }

private slots:
    void slotNewConnection();
    void slotReadClient();

signals:
    void receivedSome();

private:

    QTcpServer m_server;
    qint16 m_nextBlockSize;

    TcpData m_received;

};

#endif // TCPSERVER_H
