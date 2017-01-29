#ifndef TSP_H
#define TSP_H

#include "../../../qvmgraph.h"
#include "../../interfaces/imodel.h"
#include "../../interfaces/iplugin.h"
#include "../../../commander.h"

class TSP : public IModel, public IPlugin {
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin")
#endif
    Q_INTERFACES( IPlugin )

public:
    // конструктор
    TSP();

    // деструктор
    ~TSP() {}

    // идентификатор плагина
    QVariant getID() const { return "TSP"; }

    // справочная информация о плагине
    QString aboutInfo() const { return "model for Travelling salesman problem (TSP) search path"; }

    // обработка данных, адресованных этому плагину
    void processData(const QVariant & data);

private slots:
    ///
    /// \brief onRunCommand - выполнение команды
    /// \param data
    ///
    void onRunCommand(const Data data);

signals:
    void dataReceived(const Data data);

public slots:
    // обработка данных, полученных из широковещательной рассылки
    void processMulticastData(const QVariant & data);

private:

    void registerCommands();
    void setData(const QVariant = 0);
    void stop(const QVariant = 0);
    void onMenuItemSelected(const QVariant inputData);
    void onClearScene(const QVariant = 0);

    // запуск модели
    void run(const QVariant = 0);

    void concurrentRun();

    // настройка графического интерфейса
    void setupGUI(const QVariant = 0);

    // построение графа на основе матрицы весов
    QVMGraph buildGraph(const QVector<int> & U_weightMatrix);

    // получение названия вершины графа
    QString getNodeName(int i);

    // генерация случайного пути в графе
    QVector<int> generateRandomSolution(int vertexCount);

    // получение номера следующей вершины
    int getNextVertex(int fromVertex, QVector<QVector<int> > & populations,
                             int childNumber, const QVector<int> &, const QVMGraph & m_graph);

    // получение веса особи
    int getSolutionWeight(const QVector<int> & population, const QVMGraph & m_graph);

    // генерация матрицы весов
    QVector<int> generateSquareMatrix(int size);

    // количество вершин графа
    int m_genesQuantity;

    //  объем популяции
    int m_chromosomesQuantity;

    // максимальное число попыток найти решение
    int m_maxGeneration;

    // флаг останова поиска
    bool m_flagRunning;

    // граф
    QVMGraph m_graph;

    // команды для выполнения
    Commander<TSP, const QVariant> m_commander;

};

#endif // TSP_H
