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
    mainWidget = new MainWidget;

    // подключение слотов Visualiser-a
    QObject::connect(this, SIGNAL(runCommand(Data)), this, SLOT(onRunCommand(Data)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(updateLog()), this, SLOT(onUpdateLog()), Qt::QueuedConnection);
    QObject::connect(mainWidget, SIGNAL(runModel(QString)), this, SLOT(onRunModel(QString)));
    QObject::connect(mainWidget, SIGNAL(changeModel(QString)), this, SLOT(onChangeModel(QString)));
    QObject::connect(mainWidget, SIGNAL(clearScene()), this, SLOT(onClearScene()));
    QObject::connect(mainWidget, SIGNAL(closeAllNodes()), this, SLOT(onCloseAllNodes()));
    QObject::connect(this, SIGNAL(setNodesCount(int)), mainWidget, SLOT(onSetNodesCount(int)));

    scene = mainWidget->getScene();
    mainWidget->show();

    registerCommands();

    timer.start(300, this);
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
    commander.registerCommand("showLogMessage", &Visualiser::onShowLogMessage);
    commander.registerCommand("addItem", &Visualiser::onAddItem);
    commander.registerCommand("removeItem", &Visualiser::onRemoveItem);
    commander.registerCommand("addNodeMenu", &Visualiser::onAddNodeMenu);
    commander.registerCommand("setGUIproperties", &Visualiser::onSetGUIproperties);
    commander.registerCommand("clearScene", &Visualiser::onClearScene);
    commander.registerCommand("addModelNameToList", &Visualiser::onAddModelName);
    commander.registerCommand("addElementGUI", &Visualiser::addElementGUI);
    addItemCommander.registerCommand("node", &Visualiser::addNode);
    addItemCommander.registerCommand("arc", &Visualiser::addArc);
    removeItemCommander.registerCommand("node", &Visualiser::removeNode);
    removeItemCommander.registerCommand("arc", &Visualiser::removeArc);
}

#include <QLineEdit>

void Visualiser::onSignalFromElementGUI() {

    Data data;

    foreach (QWidget * widget, elementsGUI.keys()) {
        GUI_ElementProperties ep;
        ep.fromData(elementsGUI[widget]);

        if(ep.widgetType == "LineEdit") {
            QLineEdit * lineEdit = dynamic_cast<QLineEdit*>(widget);
            if(lineEdit != 0)
                ep.value = lineEdit->text();
        }

        data[ep.variableName] = ep.value;
    }

    Request rp;
    rp.senderID = "Visualiser";
    rp.receiverID = mainWidget->getCurrentModel();
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

void Visualiser::onAddElementGUI(const QVariant data)
{
    qDebug() << "GUI element data " << data;
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
        elementsGUI.insert(widget, data);
        mainWidget->onAddElementGUI(widget);
        widget->show();
    }
}

void Visualiser::onClearScene(const QVariant )
{    
    arcList.clear();
    rootNodes.clear();
    allNodes.clear();
    scene->clear();

    QRectF rect(scene->sceneRect());
    scene->update(rect);

    radius = 150;
}

void Visualiser::onRemoveElementsGUI()
{
    foreach (QWidget * widget, elementsGUI.keys()) {
        delete widget;
    }
    elementsGUI.clear();
}

void Visualiser::onAddModelName(const QVariant data)
{
    mainWidget->onAddModelName(data.toString());
}

