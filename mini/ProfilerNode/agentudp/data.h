/*!
\file
\brief Заголовочный файл с описанием класса

Данный файл содержит в себе определение класса Data
*/

#ifndef DATA_H
#define DATA_H

#include <QVariantMap>
#include <QStringList>
#include "streamtostring.h"

/*!
    \brief Data - структура произвольного набора данных, обертка для QVariantMap
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года

    Класс обеспечивает:
    1) импорт данных из QVariant
    2) вывод содержимого в текстовую строку в формате Json
*/

//#include <QDebug>

class Data : public QVariantMap {
public:

    ////
    /// \brief Data - Конструктор по умолчанию
    ///
    Data() {}

    ////
    /// \brief Деструктор класса
    ///
    ~Data() {}

    ////
    /// \brief Data - конструктор класса с конвертером из QVariant
    /// \param data - импортируемые данные
    ///
    Data(const QVariant & data) {
        if(data.isValid())
        if(data.canConvert<QVariantMap>())
            unite(data.toMap());
    }

    ////
    /// \brief Data - конструктор копирования
    /// \param data - импортируемые данные
    ///
    Data(const Data & data) : QVariantMap(data) {}

    ////
    /// \brief toString - вывод данных в текстовую строку в формате Json
    /// \return QString
    ///
    QString toString() const {
        QString result;
        if(!isEmpty()) {
            Data data(*this);
            result = data.toJsonString();
        }
        //qDebug() << keys() << values();
        return result;
    }

protected:

    ////
    /// \brief toJsonString - вывод данных из потока в текстовую строку
    /// \return QString - строка в формате Json
    ///
    QString toJsonString() {
        setStreamString(*this);
        return ss.out();
    }

    ////
    /// \brief setStreamString - парсинг значений и вывод в поток
    /// \param value - значение параметра
    /// \param name - название параметра
    ///
    void setStreamString(const QVariant & value, const QString name = "") {
        if(value.canConvert<QVariantMap>()) {

            if(!name.isEmpty())
                ss.in << "\n\"" << name << "\": {";
            else
                ss.in << "\n{";

            const QVariantMap & data(value.toMap());
            const QList<QString> & keys(data.keys());

            for(int i = 0, max = keys.size() - 1; i < max; ++i) {
                setStreamString(data.value(keys[i]), keys[i]);
                ss.in << ",";
            }
            setStreamString(data.value(keys.back()), keys.back());

            ss.in << "\n}";
        }
        else {
            ss.in << "\n\"" << name << "\": ";

            QString strValue(value.toString());
            if(strValue.isEmpty() && value.canConvert<QStringList>()) {
                QStringList strList(value.toStringList());
                ss.in << "[";
                for(int i = 0, max = strList.size() - 1; i < max; ++i) {
                    ss.in <<  "\"" << strList[i] << "\"" << ",";
                }
                ss.in << "\"" << strList.back() << "\"" << "]";
            }
            else {

                if((strValue == "true") ||
                   (strValue == "false") ||
                   (strValue == "null") ||
                   (QString::number(strValue.toDouble(), 'g', 8) == strValue))
                    ss.in << strValue;
                else
                    ss.in << "\"" << strValue << "\"";
            }
        }
    }

    ////
    /// \brief ss - переменная для ввода данных в поток и вывода в текст
    ///
    StreamString ss;
};


#endif // DATA_H

