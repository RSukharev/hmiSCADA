/*!
\file
\brief Заголовочный файл с описанием класса

Данный файл содержит в себе определение класса Node
*/

#ifndef NODE_H
#define NODE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsObject>
#include <QPainter>
#include <QMenu>
#include <QAction>
#include <QCursor>
#include <QGraphicsSimpleTextItem>

#include "../Logger/ConsoleAppender.h"

/*!
    \brief Класс Node - обертка класса QGraphicsObject, визуализация объекта узла графа
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года

    Класс обеспечивает:
    1) отображение узла графа в соответствии с переданым в качестве параметра шаблоном изображения
    2) обработку выбора элементов меню
*/

enum NodeState { off = 0, on = 1, error = 2 };

class Node : public QGraphicsObject {

    Q_OBJECT

public:

    ////
    /// \brief Node - Конструктор класса
    ///
    Node(const QString & nodeName);

    ////
    /// \brief Деструктор класса
    ///
    ~Node();

    ////
    /// \brief setPixmap - назначение изображения для узла
    /// \param _pixmap - изображение
    ///
    void setPixmap(QVector<QPixmap> & _pixmap);

    ////
    /// \brief addToScene - добавление узла на сцену
    /// \param scene - указатель на объект сцены
    ///
    void addToScene(QGraphicsScene * scene);

    ////
    /// \brief move - перемещение изображения узла на параметры смещения по x и y
    /// \param dx - смещение по x
    /// \param dy - смещение по y
    ///
    void move(qreal dx, qreal dy);

    ////
    /// \brief boundingRect - границы изображения
    /// \return
    ///
    QRectF boundingRect() const;

    ////
    /// \brief refresh - обновление участка изображения по границе
    ///
    virtual void refresh();

    ////
    /// \brief addMenu - добавление меню для изображения узла
    /// \param data - параметры меню
    ///
    void setMenu(const QStringList data);

    void setParentNode(Node * parent = 0);

    bool hasParentNode();
    bool hasChildNode();

    QString getName();
    QStringList getMenu();

    void setState(NodeState state);

    NodeState getState() const;


    bool isChildOf(Node *node);


signals:
    ////
    /// \brief menuSignal - сигнал выбора элемента меню
    /// \param nodeName - название узла
    /// \param menuItem - название пункта меню
    ///
    void menuSignal(const QString & nodeName, const QString menuItem);

    ////
    /// \brief nodeRemoved - сигнал удаления изображения узла
    ///
    void nodeRemoved(Node *);

    ///
    /// \brief showNodeInfo - отображение информации об узле
    ///
    void showNodeInfo(const QString nodeName, const QString nodeInfo);

    ///
    /// \brief changeActiveState - сообщение об изменении состояния активности
    ///
    void changeActiveState(bool);

protected slots:

    ////
    /// \brief contextMenuEvent - слот обработки выбора пункта меню
    /// \param event
    ///
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);


    ////
    /// \brief mouseMoveEvent - слот обработки перемещения указателя мыши
    /// \param event
    ///
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    void removeChildNode(Node * = 0);

protected:

    ///
    /// \brief addChild добавление узла наследника
    ///
    void setChildNode(Node * = 0);

    ///
    /// \brief setChildrenPosition - расстановка узлов наследников
    ///
    void setChildNodePosition();

    ////
    /// \brief paint - отрисовка изображения
    /// \param painter
    /// \param item
    /// \param widget
    ///
    void paint(QPainter *painter, const QStyleOptionGraphicsItem* item, QWidget* widget);

    ////
    /// \brief m_pixmapFrames - изображение узла
    ///
    QVector<QPixmap> * m_pixmapFrames;

    ////
    /// \brief m_width - ширина изображения узла
    ///
    int m_width;

    ////
    /// \brief m_height - высота изображения узла
    ///
    int m_height;

    ////
    /// \brief m_menuItems - пункты меню
    ///
    QStringList m_menuItems;

    ///
    /// \brief m_parentNode - узел родитель
    ///
    Node * m_parentNode;

    ///
    /// \brief m_childNode - узел наследник
    ///
    Node * m_childNode;

    ///
    /// \brief m_nodeState флаг состояния узла
    ///
    NodeState m_nodeState;

    QString m_name;

    bool m_changeDirection;

    bool m_isTopNode;

};

#endif // NODE_H
