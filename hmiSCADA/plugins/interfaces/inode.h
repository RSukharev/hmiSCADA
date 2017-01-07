/*!
\file
\brief Заголовочный файл с описанием класса

Данный файл содержит в себе определение класса INode
*/

#ifndef INODE_H
#define INODE_H

#include "../networkproperties.h"
#include "../request.h"
#include "iplugin.h"
#include "iobject.h"
#include "../../qvmgraph.h"

/*!
    \brief INode - интерфейсный класс для сетевых агентов, обертка IObject
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года

    Класс обеспечивает:
    1) получение сетевых настроек агента
    2) отправку запроса (через плагин сетевой сервер) данных предназначенных плагину-получателю
    3) отправка запроса на отображение текста
    4) отправку запроса на добавление своего узла
    5) отправку запроса на добавление своего компонента-наследника
    6) отправку запроса на добавление меню своего компонента-наследника
    7) отправку запроса на добавление линка
    8) отправку запроса на удаление своего узла или линка
    9) отправку запроса на добавление пунктов меню узла
*/

class INode : public IObject {

    Q_OBJECT

public:

    ////
    /// \brief getNetworkProperties - получение сетевых настроек агента
    /// \return
    ///
    virtual NetworkProperties getNetworkProperties() const = 0;

    // отправка запроса

    ////
    /// \brief sendRequestData - отправка запроса (через плагин сетевой сервер) данных предназначенных плагину-получателю
    /// \param value - параметры запроса
    ///
    virtual void sendRequestData(const QVariant & value) {
        emit request(value);
    }

    ////
    /// \brief sendTextMessage - запрос на отображение текста
    /// \param value - текст
    ///
    void sendTextMessage(const QVariant & value) {

        Request request(parametersForVisualise());
        request.command = "showLogMessage";
        request.value = value;
        sendRequestData(request.toData());
    }

    ////
    /// \brief addNodeMenu - запрос на добавление пунктов меню узла
    /// \param menu - параметры меню узла
    ///
    void addNodeMenu(const QVariant & menu, const QString nodeName="") {
        Request request(parametersForVisualise());
        if(nodeName.isEmpty())
            request.senderID = getID().toString();
        else
            request.senderID = nodeName;
        request.command = "addNodeMenu";
        request.value = menu;
        sendRequestData(request.toData());
    }

    ///
    /// \brief createChild
    /// \param parentName
    /// \param childName
    /// \param style
    /// \return
    ///
    Data createChild(
            const QString & parentName,
            const QString & childName,
            const QString & style = "") {
        Data childNode = QVMGraph::simpleNode(childName, style);

        if(parentName.isEmpty())
            childNode["parent"] = getID();
        else
            childNode["parent"] = parentName;

        return childNode;
    }

    ////
    /// \brief addChildMenu - запрос на добавление пунктов меню узла
    /// \param menu - параметры меню узла
    ///
    void addChildMenu(const QString & childName, const QVariant & menu) {
        Request request(parametersForVisualise());
        request.senderID = childName;
        request.command = "addNodeMenu";
        request.value = menu;
        sendRequestData(request.toData());
    }

    ////
    /// \brief removeMe - запрос на удаление своего узла
    ///
    void removeMe() {
        Request request(parametersForVisualise());
        request.command = "removeItem";
        request.value = QVMGraph::simpleNode(getID().toString());
        sendRequestData(request.toData());
    }

    void addItem(const Data & item) {
        Request request(parametersForVisualise());
        request.command = "addItem";
        request.value = item;
        sendRequestData(request.toData());
    }

    ////
    /// \brief addFlow - запрос на добавление линка
    /// \param receiverNodeName - название узла получателя
    /// \param senderNodeName - название узла отправителя
    ///
    void addFlow(const QString & senderNodeName, const QString & receiverNodeName, const QString & arcType = "flow") {

        Request request(parametersForVisualise());
        request.command = "addItem";

        Data arc;
        if(senderNodeName.isEmpty())
            arc = QVMGraph::simpleArc(getID().toString(), receiverNodeName);
        else
            arc = QVMGraph::simpleArc(senderNodeName, receiverNodeName);

        arc["arctype"] = arcType;
        request.value = arc;

        sendRequestData(request.toData());
    }

    ////
    /// \brief removeFlow - запрос на удаление  линка
    /// \param secondNodeName - название второго узла дуги
    ///
    void removeFlow(const QString & secondNodeName) {
        Request request(parametersForVisualise());
        request.command = "removeItem";
        request.value = QVMGraph::simpleArc(getID().toString(), secondNodeName);
        sendRequestData(request.toData());
    }

protected:
    ////
    /// \brief addNode - запрос на добавление своего узла в граф
    ///
    virtual void addNode() = 0;

    ////
    /// \brief parametersForVisualise - базовые параметры запроса на визуализацию
    /// \return
    ///
    virtual Request & parametersForVisualise() {

        static Request defaultParameters;
        if(defaultParameters.senderID.isNull()) {
            defaultParameters.senderID = getID().toString();
            defaultParameters.receiverID = "Visualiser";
        }
        return defaultParameters;
    }

};

#endif // INODE_H

