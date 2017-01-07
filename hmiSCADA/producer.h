#ifndef PRODUCER_H
#define PRODUCER_H

#include <QVector>
#include <QThread>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QDebug>

#include "data.h"

/*
  Producer - class for processing queue in threads,
  using Model "one Producer and many Consumers"

  Wqueue - thread's safe queue
  Worker - interface class for processing data items in queue

  Example usage:

  SomeClass() : public Worker {
    Producer * producer;

    SomeClass() {
        producer = new Producer(this); // this - pointer to class Worker
    }

    void processData(const Data & data) { ... }

    void addDataToQueue(Data data) {

        // Producer enqueues data in it's own thread
        producer->enqueueData(data);

        // Consumers runs in other threads then Producer
        // and calls method worker->processData(data)
    }
  };
*/
template <typename T> class Wqueue
{
    QVector<T>   m_queue;
    QMutex m_mutex;
    QWaitCondition m_wait_cond;

public:
    Wqueue() {
    }
    ~Wqueue() {
    }
    void add(const T & item) {
        m_mutex.lock();
        m_queue.push_back(item);
        m_mutex.unlock();
        m_wait_cond.wakeAll();
    }
    T remove() {
        m_mutex.lock();

        while (m_queue.size() == 0)
            m_wait_cond.wait(&m_mutex);

        T item = m_queue.front();
        m_queue.pop_front();
        m_mutex.unlock();
        return item;
    }
    int size() {
        m_mutex.lock();
        int size = m_queue.size();
        m_mutex.unlock();
        return size;
    }
};

typedef Wqueue<QVariant> DataQueue;

class Worker {
public:
    virtual ~Worker() {}
    virtual void processData(const Data & data) = 0 ;
};

class Producer : public QObject {
    Q_OBJECT

    class Consumer : public QThread {
        //Q_OBJECT
    public:
        Consumer(DataQueue & queue, Worker * proc) :
            QThread(0),
            m_queue(queue),
            worker(proc)
        {
            if(worker == 0) {
                qDebug() << "Error: worker does not exists";
                exit(1);
            }
        }

        void run() {
            while(true) {
                //qDebug() << "consumer:" << this << "thread:" << this->thread();

                QVariant data(m_queue.remove());
                worker->processData(data);
            }
        }
    private:
        DataQueue & m_queue;
        Worker * worker;
    };

public:

    Producer(Worker * proc, int consumersCount = 2) :
        QObject(0),
        m_worker(proc)
    {
        for(int i = 0; i < consumersCount; ++i) {
            Consumer * consumer =
                    new Consumer(m_queue, m_worker);
            if(consumer != 0) {
                m_consumers.append(consumer);
                consumer->start();
            }
        }

        QThread * thread = new QThread();
        if(thread != 0) {
            moveToThread(thread);
            thread->start();
        }
    }

    ~Producer() {
        foreach (Consumer * consumer, m_consumers) {
            consumer->deleteLater();
        }
    }

public slots:

    void enqueueData(const QVariant data) {
        //qDebug() << "producer thread:" << this->thread();
        m_queue.add(data);
    }

private:
    DataQueue  m_queue;
    Worker *    m_worker;
    QVector<Consumer *> m_consumers;
};

#endif // PRODUCER_H
