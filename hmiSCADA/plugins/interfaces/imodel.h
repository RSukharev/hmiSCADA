/*!
\file
\brief Заголовочный файл с описанием класса

Данный файл содержит в себе определение класса IModel
*/

#ifndef IMODEL_H
#define IMODEL_H

#include "iobject.h"
#include "../request.h"
#include "../../qvmgraph.h"

/*!
    \brief IModel - 1-й интерфейсный класс для плагинов-моделей, обертка IObject
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года

    Класс обеспечивает:
    1) отправку запроса классу Controller для пересылки данных плагину-получателю
    2) отправку запроса на отображение текста
    3) отправку запроса на добавление узла или линка
    4) отправку запроса на удаление узла или линка
    5) отправку запроса на добавление пунктов меню узла
    6) отправку запроса на очистку сцены
*/

class IModel : public IObject {

    Q_OBJECT

protected:

    ////
    /// \brief sendRequestData - отправка запроса классу Controller для пересылки данных плагину-получателю
    /// \param value - параметры запроса
    ///
    virtual void sendRequestData(const QVariant & value) {
        emit request(value);
    }

    ////
    /// \brief showMessage - отправка запроса на отображение текста
    /// \param value - текст
    ///
    void showMessage(const QString & value) {

        Request rp(parametersForVisualise());
        rp.command = "showLogMessage";
        rp.value = value;
        sendRequestData(rp.toData());
    }

    ////
    /// \brief addNodeMenu - отправка запроса на добавление пунктов меню узла
    /// \param nodeName - название узла
    /// \param menu - параметры меню
    ///
    void addNodeMenu(const QString & nodeName, const Data & menu) {
        Request rp(parametersForVisualise());
        rp.senderID = nodeName;
        rp.command = "addNodeMenu";
        rp.value = menu;
        sendRequestData(rp.toData());
    }

    ////
    /// \brief addItem - отправка запроса на добавление узла или линка
    /// \param item - параметры отображаемого объекта
    ///
    void addItem(const Data & item) {

        Data request;

        request["command"] = "addItem";
        request["receiverID"] = "Visualiser";
        request["senderID"] = item["name"];
        request["value"] = item;
        request["pluginID"] = getID();

        sendRequestData(request);
    }

    ////
    /// \brief removeItem - отправка запроса на удаление узла или линка
    /// \param item - параметры объекта
    ///
    void removeItem(const Data & item) {

        Request rp(parametersForVisualise());
        rp.command = "removeItem";
        rp.value = item;
        sendRequestData(rp.toData());
    }

    ////
    /// \brief clearScene - отправка запроса на очистку сцены
    ///
    void clearScene() {

        Request rp(parametersForVisualise());
        rp.command = "clearScene";
        sendRequestData(rp.toData());
    }

    ////
    /// \brief parametersForVisualise - базовые параметры запроса на визуализацию
    /// \return
    ///
    virtual Request & parametersForVisualise() {

        static Request defaultParameters;
        if(defaultParameters.senderID.isNull()) {
            defaultParameters.senderID = getID();
            defaultParameters.receiverID = "Visualiser";
        }
        return defaultParameters;
    }
};

#endif // IMODEL_H

