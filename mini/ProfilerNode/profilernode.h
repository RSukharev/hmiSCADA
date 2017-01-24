#ifndef PROFILERNODE_H
#define PROFILERNODE_H

#include "../Logger/ConsoleAppender.h"
#include "agentudp/agentudp.h"
#include "Profiler/profiler.h"

class ProfilerNode : public QObject
{
    Q_OBJECT

public:
    static void startTest(const TestData &data);
    static void stopTest(const TestData &data);
    static void shutdown();

private slots:
    void onExitSignal();

private:
    static Data testNode(const TestData &testData);

    ProfilerNode(const std::string & name);
    ~ProfilerNode() { }

    static ProfilerNode * getInstance(std::string name = "stub profiler node");

    Data m_applicationNode;

    QString m_parentNodeName;
    long long m_parentTestEndTime;

    AgentUdp * m_agentUdp;

};

#endif // PROFILERNODE_H
