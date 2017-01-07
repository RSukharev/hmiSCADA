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

private slots:    
    void parseReceived(QByteArray receivedData);

signals:
    void parsedData(const QVariant data);

private:    
    QVector<UdpSocket *> udpSocketList;
    QVector<QThread *> threadList;
};

#endif // UDPRECEIVER_H
