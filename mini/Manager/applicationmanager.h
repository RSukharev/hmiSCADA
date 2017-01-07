#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include <QMap>

class ApplicationManager
{
public:

    void registerFunction(const QString & name, void* function(void*));

    void runFunction(const QString & name, const char * param = "");

    ApplicationManager();

    QMap<QString, void*(*)(void*)> functions;
};

#endif // APPLICATIONMANAGER_H
