#include <QGraphicsView>
#include <QScrollBar>
#include <QGraphicsView>
#include <QDebug>
#include <QDateTime>
#include <qmath.h>

#include "visualiser.h"

#include "scene/figures/node.h"
#include "scene/figures/arc.h"

#include "scene/pixmapholder.h"

#include "plugins/request.h"
#include "plugins/interfaces/iplugin.h"

#include "qvmgraph.h"

Visualiser::Visualiser()
{
    m_mainWidget = new MainWidget;

    // подключение слотов Visualiser-a
    QObject::connect(this, SIGNAL(runCommand(Data)), this, SLOT(onRunCommand(Data)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(setNodesCount(int)), m_mainWidget, SLOT(onSetNodesCount(int)));
    QObject::connect(this, SIGNAL(setLogMessages(QString)), m_mainWidget, SLOT(onSetLogMessages(QString)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(setWindowState(Qt::WindowState)), m_mainWidget, SLOT(onSetWindowState(Qt::WindowState)));

    QObject::connect(m_mainWidget, SIGNAL(runModel(QString)), this, SLOT(onRunModel(QString)));
    QObject::connect(m_mainWidget, SIGNAL(changeModel(QString)), this, SLOT(onChangeModel(QString)));
    QObject::connect(m_mainWidget, SIGNAL(clearScene()), this, SLOT(onClearScene()));
    QObject::connect(m_mainWidget, SIGNAL(closeAllNodes()), this, SLOT(onCloseAllNodes()));
    QObject::connect(m_mainWidget, SIGNAL(closeGUI()), this, SLOT(onCloseGUI()));

    m_scene = m_mainWidget->getScene();
    m_mainWidget->show();

    registerCommands();

    m_timer.start(300, this);
}

Visualiser::~Visualiser()
{
    onClearScene();
}

QString Visualiser::aboutInfo() const
{
    return "A plugin for visualise input data";
}

void Visualiser::registerCommands()
{
    m_commander.setClassID(getID());
    m_commander.registerCommand("showLogMessage", &Visualiser::onShowLogMessage);
    m_commander.registerCommand("addItem", &Visualiser::onAddItem);
    m_commander.registerCommand("removeItem", &Visualiser::onRemoveItem);
    m_commander.registerCommand("addNodeMenu", &Visualiser::onAddNodeMenu);
    m_commander.registerCommand("setGUIproperties", &Visualiser::onSetGUIproperties);
    m_commander.registerCommand("clearScene", &Visualiser::onClearScene);
    m_commander.registerCommand("addModelNameToList", &Visualiser::onAddModelName);
    //m_commander.registerCommand("addElementGUI", &Visualiser::addElementGUI);
    m_addItemCommander.registerCommand("node", &Visualiser::addNode);
    m_addItemCommander.registerCommand("arc", &Visualiser::addArc);
    m_removeItemCommander.registerCommand("node", &Visualiser::removeNode);
    m_removeItemCommander.registerCommand("arc", &Visualiser::removeArc);
}

#include <QLineEdit>

void Visualiser::onSignalFromElementGUI() {

    Data data;

    foreach (QWidget * widget, m_elementsGUI.keys()) {
        GUI_ElementProperties ep;
        ep.fromData(m_elementsGUI[widget]);

        if(ep.widgetType == "LineEdit") {
            QLineEdit * lineEdit = dynamic_cast<QLineEdit*>(widget);
            if(lineEdit != 0)
                ep.value = lineEdit->text();
        }

        data[ep.variableName] = ep.value;
    }

    Request rp;
    rp.senderID = "Visualiser";
    rp.receiverID = m_mainWidget->getCurrentModel();
    rp.command = "SetData";
    rp.value = data;

    emit request(rp.toData());
}

void Visualiser::onMenuItemSelected(const QString & nodeName, const QString & menuItem)
{
    Request rp;
    rp.senderID = "Visualiser";
    rp.receiverID = nodeName;
    rp.command = menuItem;//"MenuItemSelected";
    rp.value = menuItem;

    qDebug() << "selected:" << menuItem;

    emit request(rp.toData());
}

void Visualiser::onClearScene(const QVariant )
{    
    m_arcList.clear();
    m_rootNodes.clear();
    m_allNodes.clear();
    m_scene->clear();

    QRectF rect(m_scene->sceneRect());
    m_scene->update(rect);

    m_radius = 150;
}

void Visualiser::onRemoveElementsGUI()
{
    foreach (QWidget * widget, m_elementsGUI.keys()) {
        delete widget;
    }
    m_elementsGUI.clear();
}

void Visualiser::onAddModelName(const QVariant data)
{
    m_mainWidget->onAddModelName(data.toString());
}

void Visualiser::setNodesPosition() {

    //qDebug() << "place nodes";

    QList<Node *> parentNodes(m_rootNodes.getValues());
    int parentsCount = parentNodes.size();

    emit setNodesCount(parentsCount);

    m_radius =  qMax(m_radius, qMax(200, parentsCount * 8));

    qreal angle = -6.282 / parentsCount;

    if(parentsCount == 1) {
        Node * node = parentNodes[0];
        node->move(0, 0);
    }
    else {
        for(int i = 0; i < parentsCount; ++i) {

            Node * node = parentNodes[i];

            qreal nodeAngle = i * angle;
            qreal x = m_radius * qCos(nodeAngle);
            qreal y = m_radius * qSin(nodeAngle);

            qreal dx = x - node->x();
            qreal dy = y - node->y();

            node->move(dx, dy);
        }
    }

    //scene->update(scene->sceneRect());
}

void Visualiser::addNode(const Data & nodeData)
{
    //qDebug() << "node data:" << nodeData.toString();

    QString nodeName(nodeData["name"].toString());
    QString parentNodeName(nodeData["parent"].toString());

    if(!nodeName.isEmpty()) {        

        Node * node = m_allNodes.getValue(nodeName);
        if(!node) {

            node = new Node(nodeName);
            if(node) {

                // установка стиля отображения
                QString nodeStyle(nodeData["style"].toString());
                if(!nodeStyle.isEmpty())
                    node->setPixmap(PixmapHolder::getPixmapFrames(nodeStyle));
                else
                    node->setPixmap(PixmapHolder::getPixmapFrames("defaultNode"));

                node->addToScene(m_scene);

                connect(node, SIGNAL(menuSignal(QString,QString)), this, SLOT(onMenuItemSelected(QString, QString)));
                connect(node, SIGNAL(showNodeInfo(QString, QString)), this, SLOT(onShowNodeInfo(QString, QString)));
                connect(node, SIGNAL(nodeRemoved(Node*)), this, SLOT(onNodeRemoved(Node*)));
                connect(this, SIGNAL(animateItems()), node, SLOT(on_Animate()));

                if(!parentNodeName.isEmpty()) {
                      // получение родительского узла
                      Node * parentNode = m_allNodes.getValue(parentNodeName);

                      if(parentNode)
                           node->setParentNode(parentNode);
                }

                m_allNodes.add(nodeName, node);
            }
        }

        if(node) {

            // если у узла нет родителя
                        if(!node->hasParentNode()) {
                            QStringList nameParts = nodeName.split(".");
                            // если это не корневой узел
                            if(nameParts.size() > 1) {
                                // получение корневого узла
                                QString applicationNodeName = nameParts.value(0);
                                Node * applicationNode = m_allNodes.getValue(applicationNodeName);
                                // назначение корневого узла родительским
                                if(applicationNode) {
                                    //if(!applicationNode->hasChildNode())
                                       // addArc(QVMGraph::simpleArc(applicationNodeName, nodeName));

                                    node->setParentNode(applicationNode);
                                    addArc(QVMGraph::simpleArc(node->getParentNodeName(), nodeName));
                                }
                            }
                            else {
                                // если узел корневой и у него нет наследников
                                if(!node->hasChildNode()) {
                                    // добавление в список корневых узлов
                                    m_rootNodes.add(nodeName, node);
                                    setNodesPosition();
                                }
                            }
                        }

            if(parentNodeName != nodeName)
                addArc(QVMGraph::simpleArc(parentNodeName, nodeName));

            // добавление меню
            if(nodeData["menu"].canConvert<QStringList>()) {
                if(nodeData["menu"] != node->getMenu()) {
                    node->setMenu(nodeData["menu"].toStringList());
                }
            }

            // смена состояния узла
            QString nodeState(nodeData["nodeState"].toString());
            if(nodeState == "on" ||
               nodeState == "info" ||
               nodeState == "warning" ||
               nodeState == "notice" ||
               nodeState == "debug") node->setState(on);
            else
            if(nodeState == "err" ||
               nodeState == "crit" ||
               nodeState == "alert") node->setState(error);
            else
               node->setState(off);

            // добавление информации об узле
           // qDebug() << "add to node Info: " << nodeData["info"].toString();

            QString nodeInfo(nodeData
                             //.toString());//
                             ["info"].toString());

            if(!nodeInfo.isEmpty()) {
                node->setProperty("info", nodeInfo);
                // добавление подсказки
                node->setToolTip(nodeInfo);
                onShowLogMessage(nodeInfo);
            }
        }
    }
}

#include "scene/figures/arc.h"

void Visualiser::addArc(const Data & arcData)
{
    QString nodeFromName(arcData["from"].toString());
    QString nodeToName(arcData["to"].toString());

    if(!(nodeFromName.isEmpty() || nodeToName.isEmpty())) {
        //qDebug() << "Arc data:" << ArcData.toString();

        QString arcName(QVMGraph::arcName(nodeFromName,nodeToName));

        Arc * arc = m_arcList.getValue(arcName);

        if(!arc) {

            Node * from = m_allNodes.getValue(nodeFromName);
            Node * to = m_allNodes.getValue(nodeToName);

            if(from && to) {
                arc = new Arc(from, to);
                if(arc) {

                    connect(arc, SIGNAL(arcRemoved(Arc*)), this, SLOT(onArcRemoved(Arc*)));
                    connect(this, SIGNAL(animateItems()), arc, SLOT(on_Animate()));
                    connect(this, SIGNAL(queryArc(QString, long)), arc, SLOT(on_QueryArc(QString, long)));

                    m_arcList.add(arcName, arc);
                    //qDebug() << "new Arc created";
                }
                else
                    qDebug() << "error creating new Arc " << arcName;
            }
            else qDebug() << "error: node(s) not present for Arc " << arcName;
        }

        if(arc)
        {
             // добавление информации
             QString arcInfo(arcData["info"].toString());
             if(!arcInfo.isEmpty()) {

                 //Arc->setProperty("info", ArcInfo);
                // добавление подсказки
                arc->setToolTip(arcInfo);

                //onShowLogMessage(ArcData["info"]);
             }

             emit queryArc(arcName, arcData["showTimeCounter"].toLongLong());
        }
    }
}

void Visualiser::onAddItem(const QVariant data)
{
    Data itemData(data);
    m_addItemCommander.run(this, itemData["type"], itemData);
}

void Visualiser::onAddNodeMenu(const QVariant data)
{
    //qDebug() <<  data.toString();
    Request rp;
    rp.fromData(data);
    const QString nodeName(rp.senderID.toString());
    Node *node = m_allNodes.getValue(nodeName);
    if(node) {
        QStringList menuItems(rp.value.toStringList());
        node->setMenu(menuItems);
    }
}

void Visualiser::removeNode(const Data & data) {
    Node *node = m_allNodes.getValue(data["name"]);
    if(node) {
        //qDebug() << "found " << node->getName() << ", deleting...";
        node->deleteLater();
    }
}

void Visualiser::removeArc(const Data & data) {

    QString nodeFromName(data["from"].toString());
    QString nodeToName(data["to"].toString());

    if(!(nodeFromName.isEmpty() || nodeToName.isEmpty())) {
        qDebug() << "Arc data:" << data.toString();

        QString arcName(QVMGraph::arcName(nodeFromName,nodeToName));

        Arc * arc = m_arcList.getValue(arcName);
        if(arc) {
            //qDebug() << "found " << arcName << ", deleting...";
            arc->deleteLater();
        }
    }
}

void Visualiser::onRemoveItem(const QVariant data)
{
    Data itemData(data);
    //qDebug() << itemData.toString();

    if(m_removeItemCommander.run(this, itemData["type"], itemData))
        setNodesPosition();
}

void Visualiser::onNodeRemoved(Node * node)
{
    //qDebug() << "node removed";

    if(node) {
        const QString nodeName(node->getName());

        m_rootNodes.remove(node);
        m_allNodes.remove(node);

        qDebug() << nodeName << "node removed from rootNodes and allNodes";

        setNodesPosition();

        //qDebug() << "nodes position set, emit signal...";

        emit nodeRemoved(nodeName);
    }
}

void Visualiser::onArcRemoved(Arc * arc)
{
    //qDebug() << "Arc removed";

    if(arc) {
        QString arcName(arc->objectName());

        m_arcList.remove(arcName);
        qDebug() << arcName << "removed from ArcList, emit signal...";

        emit arcRemoved(arcName);
    }
}

void Visualiser::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timer.timerId()) {
        emit animateItems();

    } else {
        QObject::timerEvent(event);
    }
}

