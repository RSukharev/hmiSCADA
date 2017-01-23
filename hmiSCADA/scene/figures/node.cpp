#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

#include "node.h"

#ifndef Q_NULLPTR
#define Q_NULLPTR 0
#endif

Node::Node(const QString & nodeName):
    m_showTimeCounter (1),
    m_parentNode(0),
    m_childNode(0),
    m_nodeState(on),
    m_name(nodeName),
    m_changeDirection(false)
{
    // добавление подписи
    QString visibleNodeName(nodeName);
    QStringList visibleNameParts = visibleNodeName.split(".");
    if(visibleNameParts.size() > 1) {
        visibleNameParts.removeAt(0);

        if(visibleNameParts.size() > 1)
            visibleNameParts.removeAt(0);

        visibleNodeName = visibleNameParts.join(".");
    }
    new QGraphicsSimpleTextItem(visibleNodeName, this);
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

Node::~Node()
{
    emit(nodeRemoved(this));
    disconnect();
    disconnect(this);

    this->scene()->removeItem(this);
    qDebug() << m_name << " node deleted";
}

bool Node::isChildOf(Node * node) {
    //qDebug() << "node:" << node << "parentNode:" << parentNode;
    return ((node != Q_NULLPTR) && (m_parentNode != Q_NULLPTR) &&
            ((m_parentNode == node) || m_parentNode->isChildOf(node)));
}

void Node::setChildNodePosition() {

    //qDebug() << "set child position for node " << name;

    if(m_childNode) {

        int dx = 55;
        int dy = 20;
        qreal xParent = pos().x();
        qreal yParent = pos().y();

        if(m_parentNode == 0) {
            xParent -= dx / 2 - m_width/2;
            yParent += m_height - dy / 2;
        }

        if(m_changeDirection) dx = -dx;

        m_childNode->setPos(xParent + dx, yParent + dy);

        m_childNode->setChildNodePosition();
    }
}

void Node::setPixmap(QVector<QPixmap> & _pixmap) {
    m_pixmapFrames = & _pixmap;
    if(!m_pixmapFrames->isEmpty()) {
        m_width = m_pixmapFrames->at(0).width();
        m_height = m_pixmapFrames->at(0).height();
    }
}

void Node::addToScene(QGraphicsScene * scene) {
    if(scene) {
        scene->addItem(this);
        setParent(scene);
        setZValue(999);
    }
}

void Node::move(qreal dx, qreal dy)
{
    //qDebug() << "move" << dx << dy;
    moveBy(dx, dy);
    if(m_childNode) m_childNode->move(dx, dy);
}

void Node::setMenu(const QStringList items)
{
    //qDebug() << items;
    m_menuItems = items;
}

QStringList Node::getMenu()
{
    return m_menuItems;
}

void Node::on_Animate()
{
    if(m_nodeState == on) {

        if(m_showTimeCounter > 0) {
            --m_showTimeCounter;
        }
        else
            setState(off);
    }
}

void Node::setState(NodeState state) {
    if(m_pixmapFrames && (state < m_pixmapFrames->size())) {
        m_showTimeCounter += 1;
        m_nodeState = state;
        refresh();
    }
}

NodeState Node::getState() const {
    return m_nodeState;
}

Node * Node::setChildNode(Node * child)
{
    Node * delegateParent = this;
    if(child && (m_childNode == 0)) {
        m_childNode = child;
        child->m_changeDirection = !m_changeDirection;
        //qDebug() << "adding child " << child->name << " to " << name;
        connect(child, SIGNAL(nodeRemoved(Node*)), this, SLOT(removeChildNode(Node*)));
        setChildNodePosition();
    }
    else {
        delegateParent = m_childNode->setChildNode(child);
    }
        //qDebug() << "Error: cannot add child to node " << name << " child is null";
    return  delegateParent;
}

void Node::setParentNode(Node * parent)
{
    if(parent && (m_parentNode == 0) && parent != this ) {
        //qDebug() << "set parent node " << parent->name << " to " << name;

        m_parentNode = parent->setChildNode(this);;
        setParent(m_parentNode);
    }
    else
        qDebug() << "Error: cannot set parent for node " << m_name << " parent is null";
}

void Node::removeChildNode(Node * child) {
    if(child == m_childNode) {
        qDebug() << m_name << "node removed" << m_childNode->m_name << "from childrenNodes";
        m_childNode = 0;
        setChildNodePosition();
    }
    else {
        qCritical() << "childNode and child is not equal" << m_childNode->m_name << child->m_name;
    }
}

bool Node::hasParentNode() {
    return (m_parentNode != 0);
}

bool Node::hasChildNode()
{
    return (m_childNode != 0);
}

QString Node::getName() const {
    return m_name;
}

QString Node::getParentNodeName() const
{
    QString parentNodeName;
    if(m_parentNode != 0)
        parentNodeName = m_parentNode->getName();

    return parentNodeName;
}

QRectF Node::boundingRect() const {
    return QRectF(-4, -4, m_width+8, m_height+8);
}

void Node::refresh() {
    this->update(boundingRect());
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem* option,
                        QWidget*) {

    Q_UNUSED(option);

    painter->save();
    painter->drawPixmap(0, 0, m_width, m_height, m_pixmapFrames->operator[](m_nodeState));

    if (isSelected()) {
        QPen pen = QPen(Qt::gray, 1, Qt::DashLine);
        pen.setDashOffset(0.5);
        painter->setPen(pen);
        painter->drawRect(boundingRect());
    }

    painter->restore();
}

void Node::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;

    // по умолчанию добавление пункта меню "информация"
    QAction * getInfo = menu.addAction("Info");

    foreach(const QString & item, m_menuItems) {
        menu.addAction(item);
    }

    // проверка выбора пункта меню
    QAction * action = menu.exec(event->screenPos());

    if(action) {
            // вывод информации
            if(action == getInfo)
                emit showNodeInfo(m_name, property("info").toString());
            else // пункт меню пользовательского типа
                emit menuSignal(m_name, action->text());
    }
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_parentNode == 0)
    {
        QPointF currentPos(this->pos());
        QPointF newPos(mapToScene(event->pos()));
        qreal dx = newPos.x() - currentPos.x();
        qreal dy = newPos.y() - currentPos.y();
        move(dx, dy);
    }
    else
        QGraphicsObject::mouseMoveEvent(event);
}

