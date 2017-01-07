#ifndef AGENTUDP_H
#define AGENTUDP_H

#include "udp/udpreceiver.h"
#include "udp/udpsender.h"
#include "udp/udpsocket.h"
#include "interfaces/inode.h"

#include <QBasicTimer>

#include "commander.h"

class AgentUdp : public INode, public IPlugin
{
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin")
#else
#endif
    Q_INTERFACES( IPlugin )

public slots:

    void setMenu();

    ////
    /// \brief processData - отправка сигнала о наличии новых данных для обработки
    /// \param data - данные для обработки
    ///
    void processData(const QVariant & data);

signals:
    void dataReceived(const Data data);

private slots:
    ///
    /// \brief onRunCommand - выполнение команды
    /// \param data
    ///
    void onRunCommand(const Data data);

public:

    // конструктор
    AgentUdp(const Data & m_agentNode = Data());

    // деструктор
    ~AgentUdp();

    // идентификатор плагина
    QVariant getID() const { return m_id; }

    // сетевые настройки агента
    NetworkProperties getNetworkProperties() const { return m_agentNetworkProperties; }

    // справочная информация о плагине
    QString aboutInfo() const { return "A SCADA system node with udp management interface"; }

public slots:

    // обработка данных, полученных из широковещательной рассылки
    virtual void processMulticastData(const QVariant &) {}

private slots:

    ////
    /// \brief timerEvent - слот для обновления информации об узле
    /// \param event - событие тамера
    ///
    void timerEvent(QTimerEvent *event);

    // команды на выполнение
    void onFirstRemoteCommand(const QVariant data);
    void onSecondRemoteCommand(const QVariant data);
    void onExitProgramCommand(const QVariant data);

protected:
    void registerCommands();
    void showMessageBox(const QString message);
    void addNode();
    void sendRequestData(const QVariant & data);

    QString getSelfIP();

    QString m_id;
    UdpReceiver * m_udpReceiver;
    Data m_agentNode;
    QBasicTimer m_timer;
    NetworkProperties m_agentNetworkProperties;
    NetworkProperties m_scadaNetworkProperties;
    Commander<AgentUdp, const QVariant> m_commander;
};

#endif // AGENTUDP_H
