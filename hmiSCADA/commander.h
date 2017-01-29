#ifndef COMMANDER_H
#define COMMANDER_H

#include <QStringList>
#include <QVariantMap>
#include <QDebug>

/*
    template <typename ObjectType, typename ParamType> class Commander
    класс для хранения и удаленного вызова функций по ключу

    ObjectType - тип класса, указатели на функции-члены которого
    хранятся в QMap<QString, Function> functions;

    где QVariant - ключ для вызова функции
    Function - структура, содержащая:
        1) указатель на функцию
            typedef  void (ObjectType::*MemberFunction)(ParamType parameters);
        2) необязательное описание параметров функции
            QVariant info;

    Пример использования:

    class SomeClass() {

        // функции - члены класса
        void function1(ParamType params) { ... }
        void function2(ParamType params) { ... }

        // хранилище
        Commander<SomeClass, ParamType> commander;

        // регистрация функций
        SomeClass()
        {
            commander.registerCommand("command4function1", &SomeClass::function1);
            commander.registerCommand("command4function2", &SomeClass::function2);
        }

        // вызов функций
        bool runCommand(QVariant command, ParamType params)
        {
            return commander.run(this, command, params);
        }
    }

*/

// макрос для запуска функции-члена класса на выполнение
#define CALL_MEMBER_FN(ptrToObject,ptrToMember)  ((ptrToObject)->*(ptrToMember))

template <typename ObjectType, typename ParamType>
class Commander {

public:
    // тип указателя на функцию, метод класса
    typedef  void (ObjectType::*MemberFunction)(ParamType parameters);

    /* MemberFunction function - указатель на функцию, метод класса
        QVariant info - необязательное описание параметров функции
        пример описания параметров:
        QVariantMap parameters;
        parameters[0]["name"] = "имя 1 параметра";
        parameters[0]["type"] = "int";
        parameters[1]["name"] = "имя 2 параметра";
        parameters[1]["type"] = "QString";
        info = parameters;
    */
    struct Function {
        MemberFunction function;
        QVariant info;
        Function() : function(0) {}
    };

    // присвоение идентификатора класса
    void setClassID(const QVariant id) {
        m_classID = id;
    }

    // регистрация команды
    void registerCommand(const QString command,
                            MemberFunction function,
                            QVariant info = 0) {
        Function newFunction;
        newFunction.function = function;
        newFunction.info = info;
        m_functions.insert(command, newFunction);
    }

    // получение списка команд
    QStringList getCommands() const {
        return m_functions.keys();
    }

    // запуск функции по команде
    bool run(ObjectType * object, const QVariant & command,  ParamType param) {

        MemberFunction function =
                m_functions[command.toString()].function;

        if(function != 0)
            CALL_MEMBER_FN(object, function)(param);
        else
            qDebug() << m_classID << " has no command:" << command;

        return static_cast<bool>(function);
    }

    // получение информации о функции по названию команды
    QVariant & getFunctionInfo(const QVariant & command) {
        return m_functions[command].info;
    }

private:
    // команды и функции
    QMap<QString, Function> m_functions;

    // идентификатор класса
    QVariant m_classID;
};

#endif // COMMANDER_H
