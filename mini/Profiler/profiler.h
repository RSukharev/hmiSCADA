#ifndef PROFILER_H
#define PROFILER_H

#include <string>
#include <vector>

// получение времени с начала текущей "эпохи" в наносекундах
long long getTimeNS();

extern std::string appID;

// тип тестовых данных
enum TestDataType {
    tracePoint, profileBlock, flow, custom
};

enum FlowDataType { send, receive };

struct FlowData {
    FlowDataType type;
    std::string senderName;
    std::string receiverName;
};

// Структура данных профилировщика
struct TestData {

    // тип тестовых данных
    TestDataType testDataType;

    // информация о программе
    std::string nodeInfo;

    // информация о модуле программы
    std::string moduleInfo;

    // название теста
    std::string testInfo;

    // название файла исходного кода
    std::string sourceCodeFileName;

    // номер начальной строки профилируемого блока
    int beginLineNumber;

    // номер конечной строки профилируемого блока
    int endLineNumber;

    // время начала теста в наносекундах от начала "эпохи"
    long int startTime;

    // время завершения теста в наносекундах
    long int stopTime;

    // тестируемый блок исходного кода
    std::string testBlock;

    // инициализация данных
    TestData() {
        clear();
    }

    void clear() {
        testInfo.clear();
        beginLineNumber = 0;
        endLineNumber = 0;
        startTime = 0;
        stopTime = 0;
        testBlock.clear();
    }
};

// класс профилировщика
class Profiler
{
public:

    // конструктор
    Profiler(const std::string & nodeInfo, const std::string & moduleInfo, const std::string & fileName);

    // запуск профилирования
    void start(const char *fileName, int line, const std::string & testInfo = "");

    // останов профилирования
    void stop(TestDataType, const char * fileName, int line);

    //  запуск профилирования для отображения данных, введенных вручную
    void addCustomResults(const char * testInfo, long int startTime, long int endTime,
             const std::string &testBlock);

    // входящий поток
    void addFlowFrom(const std::string & from, const std::string & flowInfo);

    // исходящий поток
    void addFlowTo(const std::string & to, const std::string & flowInfo);

    void onExit();

private:
    // получение содержимого текстового файла
    void getContentFromFile(const std::string &fileName, std::string &content);

    // тестовые данные
    TestData testData;

    // файл исходника
    std::string sourceCode;

    bool started;
};

#ifdef PROFILE_ENABLE
#define PROFILE_BEGIN(nodeinfo, moduleinfo) Profiler _profiler_(nodeinfo, moduleinfo, __FILE__);

#define PROFILE_START(...) _profiler_.start( __FILE__, __LINE__, ##__VA_ARGS__);

#define PROFILE_STOP() _profiler_.stop(profileBlock, __FILE__, __LINE__);

#define TRACE_POINT(...)  _profiler_.start(__FILE__, __LINE__, ##__VA_ARGS__); \
                                        _profiler_.stop(tracePoint, __FILE__, __LINE__);

#define PROFILE_FLOW_FROM(from, flowInfo) _profiler_.addFlowFrom(from, flowInfo);

#define PROFILE_FLOW_TO(to, flowInfo) _profiler_.addFlowTo(to, flowInfo);

#define ADD_CUSTOM_RESULTS(testInfo, startTime, endTime, testBlock) \
            _profiler_.addCustomResults(testInfo, startTime, endTime, testBlock);

#define ON_EXIT(...) _profiler_.onExit();
#else
#define PROFILE_BEGIN(...) /* */
#define PROFILE_START(...) /* */
#define PROFILE_STOP() /* */
#define PROFILE_FLOW_FROM(...) /* */
#define PROFILE_FLOW_TO(...) /* */
#define TRACE_POINT(...) /* */
#define ADD_CUSTOM_RESULTS(...) /* */
#define ON_EXIT(...) /* */
#endif

#endif // PROFILER_H
