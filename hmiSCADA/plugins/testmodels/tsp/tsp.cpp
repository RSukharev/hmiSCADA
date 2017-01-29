
#include <ctime>
#include "tsp.h"
#include "../../../scene/guiproperties.h"



#include "../../../../Logger/ConsoleAppender.h"

#if QT_VERSION < 0x050000

Q_EXPORT_PLUGIN2( IPlugin, TSP )

#include <QtConcurrentRun>
#else
#include <QtConcurrent/QtConcurrentRun>
#endif


void TSP::registerCommands()
{
    QObject::connect(this, SIGNAL(dataReceived(Data)), this, SLOT(onRunCommand(Data)));

    m_commander.setClassID(getID());
    m_commander.registerCommand("Run", &TSP::run);
    m_commander.registerCommand("Load", &TSP::setupGUI);
    m_commander.registerCommand("SetData", &TSP::setData);
    m_commander.registerCommand("Stop", &TSP::stop);
    m_commander.registerCommand("MenuItemSelected", &TSP::onMenuItemSelected);
    m_commander.registerCommand("Shutdown", &TSP::onClearScene);
}

void TSP::onMenuItemSelected(const QVariant inputData)
{
    Request rp;
    rp.fromData(inputData);

    if(rp.value.toString() == "info")
       qDebug() << rp.receiverID << ":" << m_graph.getNode(rp.receiverID.toString()).value("info").toString();
    else
       qDebug() << "process menu item:" << rp.value.toString();
}

void TSP::onClearScene(const QVariant)
{
    Request rp;
    rp.receiverID = "Visualiser";
    rp.command = "clearScene";
    rp.value = "clearScene";
    rp.senderID = getID();

    sendRequestData(rp.toData());
}

void TSP::processData(const QVariant &data) {
    emit dataReceived(data);
}

void TSP::onRunCommand(const Data data)
{
    m_commander.run(this, data["command"], data["value"]);
}

TSP::TSP() :
    m_genesQuantity(16),
    m_chromosomesQuantity(10),
    m_maxGeneration(10),
    m_flagRunning(false)
{
    Logger::filterLevel(Logger::Debug);
    ConsoleAppender * consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat("[%-5l] %t{HH:mm:ss.zzz} %F line: %i  <%C> %m\n");
    Logger::registerAppender(consoleAppender);

    registerCommands();
}

// обработка данных, адресованных этому плагину
void TSP::setData(const QVariant inputData)
{
    Data data(inputData);
    qDebug() << getID() << "Setting Data..." << qPrintable(data.toString());

    m_genesQuantity = data["genesQuantity"].toInt();
    m_chromosomesQuantity = data["chromosomesQuantity"].toInt();
    m_maxGeneration = data["maxGeneration"].toInt();
}

void TSP::stop(const QVariant) {
    qDebug() << getID() << "got ''stop'' command";
    m_flagRunning = false;
}

void TSP::processMulticastData(const QVariant & inputData)
{
    Request rp;
    rp.fromData(inputData);

    qDebug() << getID() << "::processData()" << rp.toData().toString();

    if(rp.receiverID == getID())
        processData(inputData);
}

void TSP::setupGUI(const QVariant)
{
    showMessage(getID().toString() + "::setupGUI");

    GuiProperties guiProperties;

    GUI_VisualiseProperties vs;
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

    // элементы управления моделью
    GUI_ElementList modelGUIelements;

    GUI_ElementProperties ep;
    ep.variableName = "label1";
    ep.value = "Genes:";
    ep.widgetType = "Label";
    modelGUIelements.add(ep.toData());

    ep.variableName = "genesQuantity";
    ep.widgetType = "LineEdit";
    ep.value = m_genesQuantity;
    modelGUIelements.add(ep.toData());

    ep.variableName = "label2";
    ep.value = "Chromosomes:";
    ep.widgetType = "Label";
    modelGUIelements.add(ep.toData());

    ep.variableName = "chromosomesQuantity";
    ep.widgetType = "LineEdit";
    ep.value = m_chromosomesQuantity;
    modelGUIelements.add(ep.toData());

    ep.variableName = "3";
    ep.value = "Maximum generation:";
    ep.widgetType = "Label";
    modelGUIelements.add(ep.toData());

    ep.variableName = "maxGeneration";
    ep.widgetType = "LineEdit";
    ep.value = m_maxGeneration;
    modelGUIelements.add(ep.toData());

    ep.variableName = "OK";
    ep.widgetType = "PushButton";
    ep.value = "OK";
    modelGUIelements.add(ep.toData());

    guiProperties["elementsGUI"] = modelGUIelements;

    Request rp =
            parametersForVisualise();

    // настройка параметров интерфейса
    rp.command = "setGUIproperties";
    rp.value = guiProperties;
    emit sendRequestData(rp.toData());

}

