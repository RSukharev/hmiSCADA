#include "pluginudp.h"
#include "udp/udpreceiver.h"
#include "udp/udpsender.h"
#include "../interfaces/inode.h"
#include "../../data.h"

#include "../../../Logger/ConsoleAppender.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    Q_EXPORT_PLUGIN2( IPlugin, PluginUdp )
#endif

PluginUdp::PluginUdp() : udpReceiver(0) {

    Logger::filterLevel(Logger::Debug);
    ConsoleAppender * consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat("[%-5l] %t{HH:mm:ss.zzz} %F line: %i  <%C> %m\n");
    Logger::registerAppender(consoleAppender);

    udpReceiver = new UdpReceiver;
    if(udpReceiver) {
        connect(udpReceiver, SIGNAL(parsedData(QVariant)), this, SLOT(receivedNetworkData(QVariant)));
        udpReceiver->listen();
    }
}

QVariant PluginUdp::getID() const { return "PluginUdp"; }

QString PluginUdp::aboutInfo() const {
    QString port = "5000";
    if(udpReceiver)
        port = QString::number(udpReceiver->getPort());

    return "plugin for input data by udp protocol, port " + port;
}

PluginUdp::~PluginUdp()
{
    if(udpReceiver) {
        udpReceiver->deleteLater();
    }
}

void PluginUdp::processData(const QVariant & data)
{
    Request rp;
    rp.fromData(data);
    //qDebug() << "process data: " << rp.toData().toString();
    //qDebug() << "receiver id: " << rp.receiverID;

    QString receiverID(rp.receiverID.toString());
    if(receiverID == "Multicast")
        processMulticastData(data);
    else
        sendToNode(receiverID, data);
}

void PluginUdp::sendToNode(const QString & nodeName, const QVariant & data) {
    NetworkProperties nodeNetworkProperties = nodes[nodeName];

    //qDebug() << "node network properties: " << nodeNetworkProperties.toData().toString();

    if(nodeNetworkProperties.protocol == "udp") {
        sendNetworkData(nodeNetworkProperties.ipAddress, nodeNetworkProperties.port, data);
    }
}

void PluginUdp::sendNetworkData(const QString & ipAddress, int port, const QVariant & data)
{
    Data sendData(data);
    qDebug() << "send data: " << sendData.toString()
                   << " to " << ipAddress << ":" << QString::number(port);

    UdpSender udpSender(
                ipAddress,
                port);

    udpSender.sendData(data);
}

void PluginUdp::processMulticastData(const QVariant & inputData) {

    const QStringList & nodeNames = nodes.keys();
    foreach(const QString & nodeName, nodeNames)
        sendToNode(nodeName, inputData);
}

void PluginUdp::receivedNetworkData(const QVariant & data)
{
    Data requestData(data);
    //qDebug() << "received data: " << requestData.toString();

    Data value(requestData["value"]);
    if(value["NetworkProperties"].isValid()) {

        QString nodeName(value["name"].toString());

        NetworkProperties nodeNetworkProperties;
        nodeNetworkProperties.fromData(value["NetworkProperties"]);
        nodes.insert(nodeName, nodeNetworkProperties);

        /*
        const QStringList & nodeNames = nodes.keys();
        qDebug() << "nodes count: " << nodeNames.size();

        foreach(const QString & name, nodeNames) {
            qDebug() << "node: " << name << "has network props:" << nodes[name].toData().toString();
        }
        */
    }

    requestData["pluginID"] = getID();

    emit request(requestData);
}


