#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

#include "ui_mainwidget.h"

namespace Ui {
class MainWidget;
}

#include <QGraphicsScene>
#include <QBasicTimer>

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);
    QString getCurrentModel();
    ~MainWidget();
    QGraphicsScene * getScene() { return m_scene; }

signals:
    void runModel(const QString modelName);
    void changeModel(const QString modelName);
    void clearScene();
    void closeAllNodes();
    void closeGUI();

public slots:
    void onAddElementGUI(QWidget *widget);
    void onAddModelName(const QString &name);
    void onSetSplitterSizes(const QList<int> sizes);
    void onSetLogMessages(const QString &mes);
    void onSetNodesCount(int count);

private slots:
    ////
    /// \brief on_pbRun_released - слот обработки запуска плагина
    ///
    void on_pbRun_released();

    ////
    /// \brief on_comboBox_currentIndexChanged - слот обработки выбора плагина
    ///
    void on_comboBox_currentIndexChanged(const QString &);

    ////
    /// \brief on_pbClear_released  - слот обработки очистки сцены
    ///
    void on_pbClear_released();

    void on_pushButton_released();

    void closeEvent(QCloseEvent *event);

private:
    ////
    /// \brief ui - GUI, сгенерированый Creator-ом
    ///
    Ui::MainWidget * ui;

    QGraphicsScene * m_scene;

    ////
    /// \brief applicationWidget - виджет главного окна программы
    ///
    QWidget * m_applicationWidget;
};

#endif // MAINWIDGET_H
