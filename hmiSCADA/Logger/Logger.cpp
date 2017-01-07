/*
  Copyright (c) 2010 Boris Moiseev (cyberbobs at gmail dot com)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2.1
  as published by the Free Software Foundation and appearing in the file
  LICENSE.LGPL included in the packaging of this file.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  Modified by Roman Sukharev at 2015
*/
// Local
#include "../Logger/Logger.h"
#include "../Logger/AbstractAppender.h"

// Qt
#include <QCoreApplication>
#include <QReadWriteLock>
#include <QSemaphore>
#include <QDateTime>
#include <QIODevice>
#include <QTextCodec>

// STL
#include <iostream>

class LogDevice : public QIODevice
{
  public:
    LogDevice()
      : m_semaphore(1)
    {}

    void lock(Logger::LogLevel logLevel, const char* file, int line, const char* function)
    {
      m_semaphore.acquire();

      if (!isOpen())
        open(QIODevice::WriteOnly);

      m_logLevel = logLevel;
      m_file = file;
      m_line = line;
      m_function = function;
    }

  protected:
    qint64 readData(char*, qint64)
    {
      return 0;
    }

    qint64 writeData(const char* data, qint64 maxSise)
    {
      if (maxSise > 0)
        Logger::write(m_logLevel, m_file, m_line, m_function, QString::fromLocal8Bit(QByteArray(data, maxSise)));

      m_semaphore.release();
      return maxSise;
    }

  private:
    QSemaphore m_semaphore;
    Logger::LogLevel m_logLevel;
    const char* m_file;
    int m_line;
    const char* m_function;
};


// Forward declarations
static void cleanupLoggerPrivate();

#if QT_VERSION >= 0x050000
static void qtLoggerMessageHandler(QtMsgType, const QMessageLogContext& context, const QString& msg);
#else
static void qtLoggerMessageHandler(QtMsgType type, const char* msg);
#endif

/**
 * \internal
 *
 * LoggerPrivate class implements the Singleton pattern in a thread-safe way. It uses a static pointer to itself
 * protected by QReadWriteLock
 *
 * The appender list inside the LoggerPrivate class is also protected by QReadWriteLock so this class could be safely
 * used in a multi-threaded application.
 */
class LoggerPrivate
{

    Logger::LogLevel filterLevel;

  public:

    static LoggerPrivate* m_self;
    static QReadWriteLock m_selfLock;

    static LoggerPrivate* instance()
    {
      LoggerPrivate* result = 0;
      {
        QReadLocker locker(&m_selfLock);
        result = m_self;
      }

      if (!result)
      {
        QWriteLocker locker(&m_selfLock);
        m_self = new LoggerPrivate;

#if QT_VERSION >= 0x050000
        qInstallMessageHandler(qtLoggerMessageHandler);
#else
        qInstallMsgHandler(qtLoggerMessageHandler);
#endif
        qAddPostRoutine(cleanupLoggerPrivate);
        result = m_self;
      }

      return result;
    }


    LoggerPrivate()
      : filterLevel(Logger::filterLevel()),
        m_logDevice(0)
    {}


    ~LoggerPrivate()
    {
      // Cleanup appenders
      QReadLocker appendersLocker(&m_appendersLock);
      foreach (AbstractAppender* appender, m_appenders)
        delete appender;

      // Cleanup device
      QReadLocker deviceLocker(&m_logDeviceLock);
      delete m_logDevice;
    }


    void registerAppender(AbstractAppender* appender)
    {
      QWriteLocker locker(&m_appendersLock);

      if (!m_appenders.contains(appender))
        m_appenders.append(appender);
      else
        std::cerr << "Trying to register appender that was already registered" << std::endl;
    }


    LogDevice* logDevice()
    {
      LogDevice* result = 0;
      {
        QReadLocker locker(&m_logDeviceLock);
        result = m_logDevice;
      }

      if (!result)
      {
        QWriteLocker locker(&m_logDeviceLock);
        m_logDevice = new LogDevice;
        result = m_logDevice;
      }

      return result;
    }

    void write(const QDateTime& timeStamp, Logger::LogLevel logLevel, const char* file, int line, const char* function,
               const QString& message)
    {

      QReadLocker locker(&m_appendersLock);

      if(logLevel >= filterLevel) {

          if (!m_appenders.isEmpty())
          {
            foreach (AbstractAppender* appender, m_appenders)
              appender->write(timeStamp, logLevel, file, line, function, message);
          }
          else
          {
            // Fallback
            QString result = QString(QLatin1String("[%1] <%2> %3")).arg(Logger::levelToString(logLevel), -7)
                                                                   .arg(function).arg(message);

            std::cerr << qPrintable(result) << std::endl;
          }

          if (logLevel == Logger::Fatal)
            exit(1);//abort();
      }
    }