void Visualiser::onShowLogMessage(const QVariant message)
{
    emit setLogMessages(message.toString());
}


void Visualiser::onParsedProperty(const QString &name, const QVariant &value)
{
    //process some special properties:
    qDebug() << "property" << name << "=" << value;
}

void Visualiser::onBroadcast(const QVariant inputData)
{
    Data data(inputData);
    qDebug() << data.toString();
}

void Visualiser::onSetGUIproperties(const QVariant data)
{
    qDebug() << "setup GUI...";

    // настройка параметров визуализации
    m_visualiseProperties.fromData(data);

    // настройка параметров главного окна программы
    GUI_ApplicationProperties ga;
    ga.fromData(data);

    switch(ga.state) {
        case GUI_ApplicationProperties::maximize:
            qDebug() << "MAXIMIZE!";
            emit setWindowState(Qt::WindowMaximized);
        break;
        case GUI_ApplicationProperties::minimize:
            qDebug() << "MINIMIZE!";
            emit setWindowState(Qt::WindowMinimized);
        break;
        default:           
            break;
    }

    // настройка сплиттера главного окна
    GUI_SplitterProperties sp;
    sp.fromData(data);

    if(sp.bottomPanelHeight ||
       sp.mainScreenHeight) {

        QVector<int> widgetSizes;
        widgetSizes.resize(3);

        widgetSizes[0] = sp.mainScreenHeight;
        widgetSizes[1] = sp.bottomPanelHeight;

        m_mainWidget->onSetSplitterSizes(widgetSizes.toList());
    }
    Data inputData(data);
    Data elementsGUI = inputData["elementsGUI"];
    //qDebug() << qPrintable(elementsGUI.toString());

    foreach(const QString & key, elementsGUI.keys()) {
        //qDebug() << key << elementsGUI[key];
        onAddElementGUI(elementsGUI[key]);
    }
}
void Visualiser::onAddElementGUI(const Data data)
{
    qDebug() << "GUI element data " << qPrintable(data.toString());
    GUI_ElementProperties ep;
    ep.fromData(data);

    QWidget * widget = 0;

    if(ep.widgetType == "Label") {
        QLabel * label = new QLabel;
        label->setText(ep.value.toString());
        widget = label;
    }
    else
    if(ep.widgetType == "LineEdit") {
        QLineEdit * lineEdit = new QLineEdit;
        if(lineEdit) {
            connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(onSignalFromElementGUI()));
            connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(onSignalFromElementGUI()));
            lineEdit->setText(ep.value.toString());
            widget = lineEdit;
        }
    }
    else
    if(ep.widgetType == "PushButton") {
        QPushButton * pushButton = new QPushButton;
        if(pushButton) {
            connect(pushButton, SIGNAL(released()), this, SLOT(onSignalFromElementGUI()));
            pushButton->setText(ep.value.toString());
            widget = pushButton;
        }
    }

    if(widget) {
        m_elementsGUI.insert(widget, data);
        m_mainWidget->onAddElementGUI(widget);
        widget->show();
    }
}
void Visualiser::onRunModel(const QString & modelName)
{
    Request requestParams;
    requestParams.receiverID = modelName;
    requestParams.senderID = "Visualiser";
    requestParams.command = "Run";
    onClearScene();

    emit request(requestParams.toData());
}

