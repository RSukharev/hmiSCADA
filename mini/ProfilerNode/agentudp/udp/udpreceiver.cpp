#include <QtCore/QThread>
#include <QDataStream>
#include "udpreceiver.h"

#include "../Logger/ConsoleAppender.h"

UdpReceiver::UdpReceiver(QObject * parent) :
    QObject(parent)
{ }

UdpReceiver::~UdpReceiver() {

    foreach(UdpSocket * socket, udpSocketList)
        if(socket) socket->deleteLater();

    foreach(QThread * thread, threadList)
        if(thread)
            thread->deleteLater();
}

quint16 UdpReceiver::listen(const QString &address, quint16 initPort)
{
    UdpSocket * udpSocket = new UdpSocket(address, initPort);

    quint16 port = 0;

    if(udpSocket && udpSocket->getPort() < 10000)
    {
        port = udpSocket->getPort();

        connect(udpSocket, SIGNAL(received(QByteArray)),
                                    this, SLOT(parseReceived(QByteArray)));

        udpSocketList.append(udpSocket);

        QThread * thread = new QThread;

        if(thread) {

            udpSocket->moveToThread(thread);
            thread->start();

            threadList.append(thread);
        }
    }
    else {
        qDebug() << "Error init UDP socket, exiting...";
        exit(1);
    }

    return port;
}

void UdpReceiver::parseReceived(QByteArray receivedData) {

    QDataStream in(&receivedData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_7);

    QVariant data;
    in >> data;

    emit parsedData(data);
}


