/*!
\file
\brief Заголовочный файл с описанием класса

Данный файл содержит в себе определение класса Stream2String
*/

#ifndef STREAMTOSTRING
#define STREAMTOSTRING

#include <QTextStream>

/*!
    \brief Stream2String - класс вывода переменных в строку
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года

    Класс обеспечивает:
    1) добавление переменных в поток
    2) вывод переменных из потока в текстовую строку
*/

class StreamString {
public:

    ////
    /// \brief Stream2String Конструктор по умолчанию
    ///
    StreamString() : m_in(&m_text) {}

    ////
    /// \brief Stream2String Конструктор копирования (заглушка)
    ///
    StreamString(StreamString &) : m_in(&m_text) {}

    ////
    /// \brief operator = (заглушка)
    /// \return
    ///
    StreamString & operator=(const StreamString & ) {
        return *this;
    }

    ////
    /// \brief out - вывод содержимого потока в виде строки
    /// \return
    ///
    QString out() { return m_in.readAll(); }

    ////
    /// \brief in - переменная для ввода данных
    ///
    QTextStream m_in;
private:

    ////
    /// \brief text - текстовая строка для хранения данных
    ///
    QString m_text;
};

#endif // STREAMTOSTRING

