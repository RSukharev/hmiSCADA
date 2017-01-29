#include <ctime>
#include "mtsp.h"
#include "../../../scene/guiproperties.h"
#include <QDebug>

#include "Logger/ConsoleAppender.h"

#if QT_VERSION < 0x050000
#include <QtConcurrentRun>
#else
#include <QtConcurrent/QtConcurrentRun>
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    Q_EXPORT_PLUGIN2( IPlugin, MTSP )
#endif

MTSP::MTSP() :
    genesQuantity(16),
    maxGeneration(10)
{
    Logger::filterLevel(Logger::LogLevel::Debug);
    ConsoleAppender * consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat("[%-5l] %t{HH:mm:ss.zzz} %F line: %i  <%C> %m\n");
    Logger::registerAppender(consoleAppender);
}

void MTSP::processData(const QVariant & inputData)
{
    RequestParameters rp;
    rp.fromData(inputData);

    if(rp.command == "Load") {
        qDebug() << getID() << "Loading...";
        setupGUI();
    }
    else
    if(rp.command == "Run") {
        qDebug() << getID() << "Running...";
        QtConcurrent::run(MTSP::concurrentRun, this);
    }
    else
    if(rp.command == "SetData") {
        qDebug() << getID() << "Setting Data...";
        Data data(rp.value);
        int numberOfGenes = data["genesQuantity"].toInt();
        if(numberOfGenes) genesQuantity = numberOfGenes;
        int numberOfGenerations = data["maxGeneration"].toInt();
        if(numberOfGenerations) maxGeneration = numberOfGenerations;
    }
    else
        qDebug() << getID() << "got unsupported command" << rp.toData().toString();

}

bool MTSP::hasItem(const QString &name) const
{
    return graph.getNodes().keys().contains(name);
}

void MTSP::processMulticastData(const QVariant & inputData)
{
    RequestParameters rp;
    rp.fromData(inputData);

    qDebug() << getID() << "::processData()" << rp.toData().toString();

    if(rp.receiverID == getID())
        processData(inputData);
}

void MTSP::setupGUI()
{
    // настройка параметров интерфейса
    GuiProperties guiProperties;

    GUI_VisualizeProperties vs;
    vs.animation = true;

    guiProperties.add(vs.toData());

    GUI_SplitterProperties gs;

    gs.mainScreenHeight = 600;
    gs.bottomPanelHeight = 200;

    guiProperties.add(gs.toData());

    GUI_ApplicationProperties ga;
    ga.state = GUI_ApplicationProperties::maximize;

    guiProperties.add(ga.toData());

    GUI_LinkProperties gpLink;
    gpLink.linkType = "simple link";
    gpLink.lineColor = 234;
    gpLink.lineWidth = 0.35;

    guiProperties.add(gpLink.toData());

    RequestParameters rp =
            parametersForVisualize();

    // вывод строки приветствия
    showMessage(getID() + " model run");

    rp.command = "setGUIproperties";
    rp.value = guiProperties;
    emit sendRequestData(rp.toData());

    // элементы GUI для управления моделью
    rp.command = "addElementGUI";

    GUI_ElementProperties ep;
    ep.variableName = "label1";
    ep.value = "vertex number:";
    ep.widgetType = "Label";

    rp.value = ep.toData();
    emit sendRequestData(rp.toData());

    ep.variableName = "genesQuantity";
    ep.widgetType = "LineEdit";
    ep.value = genesQuantity;

    rp.value = ep.toData();
    emit sendRequestData(rp.toData());

    ep.variableName = "label2";
    ep.value = "maximum generations:";
    ep.widgetType = "Label";

    rp.value = ep.toData();
    emit sendRequestData(rp.toData());

    ep.variableName = "maxGeneration";
    ep.widgetType = "LineEdit";
    ep.value = maxGeneration;

    rp.value = ep.toData();
    emit sendRequestData(rp.toData());

    ep.variableName = "OK";
    ep.widgetType = "PushButton";
    ep.value = "OK";

    rp.value = ep.toData();
    emit sendRequestData(rp.toData());
}

