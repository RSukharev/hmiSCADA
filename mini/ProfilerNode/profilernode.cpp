#include <QMutex>

#include "profilernode.h"

#include "../Logger/ConsoleAppender.h"

#include <QDateTime>
#include <QObject>

Data ProfilerNode::testNode(const TestData & testData) {
    QString testName;
    testName += (testData.nodeInfo + ".").c_str();
    testName += (testData.moduleInfo + ".").c_str();
    testName += testData.testInfo.c_str();

    Data testNode = QVMGraph::simpleNode(testName);
    testNode["startTime"] = static_cast<long long>(testData.startTime);
    testNode["endTime"] = static_cast<long long>(testData.stopTime);
    testNode["info"] = testData.testInfo.c_str();
    return testNode;
}

void ProfilerNode::startTest(const TestData & testData) {

    ProfilerNode * instance = 0;
    if(!instance) {
        instance = getInstance(testData.nodeInfo);
        assert(instance);
    }

    Data test = testNode(testData);
    test["nodeState"] = "on";
    test["info"] = "ProfilerNode started test block:  " + test["info"].toString();
    instance->visualise(test);
}


void ProfilerNode::visualise(const Data & node) {

    //qDebug() << qPrintable(node.toString());
    ////////////////////////////////////////////
    m_agentUdp->addItem(node);
    Data flow = QVMGraph::simpleArc(m_parentNodeName, node["name"].toString());
    m_agentUdp->addItem(flow);
    m_parentNodeName = node["name"].toString();
}

void ProfilerNode::stopTest(const TestData & testData) {

    ProfilerNode * instance = 0;
    if(!instance) {
        instance = getInstance(testData.nodeInfo);
        assert(instance);
    }

    Data test = testNode(testData);
    test["nodeState"] = "pause";
    //instance->agentUdp->addItem(test);

    // отображение результатов профилирования
    showTestResult(testData);
}

void ProfilerNode::shutdown() {
    getInstance()->onExitSignal();
}

void ProfilerNode::onExitSignal()
{
    m_agentUdp->removeMe();
}

void ProfilerNode::showTestResult(const TestData &testResults) {

    long long testEndTime = getTimeNS();

    QString childName;
    childName += (testResults.nodeInfo + ".").c_str();
    childName += (testResults.moduleInfo + ".").c_str();
    childName += testResults.testInfo.c_str();

    QString info("Test block:");
    //info += testResults.testInfo.c_str();
    info += QString("\nNode: ") + testResults.nodeInfo.c_str();
    info += QString("\nModule: ") + testResults.moduleInfo.c_str();

    QString testType;
    switch(testResults.testDataType) {
        case flow:
                testType = "data flow";
                break;
        case profileBlock:
                testType = "profile block";
                break;
        case tracePoint:
                testType = "trace point";
                break;
    case custom:
            testType = "custom data";
            break;
    }

    info += QString("\nTest type: ") + testType;

    if(!testResults.sourceCodeFileName.empty()) {
        info += "\nProfiling file: \"";
        info += testResults.sourceCodeFileName.c_str();
        info += "\"";
    }

    if(!testResults.testBlock.empty()) {
        info += QString("\nProfiling block:\n") +
                    testResults.testBlock.c_str();
    }

    if(testResults.testDataType == tracePoint) {

        int currentPoint = testResults.endLineNumber + 1;

        if(testResults.testInfo.empty())
            childName +=
                    QString::number(currentPoint);

        info += QString("\nLine number: ") +
                QString::number(currentPoint);
    }
    else  {

        if(testResults.beginLineNumber != testResults.endLineNumber) {
            QString lineNumbers = QString::number(testResults.beginLineNumber) +
                    " - " + QString::number(testResults.endLineNumber);
            info += "\nLine numbers:" + lineNumbers;
            if(testResults.testInfo.empty())
                childName += lineNumbers;
        }
        else {
            QString lineNumber = QString::number(testResults.endLineNumber);
            info += "\nLine number:" + lineNumber;
            if(testResults.testInfo.empty())
                childName += lineNumber;
        }

        info += QString("\nJob time: ") +
        QString::number(testResults.stopTime - testResults.startTime) + " ms";
    }

    static ProfilerNode * instance = 0;
    if(!instance) {
        instance = getInstance(testResults.nodeInfo);
        //instance->parentNodeName = testResults.nodeInfo.c_str();
    }
     assert(instance);

     //qDebug() << "child:" << childName << " parent:" << instance->parentNodeName << " Info:" << qPrintable(info) ;

    Data childNode = instance->m_agentUdp->createChild(instance->m_parentNodeName, childName);
    //info = "Parent:" + parentNodeName + "\n" + info;
    childNode["info"] = info;
    childNode["testEndTime"] = static_cast<long long>(testEndTime);
    childNode["parentTestEndTime"] = instance->m_parentTestEndTime;

    //static long long parentEpochEndTime = getTimeNS();
    //long long childEpochTime = getTimeNS();

    //////////////////////////////////////////////////////
    instance->m_agentUdp->addItem(childNode);
    // qDebug() << qPrintable(childNode.toString());

    /*

    Data flow = QVMGraph::simpleArc(instance->parentNodeName, childName);
    long long deltaTime = childEpochTime-parentEpochEndTime;

    flow["info"] = instance->parentNodeName + "->" + childName +  " time:" + QString::number(deltaTime) + " ms";

    deltaTime = (testResults.stopTime - testResults.startTime)/1000000;
    if(deltaTime < 3) deltaTime = 3;

    flow["showTimeCounter"] = deltaTime;
    //instance->agentUdp->addItem(flow);

    instance->parentNodeName = childName;
    instance->parentTestEndTime = testEndTime;

    parentEpochEndTime = childEpochTime;
*/
}

ProfilerNode::ProfilerNode(const std::string &nodeName)  :
    m_agentUdp(0)
{
    qDebug() << "node " << nodeName.c_str() << " created";

    m_applicationNode = QVMGraph::simpleNode(nodeName.c_str(), "PC");
    m_applicationNode["info"] = ("Node of distributed application \"" + nodeName + "\"").c_str();

    m_agentUdp = new AgentUdp(m_applicationNode);
    assert(m_agentUdp);

    m_agentUdp->sendTextMessage("profiler node added");

    m_agentUdp->setMenu();

    m_parentTestEndTime = 0;
    m_parentNodeName = nodeName.c_str();
}

ProfilerNode *ProfilerNode::getInstance(std::string nodeName) {

   // qDebug() << "node name:" << nodeName.c_str();

    static ProfilerNode * instance = 0;
    if(!instance) {
        instance = new ProfilerNode(nodeName);
    }

    return instance;
}
