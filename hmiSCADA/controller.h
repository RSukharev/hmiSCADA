/*!
\file
\brief Заголовочный файл с описанием класса

Данный файл содержит в себе определение класса Controller
*/

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QDebug>
#include <QThread>

#include "data.h"
#include "qvmgraph.h"
#include "visualiser.h"
#include "plugins/interfaces/iplugin.h"
#include "plugins/interfaces/imodel.h"
#include "producer.h"

#include "../Logger/ConsoleAppender.h"

/*!
    \brief Класс контроллер
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года

    Класс обеспечивает пересылку сообщений между плагинами,
    наследуется от QObject
*/

class Controller : public QObject, public Worker
{
    Q_OBJECT

public:

    static void start() {
        Controller * controllerInstance = getInstance();
        if(controllerInstance)
            qDebug() << "Controller started";
        else
            qDebug() << "Controller starting error";
    }

    ////
    /// \brief getInstance - возвращает указатель на экземпляр класса
    /// \return Controller *
    ///
    static Controller * getInstance() {
        static Controller * controllerInstance = 0;
        if(!controllerInstance) {
            controllerInstance = new Controller();
            if(!controllerInstance) {
                qFatal() << "Error: cannot create Controller";
            }
        }

        return controllerInstance;
    }


signals:

    ////
    /// \brief multicastData широковещательная рассылка всем плагинам
    /// \param data - параметры широковещательной рассылки
    ///
    void multicastData(const QVariant data);

    ////
    /// \brief showMessage - сигнал для класса Visualiser: отобразить сообщение
    /// \param msg - сообщение
    ///
    void showMessage(const QVariant msg);

private:

    ////
    /// \brief Controller - конструктор по умолчанию
    /// \param parent - родительский объект
    ///
    explicit Controller(QObject *parent = 0);

    ////
    /// \brief Деструктор класса
    ///
    ~Controller();

    ////
    /// \brief pluginsLoader - загрузка плагинов
    /// \param pathForPlugins - путь к директории с плагинами
    ///
    void pluginsLoader(const QString &pathForPlugins);


    ////
    /// \brief registerPlugin - регистрация плагина-получателя запросов
    /// \param name - название плагина
    /// \param plugin - объект плагина
    ///
    bool registerPlugin(const QVariant name, IPlugin *plugin);

    ////
    /// \brief unregisterPlugin - разрегистрация плагина-получателя запросов
    /// \param name - название плагина
    ///
    void unregisterPlugin(const QVariant name);

    ///
    /// \brief connectPlugin - подключение плагина к сигналам и слотам контроллера
    /// \param plugin
    ///
    void connectPlugin(QObject *plugin);

    ///
    /// \brief requestPlugin - отправка запроса плагину
    /// \param inputdata - данные запроса
    ///
    void processData(const Data & inputdata);

    ////
    /// \brief visualiserInstance - ссылка на объект visualiser
    ///
    Visualiser * m_visualiser;

    ////
    /// \brief plugins - зарегистрированные плагины
    ///
    QMap<QString, IPlugin *> m_plugins;

    ///
    /// \brief m_producer
    ///
    Producer * m_producer;
};

#endif // CONTROLLER_H
