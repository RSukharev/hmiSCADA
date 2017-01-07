#ifndef COMPMODEL_H
#define COMPMODEL_H

#include "../../interfaces/imodel.h"
#include "../../interfaces/iplugin.h"
#include "../../../commander.h"

// модель локальной сети

class CompModel : public IModel, public IPlugin {
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin")
#endif
    Q_INTERFACES( IPlugin )

public:

    CompModel();

    // деструктор
    ~CompModel() {}

    // обработка данных, адресованных этому плагину
    void processData(const QVariant & data);

    // идентификатор плагина
    QVariant getID() const { return "CompModel"; }

    // справочная информация о плагине
    QString aboutInfo() const { return "Simulation of local area network"; }

public slots:
    // обработка широковещательных запросов
    void processMulticastData(const QVariant & data);

private slots:
    ///
    /// \brief onRunCommand - выполнение команды
    /// \param data
    ///
    void onRunCommand(const Data data);

signals:
    void dataReceived(const Data data);

private:

    void registerCommands();
    void onMenuItemSelected(const QVariant inputData);
    void onClearScene(const QVariant);

    // запуск модели
    void run(const QVariant);

    // граф, содержащий моделируемые устройства и связи между ними
    QVMGraph graph;

    Data createChildNode(const QString &parentName, const QString &childName, const QString &style = "");

    void addChildren(const QString &nodeName, int childrenCount);

    Commander<CompModel, const QVariant> m_commander;

};

#endif // COMPMODEL_H
