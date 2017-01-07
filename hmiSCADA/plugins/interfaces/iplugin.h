/*!
\file
\brief Заголовочный файл с описанием класса

Данный файл содержит в себе определение класса IPlugin
*/

#ifndef IPLUGIN_H
#define IPLUGIN_H

#include <QtPlugin>
#include <QDebug>
#include "iobject.h"

/*!
    \brief IPlugin - 2-й интерфейсный класс для плагинов
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года

    Класс обеспечивает:
    1) получение справочной информации о плагине
    2) обработку данных, адресованных этому плагину
*/

class IPlugin {
public:

    ////
    /// \brief aboutInfo - получение справочной информации о плагине
    /// \return
    ///
    virtual QString aboutInfo() const = 0;

    ////
    /// \brief processData - обработка данных, адресованных этому плагину
    /// \param data - данные для обработки
    ///
    virtual void processData(const QVariant & data) = 0;

    ////
    /// \brief Виртуальный деструктор класса
    ///
    virtual ~IPlugin() {}

};

Q_DECLARE_INTERFACE( IPlugin, "hmiSCADA.plugins.IPlugin/1.0" )

#endif // IPLUGIN_H
