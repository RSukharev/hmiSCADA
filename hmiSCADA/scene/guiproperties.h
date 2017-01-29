/*!
\file
\brief Заголовочный файл с описаниемя классов

Данный файл содержит в себе определения классов настройки графического интерфейса управления
*/

#ifndef GUIPROPERTIES_H
#define GUIPROPERTIES_H

#include <QVariant>
#include <QVariantMap>
#include "../data.h"

/*!
    \brief Класс - обертка класса Data, хранилище настроек графического интерфейса управления
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года

    Класс обеспечивает:
    1) формирование данных для отправки в формате Data
    2) извлечение данных из формата Data
*/

class GuiProperties : public Data
{
public:

    ////
    /// \brief GuiProperties - конструктор по умолчанию
    ///
    GuiProperties() {
        insert("name", "GUIproperties");
    }

    ////
    /// \brief GuiProperties - конструктор копирования
    /// \param data - параметры настройки GUI в формате Data
    ///
    GuiProperties(const Data & data) : Data(data) {
        insert("name", "GUIproperties");
    }

    ////
    /// \brief add - добавление параметров GUI в формате Data
    /// \param properties - параметры GUI
    ///
    void add(const Data & properties) {
        foreach(const QString & key, properties.keys()) {
            insert(key, properties.value(key));
        }
    }
};

/*!
    \brief GUI_VisualiseProperties - Структура общих настроек визуализации
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года
*/

struct GUI_VisualiseProperties {
    GUI_VisualiseProperties() :       
        fadeEffectForLinks(false),
        deleteAfterFading(false),
        animation(false),
        id("GUI_VisualiseProperties")
    {}

    ////
    /// \brief fromData - импорт настроек из входного формата Data
    /// \param guiPropertiesData - параметры в формате Data
    ///
    void fromData(const Data guiPropertiesData) {

        const QVariant & vVPData(guiPropertiesData.value(id));

        if(!vVPData.isNull() &&
            vVPData.canConvert<QVariantMap>()) {

            const QVariantMap & vpData(vVPData.toMap());

            animation = vpData["animation"].toBool();
            deleteAfterFading = vpData["deleteAfterFading"].toBool();
            fadeEffectForLinks = vpData["fadeEffectForLinks"].toBool();
        }
    }

    ////
    /// \brief toData - экспорт настроек в формат Data
    /// \return Data - параметры в формате Data
    ///
    Data toData() const {
        Data data;
        Data vpData;
        vpData["animation"] = animation;
        vpData["deleteAfterFading"] = deleteAfterFading;
        vpData["fadeEffectForLinks"] = fadeEffectForLinks;
        data.insert(id, vpData);
        return data;
    }

    ////
    /// \brief fadeEffectForLinks - эффект "затухания" - уменьшения толщины линка
    ///
    bool fadeEffectForLinks;

    ////
    /// \brief deleteAfterFading - удаление линка после "затухания"
    ///
    bool deleteAfterFading;

    ////
    /// \brief animation - анимация линков
    ///
    bool animation;

private:
    ////
    /// \brief id - идентификатор структуры данных GUI
    ///
    QString id;
};

/*!
    \brief GUI_ApplicationProperties - Структура настройки главного виджета приложения
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года
*/

struct GUI_ApplicationProperties {

    ////
    /// \brief The State enum - состояния окна: свернуто, нормально, развернуто
    ///
    enum State { minimize = 0,   ///< окно свернуто
                         normal = 1,     ///< обычное окно
                         maximize = 2  ///< окно развернуто
               };

    ////
    /// \brief GUI_ApplicationProperties - конструктор класса
    ///
    GUI_ApplicationProperties() :
        state(normal),
        id("GUI_ApplicationProperties")
    {}

    ////
    /// \brief fromData - импорт настроек из входного формата Data
    /// \param aplicationPropertiesData - параметры в формате Data
    ///
    void fromData(const Data aplicationPropertiesData) {

        const QVariant & vAPData(aplicationPropertiesData.value(id));

        if(!vAPData.isNull() &&
            vAPData.canConvert<QVariantMap>()) {

            const QVariantMap & apData(vAPData.toMap());

            state = GUI_ApplicationProperties::State(apData["state"].toInt());
        }
    }

    ////
    /// \brief toData - экспорт настроек в формат Data
    /// \return Data - параметры в формате Data
    ///
    Data toData() const {
        Data data;
        Data apData;
        apData["state"] = state;
        data.insert(id, apData);
        return data;
    }

    ////
    /// \brief state - состояние окна: свернуто, нормально, развернуто
    ///
    State state;

private:
    ////
    /// \brief id - идентификатор структуры данных GUI
    ///
    QString id;
};

/*!
    \brief GUI_SplitterProperties - Структура настройки сплиттера главного окна
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года
*/

