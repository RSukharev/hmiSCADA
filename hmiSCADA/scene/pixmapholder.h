/*!
\file
\brief Заголовочный файл с описанием класса

Данный файл содержит в себе определение класса PixmapFabric
*/

#ifndef PIXMAPHOLDER_H
#define PIXMAPHOLDER_H

#include <QDebug>
#include <QHash>
#include <QPixmap>
#include <QPainter>
#include <QDir>

#include "../Logger/ConsoleAppender.h"
#include "resourcepath.h"

/*!
    \brief Класс - хранилище графических изображений
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года
    \warning

    Класс обеспечивает:
    1) добавление изображений в хранилище
    2) извлечение изображений из хранилища по имени
*/

class PixmapHolder {
public:

    ///
    /// \brief PixmapFrames набор изображений
    ///
    typedef QVector<QPixmap> PixmapFrames;

    ////
    /// \brief addTemplate - добавление шаблона изображения
    /// \param templateName - название шаблона
    /// \param pixmapName - название файла с изображением
    /// \param instance - указатель на объект шаблона
    ///
    static void addTemplate(const QString templateName,
                                        const QString pixmapName,
                                        const int framesCount,
                                        PixmapHolder * instance = 0) {

        if(!instance) instance = getInstance();

        if(instance) {
            if(!pixmapName.isEmpty()) {
                QPixmap pixmapFromFile(pixmapName);
                PixmapFrames pixtemp = splitToFrames(pixmapFromFile, framesCount);
                instance->templates.insert(templateName, pixtemp);
                }
            else
                qDebug() << "pixmap file name is empty";
        }
    }

    static PixmapFrames splitToFrames(QPixmap pixmap, int framesCount) {
        PixmapFrames pixmapList;

        if(!pixmap.isNull()) {

            if(pixmap.height() > 64)
                pixmap = pixmap.scaledToHeight(64);

            if(!framesCount) framesCount = 1;

            int width = pixmap.width() / framesCount;
            int height = pixmap.height();

            for(int i = 0; i < framesCount; ++i)
                pixmapList << pixmap.copy(width * i, 0,  width, height);
        }

        return pixmapList;
    }

    ////
    /// \brief getPixmapFrames - получение изображения из фабрики шаблонов
    /// \param name - название шаблона
    /// \return QPixmap - изображение, соответствующее названию шаблона
    ///
    static PixmapFrames & getPixmapFrames(const QString & name) {
        static PixmapFrames stub;
        PixmapHolder * instance = getInstance();
        if(instance)
            return instance->templates[name];

        return stub;
    }

    ////
    /// \brief getTemplateNames - отображение списка названий шаблонов
    /// \return QStringList - список названий шаблонов
    ///
    static QStringList getTemplateNames() {
        QStringList nodeNames;
        PixmapHolder * instance = getInstance();
        if(instance) {
            nodeNames = instance->templates.keys();
        }
        return nodeNames;
    }

private:

    ////
    /// \brief PixmapHolder - Конструктор класса
    ///
    PixmapHolder() {
        QString pixmapPath(ResourcePath::get()+"/pixmaps/");
        addTemplate("DBnode", pixmapPath + "database.png", 1, this);
        addTemplate("PCcase", pixmapPath + "pc_case.png", 1, this);
        addTemplate("PC", pixmapPath + "pc.png", 1, this);
        addTemplate("defaultNode", 3, pixmapDefaultNode());
        addTemplate("module", 3, pixmapModule());
    }

    void addTemplate(QString templateName, int framesCount, QPixmap pixmap) {
        PixmapFrames pixtemp = splitToFrames(pixmap, framesCount);
        templates.insert(templateName, pixtemp);
    }

    QPixmap pixmapModule() {

        // размер по высоте и ширине одного фрейма
        int size = 15;

        // коэффициент размера
        int k = 1;

        // количество фреймов
        int n = 3;

        // размер холста
        QSize pictureSize(size * k * n, size * k);

        // холст
        QPixmap pixmap(pictureSize);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);

        QColor color = QColor("#888888");
        painter.setPen(color);
        painter.setBrush(QBrush(color));

        painter.drawEllipse(
                    0,0,
                    pictureSize.width() / 3,
                    pictureSize.height());

        color = QColor("#00ff00");
        painter.setPen(color);
        painter.setBrush(QBrush(color));

        painter.drawEllipse(
                    pictureSize.width() / 3,
                    0,
                    pictureSize.width() * 2 / 3,
                    pictureSize.height());

        color = QColor("#ff0000");
        painter.setPen(color);
        painter.setBrush(QBrush(color));

        painter.drawEllipse(
                    pictureSize.width() * 2 / 3,
                    0,
                    pictureSize.width(),
                    pictureSize.height());

         return pixmap;
    }

    QPixmap pixmapDefaultNode() {

        // размер холста
        QSize pictureSize(30,10);
        // холст
        QPixmap pixmap(pictureSize);

        // рисуем
        QPainter painter(&pixmap);

        int width = pictureSize.width() / 3;
        int height = pictureSize.height();

        QColor color = QColor("#888888");

        painter.fillRect(  0, 0,
                           width, height,
                           QBrush(color));

        color = QColor("#00ff00");

        painter.fillRect(  width, 0,
                           width * 2, height,
                           QBrush(color));

        color = QColor("#ff0000");

        painter.fillRect(  width * 2, 0,
                           width * 3, height,
                           QBrush(color));

         return pixmap;
    }

    ///
    /// \brief getInstance
    /// \return
    ///
    static PixmapHolder * getInstance() {
        static PixmapHolder * instance = 0;
        if(!instance) {
            instance = new PixmapHolder;
        }
        return instance;
    }

    ////
    /// \brief templates - шаблоны
    ///
    QHash<QString, PixmapFrames> templates;

};

#endif // PIXMAPHOLDER_H

