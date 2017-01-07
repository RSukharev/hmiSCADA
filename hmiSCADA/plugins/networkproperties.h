/*!
\file
\brief Заголовочный файл с описанием класса

Данный файл содержит в себе определение класса NetworkProperties
*/

#ifndef NETWORKPROPERTIES
#define NETWORKPROPERTIES

#include "../data.h"

/*!
    \brief NetworkProperties - структура сетевых настроек
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года
*/

struct NetworkProperties {

    ////
    /// \brief ipAddress - IP адрес
    ///
    QString ipAddress;

    ////
    /// \brief port - порт
    ///
    int port;

    ////
    /// \brief protocol - протокол передачи данных
    ///
    QString protocol;

    ////
    /// \brief fromData - импорт параметров из входного формата Data
    /// \param data - параметры в формате Data
    ///
    void fromData(const Data data) {
            ipAddress = data.value("ipAddress").toString();
            port = data.value("port").toInt();
            protocol = data.value("protocol").toString();
    }

    ////
    /// \brief toData - экспорт параметров в формат Data
    /// \return Data - параметры в формате Data
    ///
    Data toData() const {
        Data data;
        data["ipAddress"] = ipAddress;
        data["port"] = port;
        data["protocol"] = protocol;
        return data;
    }
};

#endif // NETWORKPROPERTIES

