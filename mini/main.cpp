#include "mini.h"
#include <QApplication>

#include "../Logger/ConsoleAppender.h"

#include "Profiler/profiler.h"

std::string appID = qPrintable(QString("mini") + QString::number(getTimeNS()));

int main(int argc, char *argv[])
{
    //Logger::filterLevel(Logger::LogLevel::Debug);
    ConsoleAppender * consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat("[%-5l] %t{HH:mm:ss.zzz} %F line: %i  <%C> %m\n");
    Logger::registerAppender(consoleAppender);

    qDebug() << "Starting the application";

    QApplication a(argc, argv);
    mini w;
    w.show();

    return a.exec();
}