void Visualiser::setNodesPosition() {

    //qDebug() << "place nodes";

    QList<Node *> & parentNodes = rootNodes.getValues();
    int parentsCount = parentNodes.size();

    emit setNodesCount(parentsCount);

    radius =  qMax(radius, qMax(200, parentsCount * 8));

    qreal angle = -6.282 / parentsCount;

    if(parentsCount == 1) {
        Node * node = parentNodes[0];
        node->move(0, 0);
    }
    else {
        for(int i = 0; i < parentsCount; ++i) {

            Node * node = parentNodes[i];

            qreal nodeAngle = i * angle;
            qreal x = radius * qCos(nodeAngle);
            qreal y = radius * qSin(nodeAngle);

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

    if(!nodeName.isEmpty()) {

        Node * node = allNodes.getValue(nodeName);
        if(!node) {

            node = new Node(nodeName);
            if(node) {

                // установка стиля отображения
                QString nodeStyle(nodeData["style"].toString());
                if(!nodeStyle.isEmpty())
                    node->setPixmap(PixmapHolder::getPixmapFrames(nodeStyle));
                else
                    node->setPixmap(PixmapHolder::getPixmapFrames("defaultNode"));

                node->addToScene(scene);

                connect(node, SIGNAL(menuSignal(QString,QString)), this, SLOT(onMenuItemSelected(QString, QString)));
                connect(node, SIGNAL(showNodeInfo(QString, QString)), this, SLOT(onShowNodeInfo(QString, QString)));
                connect(node, SIGNAL(nodeRemoved(Node*)), this, SLOT(onNodeRemoved(Node*)));

                const QVariant & parentNodeName(nodeData["parent"]);
                if(!parentNodeName.isValid()) {
                      // получение родительского узла
                      Node * parentNode = allNodes.getValue(parentNodeName);

                      if(parentNode)
                           node->setParentNode(parentNode);
                }

                allNodes.add(nodeName, node);
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
                    Node * applicationNode = allNodes.getValue(applicationNodeName);
                    // назначение корневого узла родительским
                    if(applicationNode) {
                        if(!applicationNode->hasChildNode())
                            addArc(QVMGraph::simpleArc(applicationNodeName, nodeName));

                        node->setParentNode(applicationNode);
                    }
                }
                else {
                    // если узел корневой и у него нет наследников
                    if(!node->hasChildNode()) {
                        // добавление в список корневых узлов
                        rootNodes.add(nodeName, node);
                        setNodesPosition();
                    }
                }
            }

            // добавление меню
            if(nodeData["menu"].canConvert<QStringList>()) {
                if(nodeData["menu"] != node->getMenu()) {
                    node->setMenu(nodeData["menu"].toStringList());
                }
            }

            // смена состояния узла
            QString nodeState(nodeData["nodeState"].toString());
            if(nodeState == "on") node->setState(on);
            else
            if(nodeState == "error") node->setState(error);
            else
               node->setState(off);

            // добавление информации об узле
           // qDebug() << "add to node Info: " << nodeData["info"].toString();

            QString nodeInfo(nodeData["info"].toString());
            if(!nodeInfo.isEmpty()) {
                node->setProperty("info", nodeInfo);
                // добавление подсказки
                node->setToolTip(nodeInfo);
                onShowLogMessage(nodeInfo);
            }
        }
    }
}

#include "scene/figures/Arc.h"

void Visualiser::addArc(const Data & arcData)
{
    QString nodeFromName(arcData["from"].toString());
    QString nodeToName(arcData["to"].toString());

    if(!(nodeFromName.isEmpty() || nodeToName.isEmpty())) {
        //qDebug() << "Arc data:" << ArcData.toString();

        QString arcName(QVMGraph::arcName(nodeFromName,nodeToName));

        Arc * arc = arcList.getValue(arcName);

        if(!arc) {

            Node * from = allNodes.getValue(nodeFromName);
            Node * to = allNodes.getValue(nodeToName);

            if(from && to) {
                arc = new Arc(from, to);
                if(arc) {

                    connect(arc, SIGNAL(arcRemoved(Arc*)), this, SLOT(onArcRemoved(Arc*)));
                    connect(this, SIGNAL(animateItems()), arc, SLOT(on_Animate()));
                    connect(this, SIGNAL(queryArc(QString, long)), arc, SLOT(on_QueryArc(QString, long)));

                    arcList.add(arcName, arc);
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
    addItemCommander.run(this, itemData["type"], itemData);
}

void Visualiser::onAddNodeMenu(const QVariant data)
{
    qDebug() <<  data.toString();
    Request rp;
    rp.fromData(data);
    const QString nodeName(rp.senderID.toString());
    Node *node = allNodes.getValue(nodeName);
    if(node) {
        QStringList menuItems(rp.value.toStringList());
        node->setMenu(menuItems);
    }
}

void Visualiser::removeNode(const Data & data) {
    Node *node = allNodes.getValue(data["name"]);
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

        Arc * arc = arcList.getValue(arcName);
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

    if(removeItemCommander.run(this, itemData["type"], itemData))
        setNodesPosition();
}

void Visualiser::onNodeRemoved(Node * node)
{
    //qDebug() << "node removed";

    if(node) {
        const QString nodeName(node->getName());

        rootNodes.remove(node);
        allNodes.remove(node);

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

        arcList.remove(arcName);
        qDebug() << arcName << "removed from ArcList, emit signal...";

        emit arcRemoved(arcName);
    }
}


void Visualiser::onUpdateLog() {

    if(!logMessages.isEmpty()) {

        mainWidget->onSetLogMessages(logMessages);
        logMessages.clear();
    }
}

void Visualiser::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timer.timerId()) {
        emit animateItems();

        static int logTimeCounter = 0;

        if(logTimeCounter < 5) {
            ++logTimeCounter;
        }
        else {
            logTimeCounter = 0;
            emit updateLog();
        }

    } else {
        QObject::timerEvent(event);
    }
}

void Visualiser::onShowLogMessage(const QVariant message)
{
    QString msg(message.toString());
    //qDebug() << message << "++++++++------=========--------++++++++++++++";
    if(!msg.isEmpty())
    {
        QDateTime localTime(QDateTime::currentDateTime());
        logMessages = localTime.toString("hh:mm:ss dd.MM.yy") + "  " + msg + "\n" + logMessages;
    }
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
    visualiseProperties.fromData(data);

    // настройка параметров главного окна программы
    GUI_ApplicationProperties ga;
    ga.fromData(data);

    switch(ga.state) {
        case GUI_ApplicationProperties::maximize:
            qDebug() << "MAXIMIZE!";
            applicationWidget->setWindowState(Qt::WindowMaximized);
        break;
        case GUI_ApplicationProperties::minimize:
            qDebug() << "MINIMIZE!";
            applicationWidget->setWindowState(Qt::WindowMinimized);
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

        mainWidget->onSetSplitterSizes(widgetSizes.toList());
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
    QList<QVariant> nodeNames(rootNodes.getKeys());
    foreach (const QVariant & nodeName, nodeNames) {
        Request rp;
        rp.senderID = "Visualiser";
        rp.receiverID = nodeName;
        rp.command = "Shutdown";
        rp.value = "Shutdown";
        emit request(rp.toData());
    }
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
