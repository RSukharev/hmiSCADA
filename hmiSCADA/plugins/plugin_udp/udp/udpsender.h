#ifndef UDPSENDER_H
#define UDPSENDER_H

#include <QObject>
#include <QByteArray>
#include <QtNetwork/QUdpSocket>

class UdpSender : public QObject
{
    Q_OBJECT

public:
    UdpSender(const QString & _ip = "255.255.255.255",
              quint16 _port = 5000, QObject *parent = 0);

    ~UdpSender();

public slots:
    void sendData(const QVariant data);

private:
    QUdpSocket * udpSocket;
    QHostAddress hAddress;
    quint16 port;
};

#endif // UDPSENDER_H