void MTSP::run()
{
    // инициализация генератора псевдослучайных чисел
    srand((unsigned)time(NULL));

    // пусть размер популяции будет равен количеству вершин графа
    // чем больше размер популяции - тем больше количество наилучших переходов
    const int populationQuantity = genesQuantity;

    // матрица весов дуг графа (переходов между вершинами)
    QVector<Weights> U_weightMatrix;
/*
    U_weightMatrix
     << Weights(0 , 0) << Weights(1 , 1) << Weights(9 , 9) << Weights(9 , 9) << Weights(9 , 9) << Weights(9 , 9) << Weights(1 , 1)
     << Weights(1 , 1) << Weights(0 , 0) << Weights(1 , 1) << Weights(9 , 9) << Weights(9 , 9) << Weights(9 , 9) << Weights(9 , 9)
     << Weights(9 , 9) << Weights(1 , 1) << Weights(0 , 0) << Weights(1 , 1) << Weights(9 , 9) << Weights(9 , 9) << Weights(9 , 9)
     << Weights(9 , 9) << Weights(9 , 9) << Weights(1 , 1) << Weights(0 , 0) << Weights(1 , 1) << Weights(9 , 9) << Weights(9 , 9)
     << Weights(9 , 9) << Weights(9 , 9) << Weights(9 , 9) << Weights(1 , 1) << Weights(0 , 0) << Weights(1 , 1) << Weights(9 , 9)
     << Weights(9 , 9) << Weights(9 , 9) << Weights(9 , 9) << Weights(9 , 9) << Weights(1 , 1) << Weights(0 , 0) << Weights(1 , 1)
     << Weights(1 , 1) << Weights(9 , 9) << Weights(9 , 9) << Weights(9 , 9) << Weights(9 , 9) << Weights(1 , 1) << Weights(0 , 0);
*/
    // генерация матрицы весов
    U_weightMatrix = generateSquareMatrix(genesQuantity);

    // построение графа из сгенерированной матрицы весов
    graph = MTSP::buildGraph(U_weightMatrix);

    //graph.printArcs();

    // проверка количества вершин графа
    if(graph.nodesCount() != genesQuantity) return;

    // популяция - вектор списков названий вершин графа
    QVector< QStringList > population;

    // генерация популяции - добавление списков вершин графа
    for(int i = 0; i < populationQuantity; ++i)
        population << generateRandomPath(genesQuantity);

    //printPopulation(population);

    // случайный выбор точки кроссовера
    int crossPoint = rand()%genesQuantity;
    //qDebug() << "cross point number:" << crossPoint;

    // построение новых популяций
    int generationNum = 0;
    for(; generationNum < maxGeneration; ++generationNum) {

        // проверка на одинаковые пути в популяции
        if(checkForEqualsInPopulation(population)) break;

        // новая популяция
        QVector< QStringList > newPopulation;

        // построение особей новой популяции
        for(int childNumber = 0; childNumber < populationQuantity; ++childNumber) {

            // потомок
            QStringList child;

            // получение названия вершины точки кроссовера
            QString vertexFrom(population[childNumber][crossPoint]);

            // добавление первой вершины потомка
            child.append(vertexFrom);

            // добавление остальных вершин
            for(int k = 0, max = genesQuantity - 1; k < max; ++k) {

                // получение следующей вершины на основе жадного кроссовера
                vertexFrom = getNextVertex(vertexFrom, population, childNumber, child, graph);
                // добавление вершины
                child.append(vertexFrom);
            }

            // визуализация дуги графа от последней вершины к первой (замыкание)
            Data link = graph.getArc(child.back(), child.front());
            addItem(link);

            // добавление потомка к новой популяции
            newPopulation.append(child);
        }

        // замена исходной популяции новой популяцией
        population = newPopulation;

    }

    // вывод на экран особей последней сгенерированной популяции
    for(int i = 0, size = population.size(); i < size; ++i)
        qDebug() << "N" << i << ":" << population.at(i) << "weight:" << getSolutionWeight(population.at(i), graph);

    // если число попыток превышает максимальное - поиск не увенчался успехом
    if(generationNum == maxGeneration)
        qDebug() << "solution is not found, number of tryes exceeds maximum";
    else
        qDebug() << "solution found at" << generationNum << "generation";

}

