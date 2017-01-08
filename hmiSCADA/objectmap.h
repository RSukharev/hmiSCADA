#ifndef OBJECTMAP_H
#define OBJECTMAP_H

#include <QString>
#include <QList>
#include <QMutex>
#include <QDebug>

template<typename T>
class ObjectMap {
public:
    void add(const QVariant & key, T object) {
        m_mutex.lock();

        if(!(m_keys.contains(key) || m_values.contains(object))) {
            m_values << object;
            m_keys << key;
        }
        else {
            qDebug() << "object is already present" << key;
        }

        m_mutex.unlock();
    }

    int size() {
        m_mutex.lock();
        int size = m_values.size();
        m_mutex.unlock();
        return size;
    }

    void clear() {
        m_mutex.lock();
        m_keys.clear();
        m_values.clear();
        m_mutex.unlock();
    }

    T at(int i) {
        m_mutex.lock();
        T result = m_values.at(i);
        m_mutex.unlock();
        return result;
    }

    void remove(const QVariant & key) {
        m_mutex.lock();
        int index = m_keys.indexOf(key);
        if(index != -1) {
            m_keys.removeAll(key);
            m_values.removeAll(m_values[index]);
        }
        m_mutex.unlock();
    }

    void remove(T object) {
        m_mutex.lock();
        int index = m_values.indexOf(object);
        if(index != -1) {
            m_keys.removeAll(m_keys[index]);
            m_values.removeAll(object);
        }
        m_mutex.unlock();
    }

    T getValue(const QVariant & key) {
        m_mutex.lock();
        int index = m_keys.indexOf(key);
        T object = 0;
        if (index != -1) object = m_values[index];
        m_mutex.unlock();
        return object;
    }

    QVariant getKey(T object) {
        m_mutex.lock();
        int index = m_values.indexOf(object);
        QVariant key;
        if(index != -1) key = m_keys[index];
        m_mutex.unlock();
        return key;
    }

    QList<T> getValues() {
        m_mutex.lock();
        QList<T> values(m_values);
        m_mutex.unlock();
        return values;
    }

    QList<QVariant> getKeys() {
        m_mutex.lock();
        QList<QVariant> keys(m_keys);
        m_mutex.unlock();
        return keys;
    }

private:

    QList<QVariant> m_keys;
    QList<T> m_values;

    QMutex m_mutex;
};

#endif // OBJECTMAP_H
