#ifndef ARC_H
#define ARC_H

#include "node.h"

#include "../Logger/ConsoleAppender.h"

#include <QGraphicsPathItem>

 class QGraphicsPolygonItem;
 class QGraphicsLineItem;
 class QGraphicsScene;
 class QRectF;
 class QGraphicsSceneMouseEvent;
 class QPainterPath;

#include <QPen>

 class Arc : public QObject, public QGraphicsPathItem
 {
 public:

     Q_OBJECT

 public:

     ////
     /// \brief Arc - Конструктор класса
     /// \param from - название узла источника
     /// \param to - название узла получателя
     ///
     Arc(Node * from = 0, Node * to = 0);

     QRectF boundingRect() const;

     void setColor(const QColor color)
         { m_color = color; updatePosition();  }

     void setPen(const QPen pen)
         { m_pen = pen; updatePosition();  }

     void updatePosition();

 protected:
     void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget = 0);

     ////
     /// \brief Деструктор класса
     ///
     ~Arc();


 signals:
     ////
     /// \brief FlowRemoved - сигнал об удалении дуги
     ///
     void arcRemoved(Arc *);

 protected:
     long long showTimeCounter;

     QString name;

 public slots:
     void on_Animate();

     ////
     /// \brief query - запрос на отображение линка
     /// \param showTime - время активности
     ///
     void on_QueryArc(const QString name, long showTime);

 protected slots:

     ///
     /// \brief setActive
     /// \param flag
     ///
     void setActive(bool flag);

     ////
     /// \brief on_NodeRemove - слот удаления
     ///
     void on_NodeRemove(Node *);

 private:
     QGraphicsItem *m_startItem;
     QGraphicsItem *m_endItem;
     QColor m_color;
     QPen m_pen;
     qreal m_dashOffset;
     bool m_isActive;
     int m_arrowSize;

     QPen m_activePen;
     QPen m_normalPen;

     qreal m_startDx;
     qreal m_startDy;
     qreal m_endDx;
     qreal m_endDy;
 };

#endif // ARC_H
