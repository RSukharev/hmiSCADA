#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QObject>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QUdpSocket>
#include <QVector>
#include <QByteArray>

class UdpSocket  : public QUdpSocket {
    Q_OBJECT

public:
    UdpSocket(const QString & address, quint16 port, QObject *parent = 0) :
        QUdpSocket(parent), m_port(port) {
        connect(this, SIGNAL(readyRead()), this, SLOT(readFromSocket()));

        for(;m_port < 10000; m_port++) {

            // reset socket
            abort();

            // try to connect to the next port
            if(bind(QHostAddress(address),
                    m_port, DontShareAddress)) break;
        }

        if(m_port == 10000) {
            qDebug() << "Error: cannot bind udp listener to ports, exiting...";
            exit(1);
        }
    }

    ~UdpSocket() {}

    quint16 getPort() const { return m_port; }

signals:
    void received(const QByteArray);

private slots:
    void readFromSocket() {

        while (hasPendingDatagrams()) {

            QHostAddress senderIP;
            quint16 senderPort;
            QByteArray datagram;

            datagram.resize(pendingDatagramSize());
            readDatagram(datagram.data(), datagram.size(),
                                              &senderIP, &senderPort);

            emit received(datagram);
        }
    }
private:
    quint16 m_port;
};

#endif // UDPSOCKET_H