void TSP::run(const QVariant)
{
    QtConcurrent::run(this, &TSP::concurrentRun);
    //concurrentRun();
}

void TSP::concurrentRun() {

    m_flagRunning = true;

    srand((unsigned)time(NULL));

    QVector<int> U_weightMatrix = generateSquareMatrix(m_genesQuantity);


//    U_weightMatrix
//            << 0 << 1 << 9 << 9 << 9 << 9 << 1
 //           << 1 << 0 << 1 << 9 << 9 << 9 << 9
  //          << 9 << 1 << 0 << 1 << 9 << 9 << 9
   //         << 9 << 9 << 1 << 0 << 1 << 9 << 9
    //        << 9 << 9 << 9 << 1 << 0 << 1 << 9
     //       << 9 << 9 << 9 << 9 << 1 << 0 << 1
      //      << 1 << 9 << 9 << 9 << 9 << 1 << 0;


    // create graph with data from weight matrix
    m_graph = TSP::buildGraph(U_weightMatrix);

    // check graph - compare graph nodes quantity with vertexQuantity
    if(m_graph.nodesCount() != m_genesQuantity) return;

    // create population
    QVector< QVector<int> > chromosomeList;

    // fill population with random solutions
    for(int i = 0; i < m_chromosomesQuantity; ++i)
        chromosomeList << generateRandomSolution(m_genesQuantity);

    // set crosspoint
    int crossPoint = rand()%m_genesQuantity;
    showMessage("cross point number:" + QString::number(crossPoint));

    bool solutionFound = false;
    int generationNum = 0;

    // build new generations
    for(; generationNum < m_maxGeneration; ++generationNum) {

        // create new population
        QVector< QVector<int> > newGeneration;

        // build children in generation
        for(int childNumber = 0; childNumber < m_chromosomesQuantity; ++childNumber) {

            // new child solution
            QVector<int> child;

            // vertex in cross point
            int vertexFrom = chromosomeList[childNumber][crossPoint];

            // add first point
            child << vertexFrom;

            for(int k = 0, max = m_genesQuantity - 1; k < max; ++k) {

                // exit on stop command
                if(!m_flagRunning) return;

                vertexFrom = getNextVertex(vertexFrom, chromosomeList, childNumber, child, m_graph);
                child << vertexFrom;
            }

            // draw link from back to front
            addItem(QVMGraph::simpleArc(
                        getNodeName(child.back()),
                        getNodeName(child.front())));

            // add child to population
            newGeneration.append(child);
        }

        // replace source population with new generation
        int equalCount = 1;
        int firstGene = chromosomeList.at(0).at(0);
        for(int i = 1; i < m_chromosomesQuantity; ++i) {
            int offset = chromosomeList.at(i).indexOf(firstGene) ;
            bool equal = true;
            for(int j = 0, ji = offset; equal && (j < m_genesQuantity); ++j, ++ji)
            {
                // exit on stop command
                if(!m_flagRunning) return;

                if(chromosomeList.at(i).at(ji) != chromosomeList.at(0).at(j)) {

                    equal = false;
                    break;
                }

                if(ji == (m_genesQuantity - 1)) {
                    ji = -1;
                }
            }

            if(!equal) break;
            else
                ++equalCount;
        }

        if(equalCount == m_chromosomesQuantity) {

            solutionFound = true;
            break;
        }

        chromosomeList = newGeneration;
    }

    for(int i = 0, size = chromosomeList.size(); i < size; ++i) {

        // exit on stop command
        if(!m_flagRunning) return;

        QString genes;
        const QVector<int> & chromosome = chromosomeList.at(i);

        foreach(int gene, chromosome) {
            genes += QString::number(gene) + ",";
        }
        genes.chop(1);
        showMessage("chromosome N" + QString::number(i) + ": " + genes);
    }

    if(solutionFound)
        showMessage("solution found at " + QString::number(generationNum+1) + " generation");
    else
        showMessage("Error: solution not found!");
}

