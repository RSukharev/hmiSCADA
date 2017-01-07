#include "udpsender.h"
#include <QtNetwork/QHostAddress>
#include <QDataStream>

#include "../Logger/ConsoleAppender.h"

UdpSender::UdpSender(const QString & ip, quint16 _port, QObject * _parent) :
    QObject(_parent),
    udpSocket(0),
    hAddress(ip),
    port(_port)
{
    udpSocket = new QUdpSocket(this);
}

UdpSender::~UdpSender() {
    if(udpSocket) udpSocket->deleteLater();
}

void UdpSender::sendData(const QVariant data)
{
    if(udpSocket) {

        QByteArray datagram;
        QDataStream out(&datagram, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);

        out << data;
        out << 0;

        if(udpSocket->writeDatagram(datagram, hAddress, port) == -1)
            qDebug() << "UdpSender::writeToSocket error sending data";
        //else
          //  qDebug() << "data sent";
    }
}
