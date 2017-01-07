#include <QtCore/QThread>
#include <QDataStream>
#include "udpreceiver.h"

#include "../../Logger/ConsoleAppender.h"

UdpReceiver::UdpReceiver(QObject * parent) :
    QObject(parent), m_port(0)
{ }

UdpReceiver::~UdpReceiver() {

    foreach(UdpSocket * socket, m_udpSocketList)
        if(socket) socket->deleteLater();

    foreach(QThread * thread, m_threadList)
        if(thread)
            thread->deleteLater();
}

quint16 UdpReceiver::listen(const QString &address, quint16 initPort)
{
    UdpSocket * udpSocket = new UdpSocket(address, initPort);

    if(udpSocket && udpSocket->getPort() < 10000)
    {
        m_port = udpSocket->getPort();

        connect(udpSocket, SIGNAL(received(QByteArray)),
                                    this, SLOT(parseReceived(QByteArray)));

        m_udpSocketList.append(udpSocket);

        QThread * thread = new QThread;

        if(thread) {

            udpSocket->moveToThread(thread);
            thread->start();

            m_threadList.append(thread);
        }
    }
    else {
        qDebug() << "Error init UDP socket, exiting...";
        exit(1);
    }

    return m_port;
}

quint16 UdpReceiver::getPort() const { return m_port; }

void UdpReceiver::parseReceived(QByteArray receivedData) {

    QDataStream in(&receivedData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_7);

    QVariant data;
    in >> data;

    emit parsedData(data);
}


