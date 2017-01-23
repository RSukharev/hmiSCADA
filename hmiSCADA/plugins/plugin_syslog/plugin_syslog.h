#ifndef PLUGIN_SYSLOG_H
#define PLUGIN_SYSLOG_H

#include "../interfaces/imodel.h"
#include "../interfaces/iobject.h"
#include "../interfaces/iplugin.h"
#include "../../commander.h"

#include <QtScript>

class UdpReceiver;

class Plugin_SysLog : public IModel, public IPlugin {
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin")
#endif
    Q_INTERFACES( IPlugin )

    Data createChildNode(
            const QString &parentName,
            const QString &childName,
            const QString &style = "");

public:
    ~Plugin_SysLog();

    void processData(const QVariant & data);

    Plugin_SysLog();

    QVariant getID() const;

    QString aboutInfo() const;

public slots:
    void processMulticastData(const QVariant & data);

private slots:
    void onReceivedBytes(const QByteArray data);

    void onRunCommand(const Data data);

    void onClearScene(const QVariant);

signals:
    void dataReceived(const Data data);

private:

    Data getLogValues(const QString &log);
    void loadScript(const QString &fileName);
    void registerCommands();
    void run(const QVariant data);

    QScriptEngine m_jsEngine;
    QScriptValue m_jsFunction;

    quint16 m_syslogPort;
    UdpReceiver * udpReceiver;

    Commander<Plugin_SysLog, const QVariant> m_commander;
};

#endif // PLUGIN_SYSLOG