void Visualiser::onChangeModel(const QString & modelName)
{
    m_mainWidget->setModelOptionsHidden(true);

    Request requestParams;
    requestParams.receiverID = modelName;
    requestParams.senderID = "Visualiser";
    requestParams.command = "Load";
    onClearScene();
    onRemoveElementsGUI();

    emit request(requestParams.toData());
}

void Visualiser::onCloseAllNodes()
{
    QList<QVariant> nodeNames(m_rootNodes.getKeys());
    foreach (const QVariant & nodeName, nodeNames) {
        Request rp;
        rp.senderID = "Visualiser";
        rp.receiverID = nodeName;
        rp.command = "Shutdown";
        rp.value = "Shutdown";
        emit request(rp.toData());
    }
}

void Visualiser::onCloseGUI()
{
    qDebug() << "main widget closed, exiting...";
    deleteLater();
}

void Visualiser::on_comboBox_currentIndexChanged(const QString & modelName)
{
    Request rp;
    rp.receiverID = modelName;
    rp.senderID = "Visualiser";
    rp.command = "Load";
    onClearScene();
    onRemoveElementsGUI();

    emit request(rp.toData());
}

void Visualiser::on_pbClear_released()
{
    onClearScene();
}

void Visualiser::onShowNodeInfo(const QString & nodeName, const QString & nodeInfo)
{
    qDebug() << "Its OK, node " << nodeName << " info:" << qPrintable(nodeInfo);
    QVariant info(nodeInfo);
    onShowLogMessage(info);
}
