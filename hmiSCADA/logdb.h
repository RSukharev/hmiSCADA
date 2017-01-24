#ifndef LOGDB_H
#define LOGDB_H

#include <QtSql>
#include <QDebug>

class LogDB {

public:

    LogDB() : m_query(0) {
        QSqlError err = init();
        if (err.type() != QSqlError::NoError) {
            qDebug() << err;
            exit(0);
        }
    }

    ~LogDB() {
        if(m_query) delete m_query;
    }

    void addLog(const QDateTime &logdate, const QString &info)
    {
        if(m_query != 0) {
        m_query->addBindValue(logdate);
        m_query->addBindValue(info);
        m_query->exec();
        }
    }

private:

    QSqlError init()
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(":memory:");

            if (!db.open())
                return db.lastError();

            QStringList tables = db.tables();
            if (tables.contains("logs", Qt::CaseInsensitive))
                return QSqlError();

            m_query = new QSqlQuery;

        if (!m_query->exec(QLatin1String("create table logs(id integer primary key, logdate datetime, info varchar)")))
                    return m_query->lastError();

        if (!m_query->prepare(QLatin1String("insert into logs(logdate, info) values(?, ?)")))
           return m_query->lastError();

        return QSqlError();
    }

    QSqlQuery * m_query;

};

#endif // LOGDB_H
