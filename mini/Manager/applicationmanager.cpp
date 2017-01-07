#include "applicationmanager.h"
#include <QString>

void ApplicationManager::registerFunction(const QString &name, void *function(void *))
{
    functions.insert(name, function);
}

void ApplicationManager::runFunction(const QString &name, const char * param) {
    //void* result =
    functions[name]((void*)param);
}

ApplicationManager::ApplicationManager()
{

}
