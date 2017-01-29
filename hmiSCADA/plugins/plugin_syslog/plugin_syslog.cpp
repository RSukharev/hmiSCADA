#include "plugin_syslog.h"
#include "../interfaces/inode.h"
#include "../../data.h"

#include "../../../Logger/ConsoleAppender.h"
#include "../../resourcepath.h"
#include "udp/udpreceiver.h"

#include <QUdpSocket>
#include <QHostAddress>
#include <QFlags>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    Q_EXPORT_PLUGIN2( IPlugin, Plugin_SysLog )
#endif


Plugin_SysLog::Plugin_SysLog() :
    m_syslogPort(5514),
    udpReceiver(0)
{

    registerCommands();

    // set global path for resources:
    ResourcePath::set(QCoreApplication::applicationDirPath());
    loadScript(ResourcePath::get()+"/plugins/syslogparser.js");

    Logger::filterLevel(Logger::Debug);
    ConsoleAppender * consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat("[%-5l] %t{HH:mm:ss.zzz} %F line: %i  <%C> %m\n");
    Logger::registerAppender(consoleAppender);

    udpReceiver = new UdpReceiver;
    if(udpReceiver) {
        connect(udpReceiver, SIGNAL(receivedBytes(QByteArray)), this, SLOT(onReceivedBytes(QByteArray)));
        udpReceiver->listen("0.0.0.0", m_syslogPort);
    }
}

QVariant Plugin_SysLog::getID() const { return "Plugin_SysLog"; }

QString Plugin_SysLog::aboutInfo() const {

    return "plugin for input data by udp protocol, port " + QString::number(m_syslogPort);
}

Plugin_SysLog::~Plugin_SysLog()
{
    udpReceiver->deleteLater();
}

void Plugin_SysLog::processData(const QVariant & data)
{
    emit dataReceived(data);
}

void Plugin_SysLog::processMulticastData(const QVariant & inputData) {

}

void Plugin_SysLog::registerCommands()
{
    QObject::connect(this, SIGNAL(dataReceived(Data)), this, SLOT(onRunCommand(Data)));

    m_commander.setClassID(getID());
    m_commander.registerCommand("Run", &Plugin_SysLog::run);
    m_commander.registerCommand("Shutdown", &Plugin_SysLog::onClearScene);
    //    m_commander.registerCommand("MenuItemSelected", &Plugin_SysLog::onMenuItemSelected);
}

void Plugin_SysLog::run(const QVariant data) {

    qDebug() << "hello???????" << data;

    showMessage(getID().toString() +
                "\nModel is loaded already\n" +
                aboutInfo());
}

void Plugin_SysLog::onClearScene(const QVariant)
{
    Request rp;
    rp.receiverID = "Visualiser";
    rp.command = "clearScene";
    rp.value = "clearScene";
    rp.senderID = getID();

    sendRequestData(rp.toData());
}

void Plugin_SysLog::onRunCommand(const Data data) {

    m_commander.run(this, data["command"], data["value"]);
}

void Plugin_SysLog::loadScript(const QString & fileName)
{
    QFile scriptFile(fileName);
    scriptFile.open(QIODevice::ReadOnly);
    QTextStream stream(&scriptFile);
    QString syslogParserScript = stream.readAll();
    scriptFile.close();

    m_jsEngine.evaluate(syslogParserScript);
    m_jsFunction = m_jsEngine.globalObject().property("parse");
}

Data Plugin_SysLog::getLogValues(const QString & log) {
    return m_jsFunction.call(QScriptValue(), QScriptValueList() << log).toVariant();
}

void Plugin_SysLog::onReceivedBytes(const QByteArray data) {

    //qDebug() << data;

    Data logValues = getLogValues(data.constData());

    //qDebug() << qPrintable(logValues.toString());

    QString nodeName = logValues["host"].toString();
    Data node = QVMGraph::simpleNode(nodeName, "PCcase");
    node["info"] = nodeName;

    addItem(node);

    Data childNode;
    QString severity = logValues["severity"].toString();
    childNode = createChildNode(nodeName, severity);
    childNode["info"] = logValues.toString();
    childNode["nodeState"] = severity;

    addItem(childNode);
}

Data Plugin_SysLog::createChildNode(
        const QString & parentName,
        const QString & childName,
        const QString & style) {

    Data childNode = QVMGraph::simpleNode(childName, style);

    childNode["parent"] = parentName;

    return childNode;
}