// проверка на одинаковые пути в популяции
bool MTSP::checkForEqualsInPopulation(QVector< QStringList > & population) {

    // число особей в популяции
    int populationQuantity = 0;

    // число одинаковых особей (с циклическим смещением генов)
    int equalsCount = 1;

    if(!population.isEmpty() && !population.at(0).isEmpty()) {

        // первая вершина первой особи в популяции
        QString firstVertex(population.at(0).at(0));

        // количество особей в популяции
        populationQuantity = population.size();

        // проход по следующим особям популяции
        for(int i = 1; i < populationQuantity; ++i) {

            // нахождение первой вершины первой особи
            int offset = population.at(i).indexOf(firstVertex) ;

            bool equal = true;

            // сравнение порядка остальных вершин
            for(int j = 0, ji = offset; equal && (j < genesQuantity); ++j, ++ji)
            {
                if(population.at(i).at(ji) != population.at(0).at(j)) {
                    equal = false;
                    break;
                }

                if(ji == (genesQuantity - 1)) {
                    ji = -1;
                }
            }

            if(!equal) break;
            else
                ++equalsCount;
        }
    }

    // true если количество совпадений равно количеству особей
    return (equalsCount == populationQuantity);
}

// генерация матрицы весов
QVector<MTSP::Weights> MTSP::generateSquareMatrix(int size) {

    QVector<Weights> matrix;

    for(int i = 0, last = size - 1; i < size; ++i) {
        for(int j = 0; j < size; ++j) {

            Weights weights;

            // для формирования круга
            // дуги с весом 1 - наименьший путь
            // остальные - случайный вес больше 2-х
            if((i ==0) && (j == last))
                weights << 1 << 1;//(1+ rand()%9);
            else
            if((j ==0) && (i == last))
                weights << 1 << 1;//(1+ rand()%9);
            else
            if(j == i)
               weights << 0 << 0;
            else
            if(j == (i + 1))
                weights << 1 << 1;//(1+ rand()%9);
            else
                weights << (2+rand()%9) << (2+rand()%9);

            matrix << weights;
        }
    }
    return matrix;
}

// выбор следующей вершины на основе жадного кроссовера
QString MTSP::getNextVertex(
        // исходящая вершина
        const QString & vertexFrom_Name,
        // популяция
        QVector< QStringList > & population,
        // номер потомка в популяции
        int childNumber,
        // потомок
        const QStringList & nextSolution,
        // граф
        const QVMGraph & graph
        ) {

    // количество вершин в графе
    const int vertexCount = graph.nodesCount();

    // следующая вершина потомка
    QString nextVertex;

    // дуга с минимальным весом
    Data minWeightArc;

    // входящая вершина дуги
    QString vertexTo_Name;

    // перемещение потомка на 0 позицию в популяции
    // для последующего упрощения поиска
    for(int i = childNumber, max = population.size(); i < max; ++i)
        population[i].swap(population[i - childNumber]);

    // обход особей в популяции
    for(int i = 0, populationQuantity = population.size();
                         i < populationQuantity; ++i) {

        // поиск номера исходящей вершины в текущей особи
        int n1 = 0;
        while((n1 < vertexCount) &&
             (population[i][n1] != vertexFrom_Name)) ++n1;

        // поиск номера входящей вершины в текущей особи
        int n2 = n1;
        do {
            // это номер следующей вершины за исходящей
            ++n2;
            if(n2 == vertexCount) n2 = 0;
        } while(nextSolution.contains(population[i][n2]));

        // для первой особи в популяции: nextVertex = population[childNumber][n2]
        if(i == 0) {
            nextVertex = population[0][n2];
            vertexTo_Name = nextVertex;
            minWeightArc = graph.getArc(vertexFrom_Name, vertexTo_Name);
        }
        else
        {
            // получение дуги из n1 в n2
            vertexTo_Name = population[i][n2];
            const Data & arc(graph.getArc(vertexFrom_Name, vertexTo_Name));

            // если это последняя вершина особи или
            // вес дуги меньше веса дуги minWeightArc (жадный алгоритм)
            if((nextSolution.size() == (vertexCount - 1)) ||
                getArcWeight(arc) < getArcWeight(minWeightArc)) {
               minWeightArc = arc;
               nextVertex = vertexTo_Name;
            }
        }

    }

    vertexTo_Name = nextVertex;
    addItem(graph.getArc(vertexFrom_Name, vertexTo_Name));

    return nextVertex;
}

