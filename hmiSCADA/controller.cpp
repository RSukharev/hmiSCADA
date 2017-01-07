#include <QPluginLoader>

#include "plugins/interfaces/iobject.h"
#include "plugins/interfaces/iplugin.h"
#include "plugins/interfaces/inode.h"

#include "controller.h"
#include "resourcepath.h"

// обработчик запросов - вызывает метод processData
// зарегистрированного плагина получателя
void Controller::processData(const Data & inputdata) {

    //qDebug() << qPrintable(inputdata.toString());

    // проверка отправителя
    const QVariant & senderID = inputdata["senderID"];
    if(senderID.isValid() &&
          m_plugins[senderID.toString()] == 0) {
        // если отправитель не зарегистрирован в списке плагинов
        // регистрация плагина - адаптера под именем отправителя
        QString pluginAdapterName(inputdata["pluginID"].toString());
        IPlugin * pluginAdapter = m_plugins[pluginAdapterName];
        if(!registerPlugin(senderID, pluginAdapter))
            qDebug() << "registerPlugin error:" <<
                        qPrintable(inputdata.toString());
    }

    // проверка получателя
    const QVariant & receiverID = inputdata["receiverID"];
    IPlugin * plugin = m_plugins[receiverID.toString()];
    if(plugin != 0) {
        // обработка данных зарегистрированным плагином
        plugin->processData(inputdata);
    }
    else {
        if( receiverID == "Multicast") {
            // отправка широковещательного сообщения
            // всем зарегистрированным объектам
            emit multicastData(inputdata);
        }
        else {
            // плагин получатель не зарегистрирован
            qDebug() << "Error: receiver does not exists " <<
                        qPrintable(inputdata.toString());
        }
    }
}

#include <QCoreApplication>
#include <QDir>

Controller::Controller(QObject *parent) :
    QObject(parent),
    m_visualiser(0)
{

    // set global path for resources:
    ResourcePath::set(QCoreApplication::applicationDirPath());

    // register data for connections
    qRegisterMetaType< QList<int> >("QList<int>");
    qRegisterMetaType< Data >("Data");

    // setup threads for plugin's data queue
    m_producer = new Producer(this);
    assert(m_producer);

    // setup Visualiser
    m_visualiser = new Visualiser;
    assert(m_visualiser);

    // move Controller to new thread
    QThread * thread = new QThread();
    if(thread != 0) {
        moveToThread(thread);
        thread->start();
    }

    // подключение слотов Visualiser-a
    QObject::connect(this, SIGNAL(showMessage(QVariant)),
                              m_visualiser, SLOT(onShowLogMessage(QVariant)), Qt::QueuedConnection);

    // регистрация Visualiser в качестве плагина
    registerPlugin("Visualiser", m_visualiser);

    // подключение сигналов запроса и широковещательной рассылки
    connectPlugin(m_visualiser);

    // подключение плагинов
    pluginsLoader(ResourcePath::get()+"/plugins/");
}

Controller::~Controller()
{
    qDebug() << "~Controller()";

    if(m_producer)
        m_producer->deleteLater();

    if(m_visualiser)
        m_visualiser->deleteLater();

    // удаление плагинов:
    QList<IPlugin *> pluginList = m_plugins.values();
    while(!pluginList.isEmpty()) {
        IPlugin * plugin = pluginList.back();
        if(plugin != 0) delete plugin;
        pluginList.removeAll(plugin);
    }
}

// регистрация плагина
bool Controller::registerPlugin(const QVariant name, IPlugin * plugin) {
    bool result = false;
    if((plugin != 0) && name.isValid()) {
        m_plugins.insert(name.toString(), plugin);
        result = true;
    }
    return result;
}

// подключение сигналов и слотов плагина
void Controller::connectPlugin(QObject * plugin) {
    connect(plugin, SIGNAL(request(QVariant)), m_producer, SLOT(enqueueData(QVariant)), Qt::QueuedConnection);
    connect(this, SIGNAL(multicastData(QVariant)), plugin, SLOT(processMulticastData(QVariant)), Qt::QueuedConnection);
}

// разрегистрация плагина
void Controller::unregisterPlugin(const QVariant pluginName) {
    QString name(pluginName.toString());
    if(!name.isEmpty()) {
        m_plugins.remove(name);
    }
    else qDebug() << "Error unregister plugin, name is empty";
}

void Controller::pluginsLoader(const QString & pathForPlugins)
{
    qDebug() << "searching for plugins...";

    QDir pluginsDir(pathForPlugins);

    // поиск плагинов
    foreach( const QString & pluginName, pluginsDir.entryList( QDir::Files ) ) {
        showMessage("===========================================================");

        // загрузка плагина
        QPluginLoader loader( pluginsDir.absoluteFilePath( pluginName ) );

        // если плагин загружен
        if(loader.load()) {

            // указатели на плагин, приведенные к форматам IPlugin и IObject
            IPlugin * plugin = qobject_cast< IPlugin* > ( loader.instance() );
            IObject * object = static_cast<IObject *>( loader.instance() );

            if(plugin && object) {

                // вывод информации о плагине
                showMessage(plugin->aboutInfo());

                // регистрация плагина, как получателя сообщений
                if(registerPlugin(object->getID(), plugin)) {
                    qDebug() << object->getID() << "registered ok";
                }

                // подключение сигналов запроса и широковещательной рассылки
                connectPlugin(object);

                // проверка плагина на принадлежность к классу модели
                IModel * modelObject = dynamic_cast<IModel *>( object );

                // если плагин модель
                if(modelObject) {

                    showMessage("this is a model: " + object->getID().toString());

                    // название плагина добавляется в список моделей
                    Request request;
                    request.receiverID = "Visualiser";
                    request.command = "addModelNameToList";
                    request.value = object->getID();

                    // отправка запроса на добавление
                    m_producer->enqueueData(request.toData());
                }

                showMessage("Found plugin: " + pluginName);
            }
            else {
                showMessage("Failed to cast plugin: " + loader.errorString());
                loader.unload();
            }
        }
        else
            showMessage("Failed to load plugin: " + loader.errorString());
    }
    showMessage("===========================================================");
    showMessage("end searching for plugins");
}

