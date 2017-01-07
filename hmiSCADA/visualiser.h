/*!
\file
\brief Заголовочный файл с описанием класса

Данный файл содержит в себе определение класса Visualiser
*/

#ifndef VISUALISER_H
#define VISUALISER_H

#include <QWidget>
#include <QGraphicsScene>
#include <QBasicTimer>
#include <QHash>

#include "data.h"
#include "scene/guiproperties.h"
#include "scene/mainwidget.h"

#include "Logger/ConsoleAppender.h"

#include "objectmap.h"

////
/// \brief Предварительное объявление класса Node
///
class Node;

////
/// \brief Предварительное объявление класса Arc
///
class Arc;

/*!
    \brief Класс визуализации
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года
    \warning Данный класс в последствии может быть реализован как плагин

    Класс обеспечивает:
    1) визуализацию элементов графа: Узлов (Node) и Дуг (Arc)
    2) взаимодействие с графическими отображениями этих элементов
*/

#include "plugins/interfaces/iplugin.h"
#include "commander.h"

class Visualiser :  public IObject, public IPlugin
{
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin")
#endif
    Q_INTERFACES( IPlugin )

public slots:

    ////
    /// \brief processData - отправка сигнала о наличии новых данных для обработки
    /// \param data - данные для обработки
    ///
    void processData(const QVariant &inputData) {
        emit runCommand(inputData);
    }

    void onRunCommand(const Data data) {

       //qDebug() << "visualiser thread:" << this->thread();
       //qDebug() << qPrintable(data.toString());
       commander.run(this, data["command"], data["value"]);
    }

signals:
    void runCommand(const Data data);

public:

    QVariant getID() const { return "Visualiser"; }

    void processMulticastData(const QVariant & ) {}

    ////
    /// \brief Конструктор класса
    /// \param parent
    ///
    explicit Visualiser();

    ////
    /// \brief Деструктор класса
    ///
    ~Visualiser();

    ////
    /// \brief aboutInfo - получение справочной информации о плагине
    /// \return
    ///
    virtual QString aboutInfo() const;


signals:

    ////
    /// \brief nodeInserted - сигнал о том, что добавлен узел
    /// \param name - название узла
    /// \param nodeTemplate - шаблон узла
    ///
    void nodeInserted(const QString & name, const QString & nodeTemplate);

    ////
    /// \brief ArcInserted - сигнал о том, что добавлен линк
    /// \param fromNode - название узла - отправителя
    /// \param toNode - название узла - получателя
    ///
    void arcInserted(const QString & fromNode, const QString & toNode);

    ////
    /// \brief nodeRemoved - сигнал о том, что удален узел
    /// \param name - название узла
    ///
    void nodeRemoved(const QString & name);

    ////
    /// \brief ArcRemoved - сигнал о том, что удален поток
    /// \param name - название линка
    ///
    void arcRemoved(const QString & name);


    ////
    /// \brief animateItems - сигнал включения анимации для графических элементов сцены
    ///
    void animateItems();

    ////
    /// \brief request - сигнал запрос к плагину программы
    /// \param data - данные запроса
    ///
    void request(const QVariant data);


    ///
    /// \brief queryArc
    /// \param arcName
    /// \param showTime
    ///
    void queryArc(const QString arcName, long showTime);

    ///
    /// \brief updateLog - сигнал для обновления логов
    ///
    void updateLog();

    ///
    /// \brief setNodesCount
    ///
    void setNodesCount(int);


    ////////////////////////////////////////////////////////////////////

    ////
    /// \brief addItem - сигнал для класса Visualiser: добавление объекта на сцену
    /// \param data - параметры объекта
    ///
    void addItem(const Data data);

    ////
    /// \brief removeItem - сигнал для класса Visualiser: удаление объекта со сцены
    /// \param data - параметры объекта
    ///
    void removeItem(const Data data);

    ////
    /// \brief clearScene - сигнал для класса Visualiser: очистить сцену
    ///
    void clearScene();

    ////
    /// \brief setGUIproperties - сигнал для класса Visualiser: задать параметры графического интерфейса
    /// \param data - параметры GUI
    ///
    void setGUIproperties(const Data data);

    ////
    /// \brief addModelNameToList - сигнал для класса Visualiser: добавить плагин-модель в список моделей
    /// \param name - название модели
    ///
    void addModelNameToList(const QVariant data);

    ////
    /// \brief addElementGUI - сигнал для класса Visualiser: добавить элемент управления
    /// \param data - параметры элемента GUI
    ///
    void addElementGUI(const QVariant data);

    ////
    /// \brief addNodeMenu - сигнал для класса Visualiser: добавить элементы меню для узла
    /// \param data - параметры меню узла
    ///
    void addNodeMenu(const Data data);

public slots:
    ////
    /// \brief onAddElementGUI - слот добавления элемента графического управления
    /// \param data - данные элемента управления
    ///
    void onAddElementGUI(const QVariant data);