struct GUI_SplitterProperties {

    ////
    /// \brief GUI_SplitterProperties - Конструктор класса
    ///
    GUI_SplitterProperties() :

        // высота центрального экрана
        mainScreenHeight(0),
        // высота нижней панели
        bottomPanelHeight(0),
        id("GUI_SplitterProperties")
    {}

    ////
    /// \brief fromData - импорт настроек из входного формата Data
    /// \param splitterPropertiesData - параметры в формате Data
    ///
    void fromData(const Data splitterPropertiesData) {

        const QVariant & vSPData(splitterPropertiesData.value(id));

        if(!vSPData.isNull() &&
            vSPData.canConvert<QVariantMap>()) {

            const QVariantMap & spData(vSPData.toMap());

            bottomPanelHeight = spData["bottomPanelHeight"].toInt();
            mainScreenHeight = spData["mainScreenHeight"].toInt();
        }
    }

    ////
    /// \brief toData - экспорт настроек в формат Data
    /// \return Data - параметры в формате Data
    ///
    Data toData() const {
        Data data;
        Data spData;
        spData["bottomPanelHeight"] = bottomPanelHeight;
        spData["mainScreenHeight"] = mainScreenHeight;
        data.insert(id, spData);
        return data;
    }

    ////
    /// \brief mainScreenHeight - высота главного окна
    ///
    int mainScreenHeight;

    ////
    /// \brief bottomPanelHeight - высота панели вывода логов
    ///
    int bottomPanelHeight;

private:
    ////
    /// \brief id - идентификатор структуры данных GUI
    ///
    QString id;
};

/*!
    \brief GUI_LinkProperties - Структура настройки конкретного экземпляра линка
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года
*/

struct GUI_LinkProperties {
    GUI_LinkProperties() :
        linkType(""),
        lineColor(0),
        lineWidth(0),
        id("GUI_LinkProperties")
    {}

    ////
    /// \brief fromData - импорт настроек из входного формата Data
    /// \param linkPropertiesData - параметры в формате Data
    /// \param _linkType - тип линка
    ///
    void fromData(const Data linkPropertiesData, const QString _linkType) {

        const QVariant & vListData(linkPropertiesData.value(id));

        if(!vListData.isNull() &&
            vListData.canConvert<QVariantMap>()) {

            const QVariantMap & linkListData(vListData.toMap());

            const QVariant & vLinkData(linkListData.value(_linkType));
            if(!vLinkData.isNull() &&
                vLinkData.canConvert<QVariantMap>()) {

                const QVariantMap & linkData(vLinkData.toMap());

                linkType = _linkType;
                lineColor = linkData["lineColor"].toInt();
                lineWidth =linkData["lineWidth"].toDouble();
            }
        }
    }

    ////
    /// \brief toData - экспорт настроек в формат Data
    /// \return Data - параметры в формате Data
    ///
    Data toData() const {

        Data data;
        Data lpListData;

        Data lpData;

        lpData["lineColor"] = lineColor;
        lpData["lineWidth"] = lineWidth;
        lpListData.insert(linkType, lpData);

        data.insert(id, lpListData);
        return data;
    }

    ////
    /// \brief linkType - тип линка
    ///
    QString linkType;

    ////
    /// \brief lineColor - цвет линка
    ///
    int lineColor;

    ////
    /// \brief lineWidth - толщина линии
    ///
    double lineWidth;

private:
    ////
    /// \brief id - идентификатор структуры данных GUI
    ///
    QString id;
};

struct GUI_ElementList : public Data {
    void add(Data data) {
        insert(QString::number(keys().size()), data);
    }
};

/*!
    \brief GUI_ElementProperties - Структура для построения графического элемента интерфейса
    \author Roman Sukharev
    \version 1.0
    \date Май 2016 года
*/

struct GUI_ElementProperties {

    ////
    /// \brief widgetType - тип виджета
    ///
    QString widgetType;

    ////
    /// \brief variableName - название переменной
    ///
    QString variableName;

    ////
    /// \brief returnType - тип возвращаемого значения
    ///
    QString returnType;

    ////
    /// \brief value - данные
    ///
    QVariant value;

    ////
    /// \brief fromData - импорт из входного формата Data
    /// \param splitterPropertiesData - параметры в формате Data
    ///
    void fromData(const Data data) {
            variableName = data.value("variable").toString();
            returnType = data.value("returnType").toString();
            widgetType = data.value("widgetType").toString();
            value = data.value("value");
    }

    ////
    /// \brief toData - экспорт в формат Data
    /// \return Data - параметры в формате Data
    ///
    Data toData() const {
        Data data;
        data["variable"] = variableName;
        data["returnType"] = returnType;
        data["widgetType"] = widgetType;
        data["value"] = value;
        return data;
    }
};

#endif // GUIPROPERTIES_H