// получение веса дуги
float MTSP::getArcWeight(const Data & arc, int weightsCount) {
    float weight = 0;
    for(int i = 0; i < weightsCount; ++i) {
        weight += arc["weight_"+QString::number(i)].toInt();
    }

    return weight;
}

// получение веса особи
float MTSP::getSolutionWeight(const QStringList & solution, const QVMGraph & graph) {

    float weight = 0;

    if(!solution.isEmpty() && !graph.getNodes().isEmpty()) {

        QString from;

        for(int i = 0, size = solution.size(); i < size; ++i) {

            QString to(solution.at(i));
            if(!from.isEmpty()) {
                const Data & arc(graph.getArc(from, to));
                weight += getArcWeight(arc);
            }

            from = to;
        }

        const QString & to(solution.front());

        const Data & arc(graph.getArc(from, to));
        weight += getArcWeight(arc);
    }

    return weight;
}

// построение графа из матрицы весов
QVMGraph MTSP::buildGraph(const QVector<Weights> & U_weightMatrix) {

    QVMGraph graph;

    // определение количества вершин в графе
    int Vcount = U_weightMatrix.size();
    for(int i = Vcount/2; i > 1 ; --i) {
        int testLength = Vcount/i;

        if((testLength == i) && (testLength*i == Vcount)) {
            Vcount = i;
            break;
        }
    }

    // матрица весов должна быть квадратной
    if(Vcount == U_weightMatrix.size()) {
        qDebug() << "matrix is not square";
        return graph;
    }
    else qDebug() << "matrix size:" << Vcount;

    // добавление узлов графа
    for(int i = 0; i < Vcount; ++i) {
        graph.insertNode(getVertexName(i));
        Data node = graph.getNode(getVertexName(i));
        addItem(node);
    }

    // добавление дуг
    Data arc;
    for(int i = 0; i < Vcount; ++i)
        for(int j = 0; j < Vcount; ++j) {
            if(i != j) {

                const Weights & weights(U_weightMatrix[i * Vcount + j]);
                // присвоение веса дуге
                for(int n = 0, size = weights.size(); n < size; ++n)
                    arc["weight_"+QString::number(n)] = weights.at(n);

                // добавление дуги
                graph.insertArc(getVertexName(i), getVertexName(j), arc);
            }
        }

    return graph;
}

// получение названия вершины
QString MTSP::getVertexName(int i) {
    return "vertex_"+QString::number(i);
}

// генерация особи (случайного пути)
QStringList MTSP::generateRandomPath(int vertexCount)
{
    QStringList population;

    while(population.size() < vertexCount) {

        QString vertexName = getVertexName(rand()%vertexCount);
        if(!population.contains(vertexName))
            population << vertexName;
    }

    return population;
}

// вывод на экран популяции
void MTSP::printPopulation(const QVector< QStringList > & population) {
    QString out = "population:\n";
    for(int i = 0, size = population.size(); i < size; ++i) {
        out += population.at(i).join(" ") +"\n";
    }
    qDebug() << out;
}