    ////
    /// \brief onAddModelName - слот добавления в отображение названия модели подключаемого плагина
    /// \param name - название модели
    ///
    void onAddModelName(const QVariant name);

    ////
    /// \brief onAddItem - слот добавления графического элемента (узла или линка)
    /// \param data - данные
    ///
    void onAddItem(const QVariant data);

    ////
    /// \brief onAddNodeMenu - слот добавления пункта меню для узла
    /// \param data - данные
    ///
    void onAddNodeMenu(const QVariant data);

    ////
    /// \brief onRemoveItem - слот для удаления графического элемента (узла или линка)
    /// \param data - данные
    ///
    void onRemoveItem(const QVariant data);

    ////
    /// \brief onSetGUIproperties - слот настройки отображения элементов графического управления
    /// \param data - данные
    ///
    void onSetGUIproperties(const QVariant data);

    ////
    /// \brief onShowLogMessage - слот отображения сообщения
    /// \param message - сообщение
    ///
    void onShowLogMessage(const QVariant message);

    ////
    /// \brief onClearScene - слот очистки графической сцены
    ///
    void onClearScene(const QVariant = 0);

    ////
    /// \brief onBroadcast - слот обработки широковещательной рассылки сообщений плагинам
    /// \param data - данные
    ///
    void onBroadcast(const QVariant data);

    ////
    /// \brief onParsedProperty - слот обработки распарсенного параметра сообщения
    /// \param name - название параметра
    /// \param value - значение параметра
    ///
    void onParsedProperty(const QString & name, const QVariant & value);

private slots:
    ////
    /// \brief onNodeRemoved - слот для удаления узла со сцены
    /// \param node - удаляемый узел
    ///
    void onNodeRemoved(Node * node);

    void onArcRemoved(Arc * Arc);

    ////
    /// \brief timerEvent - слот для анимации графических элементов сцены
    /// \param event - событие тамера
    ///
    void timerEvent(QTimerEvent *event);

    ////
    /// \brief onSignalFromElementGUI - слот обработки сигнала интерфейса управления
    ///
    void onSignalFromElementGUI();

    ////
    /// \brief onRemoveElementsGUI - слот для удаления элемента интерфейса управления
    ///
    void onRemoveElementsGUI();

    ////
    /// \brief onMenuItemSelected - слот для обработки выбора пункта меню узла
    /// \param nodeName - имя узла
    /// \param menuItem - пункт меню
    ///
    void onMenuItemSelected(const QString & nodeName, const QString & menuItem);

    ////
    /// \brief on_comboBox_currentIndexChanged - слот обработки выбора плагина
    ///
    void on_comboBox_currentIndexChanged(const QString &);

    ////
    /// \brief on_pbClear_released  - слот обработки очистки сцены
    ///
    void on_pbClear_released();

    ///
    /// \brief onShowNodeInfo - слот отображения информации об узле
    ///
    void onShowNodeInfo(const QString &, const QString &);

    ///
    /// \brief onUpdateLog - слот обновления логов
    ///
    void onUpdateLog();

    void onRunModel(const QString &modelName);

    void onChangeModel(const QString &modelName);

    void onCloseAllNodes();

private:
    ////
    /// \brief addNode - добавление узла
    /// \param data - данные
    ///
    void addNode(const Data & data);

    ////
    /// \brief addArc - добавление потока
    /// \param data - данные
    ///
    void addArc(const Data &ArcData);

    ///
    /// \brief removeNode - удаление узла
    /// \param data - данные
    ///
    void removeNode(const Data & data);

    ///
    /// \brief removeArc - удаление потока
    /// \param data - данные
    ///
    void removeArc(const Data & data);

    ////
    /// \brief setNodesPosition - расстановка узлов
    ///
    void setNodesPosition();

    ////
    /// \brief scene - графическая сцена
    ///
    QGraphicsScene *scene;

    ////
    /// \brief timer - таймер для анимации объектов сцены
    ///
    QBasicTimer timer;

    ////
    /// \brief nodesList - список узлов
    ///
    ObjectMap<Node*> allNodes;

    ObjectMap<Node*> rootNodes;

    ////
    /// \brief ArcList - список потоков
    ///
    ObjectMap<Arc*> arcList;

    ////
    /// \brief applicationWidget - виджет главного окна программы
    ///
    QWidget * applicationWidget;

    ////
    /// \brief radius - радиус для выстраивания узлов
    ///
    int radius;

    ////
    /// \brief visualiseProperties - набор параметров визуализации
    ///
    GUI_VisualiseProperties visualiseProperties;

    ////
    /// \brief elementsGUI - набор элементов графического интерфейса управления
    ///
    QMap<QWidget *, Data> elementsGUI;

    QString logMessages;

    MainWidget * mainWidget;
    void registerCommands();

    Commander<Visualiser, const QVariant> commander;
    Commander<Visualiser, const Data &> addItemCommander;
    Commander<Visualiser, const Data &> removeItemCommander;
};

#endif // VISUALISER_H
