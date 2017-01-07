#include <QTimerEvent>
#include <QDataStream>
#include "udpsignal.h"
#include "../Logger/ConsoleAppender.h"

#define PROFILE_ENABLE
#include "Profiler/profiler.h"

#define PROFILE_BEGINNER PROFILE_BEGIN(appID, " "); PROFILE_START(Q_FUNC_INFO);
#define PROFILE_ENDER PROFILE_STOP();

UdpSignal::UdpSignal() : m_serverSocketUDP(0)
{    
    PROFILE_BEGINNER;

    m_clientSocketUDP = new QUdpSocket(this);

    m_myInfo.m_ip = getSelfIP();

    connectUDPListener();

    m_timer.start(1000, this);

    PROFILE_ENDER;
}

QMap<QUuid, OnlinerInfo> UdpSignal::getOnliners() const {
    return m_onliners;
}

void UdpSignal::connectUDPListener() {

    PROFILE_BEGINNER;

    if(!m_serverSocketUDP) {

        m_serverSocketUDP = new QUdpSocket(this);
        if(!m_serverSocketUDP->bind(QHostAddress::Any, 8080, QUdpSocket::ShareAddress)) {
            qDebug() << "binding error";
            exit(0);
        }

        QObject::connect(m_serverSocketUDP, SIGNAL(readyRead()), this, SLOT(serverUdpRead()));
    }
    PROFILE_ENDER;
}

quint32 UdpSignal::getSelfIP() {

    PROFILE_BEGINNER;

    quint32 address = QHostAddress(QHostAddress::LocalHost).toIPv4Address();

    QList<QHostAddress> addr = QNetworkInterface::allAddresses();
    foreach(const QHostAddress & a, addr) {
        if(a.toString().contains(".") &&
           a != QHostAddress::LocalHost) {
            address = a.toIPv4Address();
            break;
        }
    }

    PROFILE_ENDER;

    return address;
}

void UdpSignal::sendMyInfo() {

    PROFILE_BEGINNER;

    //qDebug() << "sending info...";

    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);

    out << m_myInfo.m_id;
    out << m_myInfo.m_ip;
    out << m_myInfo.m_port;
    out << m_myInfo.m_name;
    out << 0;

    QHostAddress ha;
    if(m_myInfo.m_ip == QHostAddress(QHostAddress::LocalHost).toIPv4Address())
        ha.setAddress(m_myInfo.m_ip);
    else
        ha.setAddress("255.255.255.255");

    if(m_clientSocketUDP->writeDatagram(datagram, ha, 8080)==-1) {
            qDebug() << "write udp datagram error";
            PROFILE_ENDER;
            exit(0);
        }

    PROFILE_ENDER;
}

void UdpSignal::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timer.timerId())
        sendMyInfo();
    else
        QObject::timerEvent(event);
}

void UdpSignal::serverUdpRead() {

    PROFILE_BEGINNER;

    //qDebug() << "some data received";

    bool listIsChanged = false;

    while (m_serverSocketUDP->hasPendingDatagrams()) {

    QByteArray datagram;
    QHostAddress senderIP;
    quint16 senderPort;

    datagram.resize(m_serverSocketUDP->pendingDatagramSize());
    m_serverSocketUDP->readDatagram(datagram.data(), datagram.size(),
                                          &senderIP, &senderPort);

    QDataStream in(&datagram, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_7);

    OnlinerInfo oi;

    in >> oi.m_id;
    in >> oi.m_ip;
    in >> oi.m_port;
    in >> oi.m_name;

    if(!listIsChanged && (oi.m_id != m_myInfo.m_id)) {
        listIsChanged =
    ((m_onliners[oi.m_id].m_name != oi.m_name)||
     (m_onliners[oi.m_id].m_ip != oi.m_ip) ||
     (m_onliners[oi.m_id].m_port != oi.m_port) ||
        ((oi.m_ip == m_myInfo.m_ip) && (oi.m_port == m_myInfo.m_port)));
    }

    m_onliners[oi.m_id] = oi;

    }

    //qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!! refreshList = " << refreshList;

    if(listIsChanged) emit newMember();

    PROFILE_ENDER;
}
