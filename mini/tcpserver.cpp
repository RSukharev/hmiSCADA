#include "tcpserver.h"

#include "../Logger/ConsoleAppender.h"

#define PROFILE_ENABLE
#include "Profiler/profiler.h"

#define PROFILE_BEGINNER PROFILE_BEGIN(appID, " "); PROFILE_START(Q_FUNC_INFO);
#define PROFILE_ENDER PROFILE_STOP();

TcpServer::TcpServer()
{
    PROFILE_BEGINNER;

    PROFILE_ENDER;
}

void TcpServer::setPort(quint16 port) {
    PROFILE_BEGINNER;

    if(m_server.listen(QHostAddress::Any, port))
    {
        connect(&m_server,SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
        qDebug() << "tcp server is listening on port " << port;
    }

    m_nextBlockSize = 0;

    PROFILE_ENDER;
}

#include <QDataStream>

void TcpServer::slotNewConnection()
{
    PROFILE_BEGINNER;

    QTcpSocket * socket = m_server.nextPendingConnection();
    connect(socket,SIGNAL(disconnected()),socket,SLOT(deleteLater()));
    connect(socket,SIGNAL(readyRead()),this,SLOT(slotReadClient()));

    PROFILE_ENDER;
}

void TcpServer::slotReadClient()
{
    PROFILE_BEGINNER;

    QTcpSocket * clientSocket = (QTcpSocket*)sender();
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_4_2);
    for(;;)
    {
        if(!m_nextBlockSize)
        {
            if(clientSocket->bytesAvailable() <sizeof(quint16)){
                break;
            }
        in >> m_nextBlockSize;
        }

        if(clientSocket->bytesAvailable() < m_nextBlockSize){
            break;
        }

        in >> m_received.m_type;
        in >> m_received.m_value;

        //qDebug() << "server received: " << received_.value;
        emit receivedSome();

        m_nextBlockSize = 0;
    }

    PROFILE_ENDER;
}
