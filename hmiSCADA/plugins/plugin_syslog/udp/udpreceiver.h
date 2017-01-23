#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <QObject>
#include <QByteArray>
#include <QVariantMap>
#include <QVector>

#include "udpsocket.h"

class QThread;

class UdpReceiver : public QObject
{
    Q_OBJECT

public:
    UdpReceiver(QObject *parent = 0);
    ~UdpReceiver();
    quint16 listen(const QString & address = "0.0.0.0", quint16 port = 5000);
    quint16 getPort() const;

private slots:
    void parseReceived(const QByteArray received);

signals:
    void receivedBytes(const QByteArray data);

private:
    QVector<UdpSocket *> m_udpSocketList;
    QVector<QThread *> m_threadList;
    quint16 m_port;
};

#endif // UDPRECEIVER_H
