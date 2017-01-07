#ifndef UDPSIGNAL_H
#define UDPSIGNAL_H

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QTcpSocket>
#include <QNetworkInterface>
#include <QMap>
#include <QUuid>
#include <QTimer>
#include <QDebug>

#include <QHostAddress>
#include <QUuid>

struct OnlinerInfo {

    OnlinerInfo() :
        m_id(QUuid::createUuid()),
        m_ip(0),
        m_port(0) { }

    QString ipAsString() const {
        QHostAddress a;
        a.setAddress(m_ip);
        return a.toString();
    }

    QString portAsString() const {
        return QString::number(m_port);
    }

    QUuid m_id;
    quint32 m_ip;
    int m_port;
    QString m_name;
};

class UdpSignal : public QObject
{
    Q_OBJECT

public:
    UdpSignal();
    QMap <QUuid, OnlinerInfo> getOnliners() const;

    OnlinerInfo getMyInfo() const { return m_myInfo; }
    void setMyInfo(OnlinerInfo info) { m_myInfo = info;}

public slots:
    void sendMyInfo();

    ////
    /// \brief timerEvent - слот отправки udp пакетов
    /// \param event - событие тамера
    ///
    void timerEvent(QTimerEvent *event);

private slots:
    void serverUdpRead();

signals:
    void newMember();

private:
    quint32 getSelfIP();

    void connectUDPListener();

    QUdpSocket * m_serverSocketUDP;
    QUdpSocket * m_clientSocketUDP;

    ////
    /// \brief timer - таймер
    ///
    QBasicTimer m_timer;

    QMap <QUuid, OnlinerInfo> m_onliners;

    OnlinerInfo m_myInfo;
};

#endif // UDPSIGNAL_H