    void write(Logger::LogLevel logLevel, const char* file, int line, const char* function, const QString& message)
    {
        if(logLevel >= filterLevel)
            write(QDateTime::currentDateTime(), logLevel, file, line, function, message);
    }


    void write(Logger::LogLevel logLevel, const char* file, int line, const char* function, const char* message)
    {
        if(logLevel >= filterLevel)
            write(logLevel, file, line, function, QString(message));
    }


    QDebug write(Logger::LogLevel logLevel, const char* file, int line, const char* function)
    {
      LogDevice* d = logDevice();

      if(logLevel >= filterLevel)
          d->lock(logLevel, file, line, function);

      return QDebug(d);
    }


    void writeAssert(const char* file, int line, const char* function, const char* condition)
    {
        write(Logger::Fatal, file, line, function, QString("ASSERT: \"%1\"").arg(condition));
    }

  private:
    QList<AbstractAppender*> m_appenders;
    QReadWriteLock m_appendersLock;

    LogDevice* m_logDevice;
    QReadWriteLock m_logDeviceLock;
};

// Static fields initialization
LoggerPrivate* LoggerPrivate::m_self = 0;
QReadWriteLock LoggerPrivate::m_selfLock;


static void cleanupLoggerPrivate()
{
  QWriteLocker locker(&LoggerPrivate::m_selfLock);

  delete LoggerPrivate::m_self;
  LoggerPrivate::m_self = 0;
}

#if QT_VERSION >= 0x050000
static void qtLoggerMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
  Logger::LogLevel level = Logger::Fatal;
  switch (type)
  {
    case QtInfoMsg:
      level = Logger::Info;
      break;
  case QtDebugMsg:
      level = Logger::Debug;
      break;
    case QtWarningMsg:
      level = Logger::Warning;
      break;
    case QtCriticalMsg:
      level = Logger::Error;
      break;
    case QtFatalMsg:
      level = Logger::Fatal;
      break;
  }

  Logger::write(level, context.file, context.line, context.function, msg);
}

#else

static void qtLoggerMessageHandler(QtMsgType type, const char* msg)
{
  switch (type)
  {
    case QtDebugMsg:
      LOG_DEBUG(msg);
      break;
    case QtWarningMsg:
      LOG_WARNING(msg);
      break;
    case QtCriticalMsg:
      LOG_ERROR(msg);
      break;
    case QtFatalMsg:
      LOG_FATAL(msg);
      break;
  }
}
#endif


QString Logger::levelToString(Logger::LogLevel logLevel)
{
  switch (logLevel)
  {
    case Trace:
      return QLatin1String("Trace");
    case Debug:
      return QLatin1String("Debug");
    case Info:
      return QLatin1String("Info");
    case Warning:
      return QLatin1String("Warning");
    case Error:
      return QLatin1String("Error");
    case Fatal:
    case None:
      return QLatin1String("Fatal");
  }

  return "Undefined logger level";
}


Logger::LogLevel Logger::levelFromString(const QString& s)
{
  QString str = s.trimmed().toLower();

  LogLevel result = Debug;

  if (str == QLatin1String("trace"))
    result = Trace;
  else if (str == QLatin1String("info"))
    result = Info;
  else if (str == QLatin1String("warning"))
    result = Warning;
  else if (str == QLatin1String("error"))
    result = Error;
  else if (str == QLatin1String("fatal"))
    result = Fatal;

  return result;
}


void Logger::registerAppender(AbstractAppender* appender)
{
  LoggerPrivate::instance()->registerAppender(appender);
}


void Logger::write(const QDateTime& timeStamp, LogLevel logLevel, const char* file, int line, const char* function,
                   const QString& message)
{
  LoggerPrivate::instance()->write(timeStamp, logLevel, file, line, function, message);
}


void Logger::write(LogLevel logLevel, const char* file, int line, const char* function, const QString& message)
{  //PRINT_FILE_LINE;
   // printf("file:%s line:%d message: %s\n", file, line, qPrintable(message));
  LoggerPrivate::instance()->write(logLevel, file, line, function, message);
}


void Logger::write(LogLevel logLevel, const char* file, int line, const char* function, const char* message, ...)
{
  va_list va;
  va_start(va, message);
  LoggerPrivate::instance()->write(logLevel, file, line, function, QString().vsprintf(message,va));
  va_end(va);
}

QDebug Logger::write(LogLevel logLevel, const char* file, int line, const char* function)
{
  return LoggerPrivate::instance()->write(logLevel, file, line, function);
}


void Logger::writeAssert(const char* file, int line, const char* function, const char* condition)
{
  LoggerPrivate::instance()->writeAssert(file, line, function, condition);
}
