#include <QApplication>
#include <QDebug>

#include "controller.h"

#include "Logger/ConsoleAppender.h"

int main(int argc, char *argv[])
{

#if QT_VERSION < 0x050000
  //QApplication::setGraphicsSystem("raster");
#endif

  Logger::filterLevel(Logger::LogLevel::Debug);
  ConsoleAppender * consoleAppender = new ConsoleAppender();
  consoleAppender->setFormat("[%-5l] %t{HH:mm:ss.zzz} %F line: %i  <%C> %m\n");
  Logger::registerAppender(consoleAppender);

  qDebug() << "Starting the application";
  QApplication a(argc, argv);

  qDebug() << "Starting controller";
  Controller::start();

  return a.exec();
}

