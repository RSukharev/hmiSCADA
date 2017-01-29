#include "compmodel.h"
#include "../../../qvmgraph.h"
#include "../../../scene/guiproperties.h"

#include <QDebug>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    Q_EXPORT_PLUGIN2( IPlugin, CompModel )
#endif

CompModel::CompModel() {
    registerCommands();
}

void CompModel::registerCommands()
{
    QObject::connect(this, SIGNAL(dataReceived(Data)), this, SLOT(onRunCommand(Data)));

    m_commander.setClassID(getID());
    m_commander.registerCommand("Run", &CompModel::run);
    m_commander.registerCommand("MenuItemSelected", &CompModel::onMenuItemSelected);
    m_commander.registerCommand("Shutdown", &CompModel::onClearScene);
}

void CompModel::onMenuItemSelected(const QVariant inputData)
{
    Request rp;
    rp.fromData(inputData);

    if(rp.value.toString() == "info")
       qDebug() << rp.receiverID << ":" << m_graph.getNode(rp.receiverID.toString()).value("info").toString();
    else
       qDebug() << "process menu item:" << rp.value.toString();
}

void CompModel::onClearScene(const QVariant)
{
    Request rp;
    rp.receiverID = "Visualiser";
    rp.command = "clearScene";
    rp.value = "clearScene";
    rp.senderID = getID();

    sendRequestData(rp.toData());
}

void CompModel::processData(const QVariant &data) {
    emit dataReceived(data);
}

void CompModel::processMulticastData(const QVariant & inputData)
{
    Request rp;
    rp.fromData(inputData);

    qDebug() << getID() << "::processData()" << rp.toData().toString();

    if(rp.receiverID == getID())
        processData(inputData);
}

void CompModel::onRunCommand(const Data data) {

    m_commander.run(this, data["command"], data["value"]);
}

Data CompModel::createChildNode(
        const QString & parentName,
        const QString & childName,
        const QString & style) {

    Data childNode = QVMGraph::simpleNode(childName, style);

    childNode["parent"] = parentName;

    return childNode;
}


// запуск модели
void CompModel::run(const QVariant)
{
    //qDebug() << getID() << "::Run()";
    showMessage(getID().toString() + " model run");

    // графические настройки
    GuiProperties guiProperties;

    // настройка параметров визуализации линков
    GUI_VisualiseProperties vs;
    vs.animation = true;
    vs.deleteAfterFading = true;
    vs.fadeEffectForLinks = false;
    guiProperties.add(vs.toData());

    // настройка интерфейса главного окна
    GUI_SplitterProperties gs;
    gs.mainScreenHeight = 600;
    gs.bottomPanelHeight = 70;
    guiProperties.add(gs.toData());

    Request rp =
            parametersForVisualise();

    rp.command = "setGUIproperties";
    rp.value = guiProperties;
    emit sendRequestData(rp.toData());

    // добавление моделируемых устройств

    Data srv = QVMGraph::simpleNode("SRV", "PCcase");
    srv["info"] = "small business server, 16Gb RAM, 1Tb HDD, Gigabit ethernet";

    m_graph.insertNode(srv);
    addItem(srv);

    Data db1 = QVMGraph::simpleNode("DB1", "DBnode");
    db1["info"] = "PostgreSQL server ver 8.0";

    m_graph.insertNode(db1);
    addItem(db1);

    int number = 4;

    Data pc = QVMGraph::simpleNode("PC", "PC");
    pc["info"] = "desktop workstation";

    for(int i = 0; i < number; ++i) {
        pc["name"] = QString("PC")+QString::number(i+1);
        m_graph.insertNode(pc);
        addItem(pc);
    }

    // добавление линков

    for(int i = 0; i < number; ++i) {
        for(int j = 0; j < number; ++j) {
            if(i != j) {

                QString from(QString("PC")+QString::number(i+1));
                QString to(QString("PC") + QString::number(j+1));

                m_graph.insertArc(from, to);
                addItem(QVMGraph::simpleArc(from, to));

                m_graph.insertArc(to, from);
                addItem(QVMGraph::simpleArc(to, from));

                Data link = QVMGraph::simpleArc("SRV", QString("PC")
                                                  + QString::number(j+1));
                addItem(link);
                to = "SRV";
                addItem(QVMGraph::simpleArc(from, to));
            }
        }
    }

    for(int i = 0; i < 10; ++i) {
        Data linkDbSrv = QVMGraph::simpleArc("DB1","SRV");
        addItem(linkDbSrv);

        Data linkSrvDb = QVMGraph::simpleArc("SRV","DB1");
        addItem(linkSrvDb);
    }

    addChildren("SRV", 2);

    addChildren("PC3", 3);

    addChildren("DB1", 5);

    Data flow = QVMGraph::simpleArc("DB1.child12", "PC3.child21");
    addItem(flow);

    Data flow3111 = QVMGraph::simpleArc("SRV.child31", "SRV.child11");
    addItem(flow3111);

    Data flow2110 = QVMGraph::simpleArc("SRV.child21", "SRV.child10");
    addItem(flow2110);

}

void CompModel::addChildren(const QString & nodeName, int childrenCount) {

    Data childNode;
    Data arc;

    for(int i = 0; i < childrenCount; ++i) {
        QString strNum(QString::number(i));
        childNode = createChildNode(nodeName, nodeName + ".child1" + strNum);
        childNode["info"] = "info "+ nodeName+".child1" + strNum;
        childNode["testEndTime"] = static_cast<long long>(1000);
        childNode["parentTestEndTime"] = 1500;

        addItem(childNode);

        arc = QVMGraph::simpleArc(nodeName, nodeName + ".child1" + strNum);
        arc["arctype"] = "flow";
        arc["info"] = QVMGraph::arcName(nodeName, nodeName+".child1" + strNum);
        addItem(arc);

        childNode = createChildNode(nodeName + ".child1" + strNum, nodeName + ".child2" + strNum);
        childNode["info"] = "info "+ nodeName+".child2" + strNum;
        childNode["testEndTime"] = static_cast<long long>(1001);
        childNode["parentTestEndTime"] = 1501;

        addItem(childNode);

        arc = QVMGraph::simpleArc(nodeName + ".child1" + strNum, nodeName + ".child2" + strNum);
        arc["arctype"] = "flow";
        arc["info"] = QVMGraph::arcName(nodeName + ".child1" + strNum, nodeName + ".child2" + strNum);
        addItem(arc);

        childNode = createChildNode(nodeName + ".child2" + strNum, nodeName + ".child3" + strNum);
        childNode["info"] = "info "+ nodeName+".child3" + strNum;
        childNode["testEndTime"] = static_cast<long long>(1002);
        childNode["parentTestEndTime"] = 1502;

        addItem(childNode);

        arc = QVMGraph::simpleArc(nodeName + ".child2" + strNum, nodeName + ".child3" + strNum);
        arc["arctype"] = "flow";
        arc["info"] = QVMGraph::arcName(nodeName + ".child2" + strNum, nodeName + ".child3" + strNum);
        addItem(arc);
    }
}
