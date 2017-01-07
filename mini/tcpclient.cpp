#include "tcpclient.h"

#include "../Logger/ConsoleAppender.h"

#define PROFILE_ENABLE
#include "Profiler/profiler.h"

#define PROFILE_BEGINNER PROFILE_BEGIN(appID, " "); PROFILE_START(Q_FUNC_INFO);
#define PROFILE_ENDER PROFILE_STOP();

#include <QDataStream>
#include "udpsignal.h"

void TcpClient::sendMessage(const TcpData data, const quint32 ip, quint16 port)
{
    PROFILE_BEGINNER;

    QTcpSocket socket;

    QHostAddress ha;
    ha.setAddress(ip);

    socket.connectToHost(ha, port);
    if(socket.waitForConnected(300)) {
        QByteArray array;
        QDataStream out(&array, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_2);
        out <<quint16(0) << data.m_type << data.m_value;
        out.device()->seek(0);
        out<<quint16(array.size()-sizeof(qint16));
        socket.write(array);
        socket.disconnectFromHost();
        socket.waitForDisconnected();
    }

    PROFILE_ENDER;
}
