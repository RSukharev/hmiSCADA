/*!
\file
\brief Заголовочный файл с описанием класса

Данный файл содержит в себе определение класса RequestParameters
*/

#ifndef REQUEST
#define REQUEST

#include "data.h"

/*!
    \brief RequestParameters - структура параметров запроса для обмена данными между плагинами
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года
*/

struct Request {

    ////
    /// \brief senderID - идентификатор плагина-отправителя
    ///
    QString senderID;

    ////
    /// \brief receiverID - идентификатор плагина-получателя
    ///
    QString receiverID;

    ////
    /// \brief command - команда для выполнения на стороне плагина-получателя
    ///
    QString command;

    ////
    /// \brief value - пересылаемые данные
    ///
    QVariant value;

    ////
    /// \brief fromData - импорт параметров из входного формата Data
    /// \param data - параметры в формате Data
    ///
    void fromData(const Data data) {
            senderID = data.value("senderID").toString();
            receiverID = data.value("receiverID").toString();
            command = data.value("command").toString();
            value = data.value("value");
    }

    ////
    /// \brief toData - экспорт параметров в формат Data
    /// \return Data - параметры в формате Data
    ///
    Data toData() const {
        Data data;
        data["senderID"] = senderID;
        data["receiverID"] = receiverID;
        data["command"] = command;
        data["value"] = value;
        return data;
    }
};

#endif // REQUEST
