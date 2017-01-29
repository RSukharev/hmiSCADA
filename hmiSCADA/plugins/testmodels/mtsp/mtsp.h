#ifndef MTSP_H
#define MTSP_H

#include "../../../nodesgraph/qvmgraph.h"
#include "../../interfaces/imodel.h"
#include "../../interfaces/iplugin.h"

// класс моделирования задачи коммивояжера
// с двумя различными весовыми характеристиками дуги графа
// для поиска используется генетический алгоритм с жадным кроссовером
class MTSP : public IModel, public IPlugin {

    Q_OBJECT

    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin")
    Q_INTERFACES( IPlugin )

public:
    // конструктор
    MTSP();

    // деструктор
    ~MTSP() {}

    // идентификатор плагина
    QString getID() const { return "MTSP"; }

    // справочная информация о плагине
    QString aboutInfo() const { return "model for Travelling salesman problem (TSP) search path with multy params (2 by default)"; }

    // обработка данных, адресованных этому плагину
    void processData(const QVariant & data);

    // проверка наличия объекта визуализации
    virtual bool hasItem(const QString & name) const;

    // класс весов дуги графа - обертка списка типа integer
    class Weights : public QList<int> {
    public:
        Weights() {}
        Weights(int a, int b) : QList<int>(Weights() << a << b) {}
    };

public slots:
    // обработка данных, полученных из широковещательной рассылки
    void processMulticastData(const QVariant & data);

private:

    // настройка графического интерфейса
    void setupGUI();

    // запуск поиска
    void run();

    // запуск поиска в конкурентном режиме
    static void concurrentRun(MTSP * mtsp) {
        mtsp->run();
    }

    // построение графа на основе матрицы весов
    QVMGraph buildGraph(const QVector<Weights> & U_weightMatrix);

    // получение названия вершины графа
    QString getVertexName(int i);

    // генерация случайного пути в графе
    QStringList generateRandomPath(int vertexCount);

    // вывод на экран списка популяций
    void printPopulation(const QVector<QStringList> & populations);

    // отбор следующей вершины жадным кроссовером
    QString getNextVertex(const QString & fromVertex, QVector<QStringList > & populations,
                             int childNumber, const QStringList &, const QVMGraph & graph);

    // генерация матрицы
    QVector<Weights> generateSquareMatrix(int size);

    // вычисление веса дуги
    float getArcWeight(const Data &arc, int weightsCount = 2);

    // вычисление веса популяции
    float getSolutionWeight(const QStringList & solution, const QVMGraph & graph);

    // проверка популяции на одинаковые пути
    bool checkForEqualsInPopulation(QVector<QStringList> &population);

    // количество вершин в графе
    int genesQuantity;

    // максимальное количество попыток генерации популяций
    int maxGeneration;

    // граф
    QVMGraph graph;

};

#endif // MTSP_H