QVector<int> TSP::generateSquareMatrix(int size) {

    QVector<int> matrix;

    for(int i = 0, last = size - 1; i < size; ++i) {
        for(int j = 0; j < size; ++j) {

            if((i ==0) && (j == last))
                matrix << 1;
            else
            if((j ==0) && (i == last))
                matrix << 1;
            else
            if(j == i)
               matrix << 0;
            else
            if(j == (i + 1))
                matrix <<  1;
            else
                matrix << 2+rand()%9;
        }
    }
    return matrix;
}

int TSP::getNextVertex(
        int fromVertex,
        QVector< QVector<int> > & population,
        int childNumber,
        const QVector<int> & nextSolution,
        const QVMGraph & graph
        ) {

    // vertex count
    const int vertexCount = graph.nodesCount();

    int nextVertex = -1;

    Data minWeightArc;

    QString vertexFrom_Name, vertexTo_Name;

    for(int i = childNumber, max = population.size(); i < max; ++i)
        population[i].swap(population[i - childNumber]);

    for(int i = 0, chromosomesQuantity = population.size();
                         i < chromosomesQuantity; ++i) {

        int n1 = 0;
        // find number of vertexFrom in current solution
        while((n1 < vertexCount) &&
             (population[i][n1] != fromVertex)) ++n1;

        // get name of vertexFrom
        vertexFrom_Name = getNodeName(population[i][n1]);

        // find number of vertexTo in current population
        int n2 = n1;
        do {
            ++n2;
            if(n2 == vertexCount) n2 = 0;
        } while(nextSolution.contains(population[i][n2]));

        // for the first time nextVertex = population[childNumber][n2]
        if(i == 0) {
            nextVertex = population[i][n2];
            vertexTo_Name = getNodeName(nextVertex);
            minWeightArc = graph.getArc(vertexFrom_Name, vertexTo_Name);
        }
        else
        {
            // check for the weight of the arc from n1 to n2
            vertexTo_Name = getNodeName(population[i][n2]);
            Data arc(graph.getArc(vertexFrom_Name, vertexTo_Name));

            // if last member of solution or has minimum weight
            if((nextSolution.size() == (vertexCount - 1)) ||
                arc["weight"].toInt() < minWeightArc["weight"].toInt()) {
               minWeightArc = arc;
               nextVertex = population[i][n2];
            }
        }
    }

    vertexTo_Name = getNodeName(nextVertex);

    minWeightArc =
            graph.getArc(vertexFrom_Name, vertexTo_Name);

    addItem(minWeightArc);
    //qDebug() << "min weight arc:" << minWeightArc.toString();

    return nextVertex;
}

int TSP::getSolutionWeight(const QVector<int> & genes, const QVMGraph & graph) {
    int weight = 0;

    if(!genes.isEmpty() && !graph.getNodes().isEmpty()) {

        int from = -1;

        for(int i = 0, size = genes.size(); i < size; ++i) {

            int to = genes.at(i);
            if(from != -1) {
                Data arc(graph.getArc(getNodeName(from), getNodeName(to)));
                weight += arc["weight"].toInt();
            }

            from = to;
        }

        int to = genes.front();

        Data arc(graph.getArc(getNodeName(from), getNodeName(to)));
        weight += arc["weight"].toInt();
    }

    return weight;
}

QVMGraph TSP::buildGraph(const QVector<int> & U_weightMatrix) {

    QVMGraph graph;

    int Vcount = U_weightMatrix.size();

    for(int i = Vcount/2; i > 1 ; --i) {
        int testLength = Vcount/i;

        if((testLength == i) && (testLength*i == Vcount)) {
            Vcount = i;
            break;
        }
    }

    if(Vcount == U_weightMatrix.size()) {
        qDebug() << "matrix is not square";
        return graph;
    }
    else qDebug() << "matrix size:" << Vcount;

    for(int i = 0; i < Vcount; ++i) {
        graph.insertNode(getNodeName(i));
        Data node = graph.getNode(getNodeName(i));
        addItem(node);
    }


    for(int i = 0; i < Vcount; ++i)
        for(int j = 0; j < Vcount; ++j) {
            if(i != j) {
                Data arc = QVMGraph::simpleArc(getNodeName(i), getNodeName(j));
                arc["weight"] = U_weightMatrix[i * Vcount + j];
                graph.insertArc(arc);
            }
        }

    return graph;
}

QString TSP::getNodeName(int i) {
    return "N"+QString::number(i);
}

QVector<int> TSP::generateRandomSolution(int vertexCount)
{
    QVector<int> population;

    while(population.size() < vertexCount) {

        int n = rand()%vertexCount;
        if(!population.contains(n))
            population << n;
    }

    return population;
}

