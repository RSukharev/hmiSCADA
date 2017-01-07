#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QStringList>
#include <QDebug>

#include "tcpserver.h"

class TcpClient : public QObject
{
    Q_OBJECT

public slots:
    void sendMessage(const TcpData data, const quint32 ip, quint16 port);
};

#endif // TCPCLIENT_H
