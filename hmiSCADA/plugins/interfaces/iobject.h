/*!
\file
\brief Заголовочный файл с описанием класса

Данный файл содержит в себе определение класса IObject
*/

#ifndef IOBJECT_H
#define IOBJECT_H

#include <QObject>
#include <QVariant>

/*!
    \brief IObject - 1-й интерфейсный класс для плагинов, обертка QObject
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года

    Класс обеспечивает:
    1) получение идентификатора плагина
    2) обработку данных, полученных из широковещательной рассылки класса Controller
    3) отправку запроса классу Controller для пересылки данных плагину-получателю
*/

#include <QDebug>

class IObject : public QObject {

    Q_OBJECT

public:

    ////
    /// \brief getID - идентификатор плагина
    /// \return
    ///
    virtual QVariant getID() const = 0;

    ////
    /// \brief Виртуальный деструктор класса
    ///
    virtual ~IObject() { }


public slots:

    ////
    /// \brief processMulticastData - слот обработки данных, полученных из широковещательной рассылки
    /// \param data - данные рассылки
    ///
    virtual void processMulticastData(const QVariant & data) = 0;

signals:
    ////
    /// \brief request - сигнал отправки данных классу Controller
    /// \param data
    ///
    void request(const QVariant & data);

};

#endif // IOBJECT_H
