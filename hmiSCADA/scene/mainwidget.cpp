#define QT_NO_OPENGL

#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#endif

#include <QGraphicsView>
#include <QScrollBar>
#include <QGraphicsView>
#include <QCloseEvent>

//#include "Logger/ConsoleAppender.h"

#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    m_applicationWidget = ui->graphicsView->parentWidget();
    m_applicationWidget->setWindowTitle("hmiSCADA");

    m_scene = new QGraphicsScene(this);   // Инициализируем графическую сцену
    m_scene->setItemIndexMethod(QGraphicsScene::NoIndex); // настраиваем индексацию элементов
    //scene->setSceneRect( -800, -800, 1600, 1600 );
    //scene->setBackgroundBrush( QColor(100, 149, 237) );
    ui->graphicsView->setScene(m_scene);  // Устанавливаем графическую сцену в graphicsView

    #ifndef QT_NO_OPENGL
         ui->graphicsView->setViewport(new QGLWidget(//QGLFormat(QGL::SampleBuffers)));
                    QGLFormat( QGL::SampleBuffers | QGL::AlphaChannel | QGL::Rgba )));
    #endif

    // Настраиваем рендер

    ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);
    ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    ui->graphicsView->setCacheMode(QGraphicsView::CacheBackground); // Кэш фона
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    ui->graphicsView->centerOn(0, 0);

    ui->graphicsView->setOptimizationFlag(QGraphicsView::DontClipPainter);
    ui->graphicsView->setOptimizationFlag(QGraphicsView::DontSavePainterState);
    ui->graphicsView->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);

    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn) ;
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //ui->graphicsView->scale(0.3,0.3);

    //ui->cbNodeTemplates->addItems(PixmapFabric::nodeTemplateNames());
    ui->graphicsView->setFocus();

    initLogTable();
}

void MainWidget::setModelOptionsHidden(bool state) {
    ui->modelOptions->setHidden(state);
}

void MainWidget::initLogTable() {

    // Create the data model
    model = new QSqlRelationalTableModel(ui->logTable);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("logs");

    // Set the localized header captions
    model->setHeaderData(0, Qt::Horizontal, tr("N"));
    model->setHeaderData(1, Qt::Horizontal, tr("Log datetime"));
    model->setHeaderData(2, Qt::Horizontal, tr("Info"));
    model->setSort(0, Qt::DescendingOrder);

    // Set the model and hide the ID column
    ui->logTable->setModel(model);
    //ui->logTable->hideColumn(0);

    ui->logTable->setColumnWidth(0, 50);
    ui->logTable->setColumnWidth(1, 180);
    ui->logTable->verticalHeader()->hide();
    ui->logTable->horizontalHeader()->setStretchLastSection(true);
}

void MainWidget::closeEvent(QCloseEvent *event)
  {
        emit closeGUI();
        event->accept();
  }

MainWidget::~MainWidget() {
    delete ui;
}

QString MainWidget::getCurrentModel() {
    return ui->comboBox->currentText();
}

void MainWidget::onAddElementGUI(QWidget * widget)
{
    qDebug() << "add element gui";
    if(widget) {
        if(ui->modelOptions->isHidden())
            ui->modelOptions->setHidden(false);

        ui->modelOptions->layout()->addWidget(widget);
        widget->show();
    }
}

void MainWidget::onAddModelName(const QString &name)
{
    ui->comboBox->addItem(name);
}

void MainWidget::onSetLogMessages(const QString message)
{
    m_db.addLog(QDateTime::currentDateTime(), QString(message).replace("\n", "  "));
    model->submitAll();
}

void MainWidget::onSetNodesCount(int count)
{
    ui->lineEdit->setText(QString::number(count));
}

void MainWidget::onSetWindowState(Qt::WindowState state)
{
    setWindowState(state);
}

void MainWidget::onSetSplitterSizes(const QList<int> sizes)
{
    ui->splitter->setSizes(sizes);
}

void MainWidget::on_pbRun_released()
{
    QString modelName(ui->comboBox->currentText());
    emit runModel(modelName);
}

void MainWidget::on_comboBox_currentIndexChanged(const QString & modelName)
{
    emit changeModel(modelName);
}

void MainWidget::on_pbClear_released()
{
    emit clearScene();
}


void MainWidget::on_pushButton_released()
{
    emit closeAllNodes();
}
