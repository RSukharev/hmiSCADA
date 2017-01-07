#ifndef PLUGINUDP_H
#define PLUGINUDP_H

#include "../interfaces/inode.h"
#include "../interfaces/iobject.h"
#include "../interfaces/iplugin.h"

class UdpReceiver;

class PluginUdp : public IObject, public IPlugin {
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin")
#endif
    Q_INTERFACES( IPlugin )

public:
    ~PluginUdp();

    void processData(const QVariant & data);

    PluginUdp();

    QVariant getID() const;

    QString aboutInfo() const;

public slots:
    void processMulticastData(const QVariant & data);

private slots:
    void receivedNetworkData(const QVariant & data);

private:
    void sendToNode(const QString &nodeName, const QVariant &data);
    void sendNetworkData(const QString & ipAddress, int port, const QVariant & data);

    UdpReceiver * udpReceiver;
    QMap<QString, NetworkProperties> nodes;

};

#endif // PLUGINUDP_H
