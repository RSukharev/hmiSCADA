#include <QtGui>

#include "arc.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>


#include <math.h>
 const qreal Pi = 3.14;

#include "qvmgraph.h"


 Arc::Arc(Node * nodeFrom, Node * nodeTo) :
     m_dashOffset(0),
     m_arrowSize(12)
 {
     connect(nodeFrom, SIGNAL(nodeRemoved(Node*)), this, SLOT(on_NodeRemove(Node*)));
     connect(nodeTo, SIGNAL(nodeRemoved(Node*)), this, SLOT(on_NodeRemove(Node*)));

     m_startItem = nodeFrom;
     m_endItem = nodeTo;

     name = nodeFrom->getName() + "->" + nodeTo->getName();

     m_startDx = m_startItem->boundingRect().width()/2;
     m_startDy = m_startItem->boundingRect().height()/2;
     m_endDx = m_endItem->boundingRect().width()/2;
     m_endDy = m_endItem->boundingRect().height()/2;

     m_normalPen.setColor(Qt::lightGray);
     m_normalPen.setStyle(Qt::SolidLine);
     m_normalPen.setWidth(1);

     m_activePen.setColor(Qt::green);
     m_activePen.setStyle(Qt::DashLine);
     m_activePen.setWidth(3);

     setFlag(QGraphicsItem::ItemIsSelectable, false);
     m_pen = m_activePen;
     QGraphicsScene * scene = m_startItem->scene();
     scene->addItem(this);
 }

 Arc::~Arc() {

     disconnect();
     disconnect(this);

     QGraphicsScene * currentScene = this->scene();
     currentScene->removeItem(this);
     QRectF itemsRect(currentScene->itemsBoundingRect());
     currentScene->update(itemsRect);
 }

 void Arc::on_Animate()
 {
     if(m_isActive) {

         if(m_showTimeCounter > 0) {
             --m_showTimeCounter;

             if(m_dashOffset < 6) m_dashOffset += 0.5;
             else m_dashOffset = 0;
         }
         else
             setActive(false);

         //qDebug() << "animate" << pos;
         update(boundingRect());
     }
 }

 void Arc::on_QueryArc(const QString arcName, long showTime) {

     if(arcName == name) {
         m_showTimeCounter = 1 + showTime;
         setActive(true);
     }
 }

 void Arc::setActive(bool flag)
 {
     //qDebug() << "active:" << flag;
     m_isActive = flag;

     if(m_isActive)
         m_pen = m_activePen;
     else
         m_pen = m_normalPen;
 }

 void Arc::on_NodeRemove(Node * node)
 {
     qDebug() << name;

     emit arcRemoved(this);

     disconnect();
     disconnect(this);

     setParent(node);
 }


  QRectF Arc::boundingRect() const
  {
      qreal extra = (pen().width() + 2) / 2.0;

      return QRectF(path().boundingRect())
              .adjusted(-extra, -extra, extra, extra);
  }

  void Arc::updatePosition()
  {
      update(boundingRect());
  }

  void Arc::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
            QWidget *)
  {

         QPointF startPos(m_startItem->pos());

         startPos.setX(startPos.x() + m_startDx - 4);
         startPos.setY(startPos.y() + m_startDy - 4);

         QPointF endPos(m_endItem->pos());
         endPos.setX(endPos.x() + m_endDx - 4);
         endPos.setY(endPos.y() + m_endDy - 4);

         if(m_isActive) {
             m_pen.setDashOffset(m_dashOffset);
             setZValue(999);
         }
         else
             setZValue(0);

         painter->save();
         painter->setPen(m_pen);

         QPainterPath path;
         double angle = 0;

         if(startPos == endPos) {
               path.moveTo(startPos);
               path.addRoundRect(startPos.x(), startPos.y(), m_startDx * 3, m_startDy *3, 60);
               painter->drawPath(path);
           }
           else {
               path.moveTo(endPos);
               path.lineTo(startPos);

               painter->drawPath(path);

               QLineF line(endPos, startPos);
               angle = ::acos(line.dx() / line.length());
               if (line.dy() >= 0) angle = (Pi * 2) - angle;
           }

           QPainterPath arrowPath;
           arrowPath.moveTo(endPos);
           arrowPath.lineTo(endPos + QPointF(sin(angle + Pi / 3) * m_arrowSize,
                                                 cos(angle + Pi / 3) * m_arrowSize));

           arrowPath.lineTo(endPos + QPointF(sin(angle + Pi - Pi / 3) * m_arrowSize,
                                                 cos(angle + Pi - Pi / 3) * m_arrowSize));

           painter->setBrush(m_pen.color());
           painter->setPen(QPen(m_pen.color(), 1, Qt::SolidLine));
           painter->drawPath(arrowPath);
           painter->restore();

           path.addPath(arrowPath);
           setPath(path);
  }
