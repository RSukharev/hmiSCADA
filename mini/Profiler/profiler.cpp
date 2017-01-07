#include "profiler.h"

#include "../ProfilerNode/profilernode.h"

#include <iostream>
#include <fstream>
#include <string>

Profiler::Profiler(const std::string & nodeInfo, const std::string & moduleInfo, const std::string & fileName) :
    started(false)
{
    testData.nodeInfo = nodeInfo;
    testData.moduleInfo = moduleInfo;
    // получение содержимого файла исходного кода
    getContentFromFile(fileName, sourceCode);
}

void Profiler::start(const char *fileName, int line, const std::string & testInfo) {

    if(!started) {
        started = true;
        testData.clear();
        testData.testInfo = testInfo;
        testData.sourceCodeFileName = fileName;
        testData.beginLineNumber = line + 1;
        ProfilerNode::startTest(testData);
        testData.startTime = getTimeNS();
    }
    else
        qDebug() << "profiler already started";
}

void Profiler::addCustomResults(
                    const char * testInfo,
                   long int startTime, long int endTime,
                   const std::string &testBlock)
{
    if(!started) {
        started = true;
        testData.clear();
        testData.testInfo = testInfo;
        testData.startTime = startTime;
        testData.stopTime = endTime;
        testData.testBlock = testBlock;
    }
    else
        qDebug() << "profiler already started";

    stop(custom, "", 0);
}

void Profiler::addFlowFrom(const std::string & from, const std::string & flowInfo)
{
    qDebug() << "flow from " << from.c_str() << " info " << flowInfo.c_str();
}

void Profiler::addFlowTo(const std::string & to, const std::string & flowInfo)
{
    qDebug() << "flow to " << to.c_str() << " info " << flowInfo.c_str();
}

void Profiler::onExit() {
    ProfilerNode::shutdown();
}

/* процедура завершения профилирования
    параметры:
    название файла исходного кода
    номер строки с PROFILE_STOP(...)
*/
void Profiler::stop(TestDataType testDataType, const char *fileName, int line) {

    started = false;

    // тип тестовых данных
    testData.testDataType = testDataType;

    // проверка названия файла исходного кода
    if(fileName != testData.sourceCodeFileName)
        printf("profile error!! filename is changed");

    // если профилируется в автоматическом режиме
    if(testData.testBlock.empty()) {

        // время окончания профилирования
        testData.stopTime = getTimeNS();

        // номер завершающей строки профилируемого блока
        testData.endLineNumber = line - 1;

        // поиск профилируемого блока
        size_t pos = 0;
        for(int i = 0, max = testData.beginLineNumber - 1; i < max; ++i, ++pos) {
            pos = sourceCode.find("\n", pos);
        }

        size_t endPos = pos+1;
        for(int i = testData.beginLineNumber; i <= testData.endLineNumber; ++i, ++endPos) {
            endPos = sourceCode.find("\n", endPos);
        }

        // извлечение профилируемого блока из исходника
        std::string code = sourceCode.substr(pos, endPos - pos - 1);
        testData.testBlock = code;
    }

    ProfilerNode::stopTest(testData);
}

//#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

void Profiler::getContentFromFile(const std::string & fileName, std::string & content)
{
    content.clear();

    //qDebug() << "fromFile:" << fileName.c_str();

    FILE *f = fopen(fileName.c_str(), "rb");
    if(f != NULL) {

        const long bufSize = 8192;
        char buf[bufSize];
        memset(buf, 0, sizeof(buf));

        while (!feof(f)) {
              fread(buf, 1, bufSize, f);       // считываем файл в буфер
              content += buf;
              memset(buf, 0, sizeof(buf));
        }
    }
    else {
        //qDebug() << "Error opening file";
    }
}

#include <QDateTime>
long long getTimeNS()
{
   return QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
}
