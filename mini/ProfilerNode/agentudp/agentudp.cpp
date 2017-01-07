#include "agentudp.h"

#include "../../../Logger/ConsoleAppender.h"
#include "../../Manager/applicationmanager.h"

#include <QMessageBox>
#include <QtConcurrentRun>
#include <QTimerEvent>
#include <QNetworkInterface>


AgentUdp::AgentUdp(const Data & node) :
    m_id(node["name"].toString()),
    m_udpReceiver(0),
    m_agentNode(node)
{
    qDebug() << this << "agent created";

    registerCommands();

    m_agentNetworkProperties.ipAddress = getSelfIP();
    m_agentNetworkProperties.port = 5001;
    m_agentNetworkProperties.protocol = "udp";

    m_udpReceiver = new UdpReceiver;

    if(m_udpReceiver) {
        connect(m_udpReceiver, SIGNAL(parsedData(QVariant)), this, SLOT(processData(QVariant)));
        m_agentNetworkProperties.port = m_udpReceiver->listen(m_agentNetworkProperties.ipAddress,
                            m_agentNetworkProperties.port);

        qDebug() << "udpReceiver listen at " << m_agentNetworkProperties.ipAddress <<
                    ":" << m_agentNetworkProperties.port;
    }
    else
        qDebug() << "udpReceiver error!!! " << m_agentNetworkProperties.ipAddress <<
                    ":" << m_agentNetworkProperties.port;

    assert(!m_agentNode.isEmpty());
    m_agentNode["NetworkProperties"] = m_agentNetworkProperties.toData();

    m_scadaNetworkProperties.ipAddress = "255.255.255.255";
    m_scadaNetworkProperties.port = 5000;
    m_scadaNetworkProperties.protocol = "udp";

    addNode();

    m_timer.start(1000, this);
}

AgentUdp::~AgentUdp()
{
    qDebug() << "agent destroyed";
    if(m_udpReceiver) {
        m_udpReceiver->deleteLater();
    }
}

QString AgentUdp::getSelfIP() {

    QString address = QHostAddress(QHostAddress::LocalHost).toString();

    QList<QHostAddress> addr = QNetworkInterface::allAddresses();
    foreach(const QHostAddress & a, addr) {
        if(a.toString().contains(".") &&
           a != QHostAddress::LocalHost) {
            address = a.toString();
            break;
        }
    }

    return address;
}

void AgentUdp::sendRequestData(const QVariant & data)
{
    //Data sendData(data);
    //qDebug() << qPrintable(sendData.toString());

    UdpSender udpSender(
                m_scadaNetworkProperties.ipAddress,
                m_scadaNetworkProperties.port);

    udpSender.sendData(data);
}

void AgentUdp::addNode()
{
    if(!m_agentNode.isEmpty()) {
        Request request(parametersForVisualise());
        request.command = "addItem";
        request.value = m_agentNode;

        //qDebug() << qPrintable(agentNode.toString());

        sendRequestData(request.toData());
    }
}

void AgentUdp::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timer.timerId())
        addNode();
    else
        QObject::timerEvent(event);
}


void AgentUdp::setMenu() {
    m_agentNode["menu"] = m_commander.getCommands();
    qDebug() << qPrintable(m_agentNode.toString());
}

void AgentUdp::processData(const QVariant &data) {
    //qDebug() << qPrintable(data.toString());
    emit dataReceived(data);
}

void AgentUdp::onRunCommand(const Data data) {

    QString myID(getID().toString());
    qDebug() << myID << "received command:" << qPrintable(data.toString());

    QString receiverID(data["receiverID"].toString());

    if(receiverID == "Multicast" || receiverID.contains(myID))
        m_commander.run(this, data["command"], data["value"]);
}

void AgentUdp::registerCommands()
{
    QObject::connect(this, SIGNAL(dataReceived(Data)), this, SLOT(onRunCommand(Data)));

    m_commander.registerCommand("First remote command", &AgentUdp::onFirstRemoteCommand);
    m_commander.registerCommand("Second remote command", &AgentUdp::onSecondRemoteCommand);
    m_commander.registerCommand("Shutdown", &AgentUdp::onExitProgramCommand);
}

void AgentUdp::onFirstRemoteCommand(const QVariant data) {
    qDebug() << "received text: " << qPrintable(data.toString());

    QString message = "\"" + data.toString() + "\" command received by " + getID().toString();

    sendTextMessage(message);

    //QtConcurrent::run(this,&AgentUdp::showMessageBox, message);
    showMessageBox(message);
}

void AgentUdp::onSecondRemoteCommand(const QVariant data) {
    qDebug() << "received text: " << qPrintable(data.toString());

    QString message = "\"" + data.toString() + "\" command received by " + getID().toString();

    sendTextMessage(message);

    //QtConcurrent::run(this,&AgentUdp::showMessageBox, message);
    showMessageBox(message);
}

void AgentUdp::onExitProgramCommand(const QVariant data) {
    QString message = "\"" + data.toString() + "\" command received, " +
            getID().toString() + " has turned off";

    sendTextMessage(message);

    removeMe();

    exit(1);
}

void AgentUdp::showMessageBox(const QString message) {
    QMessageBox::information( 0, "Mini", message, QMessageBox::Ok, QMessageBox::Ok);
}
