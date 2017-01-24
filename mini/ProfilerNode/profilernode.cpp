#include <QMutex>

#include "profilernode.h"

#include "../Logger/ConsoleAppender.h"

#include <QDateTime>
#include <QObject>

// make node data from TestData
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

// show node when test started
void ProfilerNode::startTest(const TestData & testData) {

    // create test node
    Data test_node = testNode(testData);
    test_node["nodeState"] = "on";
    test_node["info"] = "ProfilerNode started test block:  " + test_node["info"].toString();

    // prepare to show
    ProfilerNode * instance = 0;
    if(!instance) {
        instance = getInstance(testData.nodeInfo);
        assert(instance);
    }

    // show node
    instance->m_agentUdp->addItem(test_node);
    QString nodeName = test_node["name"].toString();

    // show flow
    Data flow = QVMGraph::simpleArc(instance->m_parentNodeName, nodeName);
    instance->m_agentUdp->addItem(flow);

    // set new parent node
    instance->m_parentNodeName = nodeName;
}

void ProfilerNode::shutdown() {
    getInstance()->onExitSignal();
}

void ProfilerNode::onExitSignal()
{
    m_agentUdp->removeMe();
}

// show node when test finished
void ProfilerNode::stopTest(const TestData &testResults) {

    long long testEndTime = getTimeNS();

    QString childName;
    childName += (testResults.nodeInfo + ".").c_str();
    childName += (testResults.moduleInfo + ".").c_str();
    childName += testResults.testInfo.c_str();

    QString info("Test block:");
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

    long int jobTime = testResults.stopTime - testResults.startTime;

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

        info += QString("\nJob time: ") + QString::number(jobTime) + " ms";
    }

    // prepare to show
    static ProfilerNode * instance = 0;
    if(!instance)
        instance = getInstance(testResults.nodeInfo);

     assert(instance);

    // create test node
    Data test_node = instance->m_agentUdp->createChild(instance->m_parentNodeName, childName);
    test_node["info"] = info;
    test_node["testEndTime"] = static_cast<long long>(testEndTime);
    test_node["parentTestEndTime"] = instance->m_parentTestEndTime;

    // set node state, based on job time
    if(jobTime < 300)
        test_node["nodeState"] = "on";
    else
        test_node["nodeState"] = "err";

    // show node
    instance->m_agentUdp->addItem(test_node);
}

ProfilerNode::ProfilerNode(const std::string &nodeName)  :
    m_agentUdp(0)
{
    //qDebug() << "node " << nodeName.c_str() << " created";

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
