#include "mini.h"
#include "ui_mini.h"

#include "udpsignal.h"

#include "../Logger/ConsoleAppender.h"

#define PROFILE_ENABLE
#include "Profiler/profiler.h"

#define PROFILE_BEGINNER PROFILE_BEGIN(appID, " "); PROFILE_START(Q_FUNC_INFO);
#define PROFILE_ENDER PROFILE_STOP();

mini::mini(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mini)
{
    PROFILE_BEGINNER;

    ui->setupUi(this);

    QString userName;
#ifdef Q_WS_WIN
    userName = getenv("USERNAME");
#else
    userName = getenv("USER");
#endif
    ui->leName->setText(userName);

    OnlinerInfo myIpPort = m_udpSignal.getMyInfo();
    ui->label_ip->setText(myIpPort.ipAsString());
    ui->label_port->setText(myIpPort.portAsString());

    QObject::connect(&m_udpSignal, SIGNAL(newMember()), this, SLOT(newMember()));
    QObject::connect(&m_tcpServer, SIGNAL(receivedSome()), this, SLOT(receivedSome()));

    PROFILE_ENDER;
}

mini::~mini()
{
    delete ui;
}

void mini::portChanged()
{
    PROFILE_BEGINNER;

    OnlinerInfo myInfo = m_udpSignal.getMyInfo();
    qDebug() << "My new port:" << myInfo.m_port;
    m_tcpServer.setPort(myInfo.m_port);
    ui->label_ip->setText(myInfo.ipAsString());
    ui->label_port->setText(myInfo.portAsString());

    PROFILE_ENDER;
}

//#include <QWebFrame>
void mini::receivedSome()
{
    PROFILE_BEGINNER;

    TcpData received;
    received.m_type = m_tcpServer.getReceived().m_type;
    received.m_value = m_tcpServer.getReceived().m_value;

    if(received.m_type == "string") {
        addMesInPage(received.m_value);
    }

    PROFILE_ENDER;
}

void mini::addMesInPage(const QString & mes) {

    PROFILE_BEGINNER;

    ui->wvChat->append(mes);

    PROFILE_ENDER;
}

QString mini::htmlRemoveHeaders(QString in)
{
    PROFILE_BEGINNER;

    QString out;

    int bodyPos = in.indexOf("<body");
    if(bodyPos != -1) {
        out = in.remove(0, bodyPos);

        int nextTagBegin = out.indexOf("<");
        if(nextTagBegin != -1) {
            out = out.remove(0, nextTagBegin);

            int nextTagEnd = out.indexOf(">");
            if(nextTagEnd != -1)
                out = out.remove(0, nextTagEnd + 1);
       }
    }

    PROFILE_ENDER;

    return out;
}

void mini::on_pbSend_released()
{
    PROFILE_BEGINNER;
    sendMessage();
    PROFILE_ENDER;
}

void mini::newMember()
{
    PROFILE_BEGINNER;

    const QMap <QUuid, OnlinerInfo> onliners(m_udpSignal.getOnliners());
    OnlinerInfo myInfo = m_udpSignal.getMyInfo();

    QMap <QUuid, OnlinerInfo> newMembers;
    QList<int> ports;
    const QList<QUuid> & IDs(onliners.keys());

    foreach(const QUuid & id, IDs) {

        if(id == myInfo.m_id) continue;

        ports << onliners[id].m_port;

        newMembers.insert(id, onliners[id]);
     }

     if(!myInfo.m_port || ports.contains(myInfo.m_port)) {

         for(int i = 8081; i < 8999; ++i)
            if(!ports.contains(i)) {
              myInfo.m_port = i;
              m_udpSignal.setMyInfo(myInfo);
              portChanged();
              break;
            }
     }

     ui->lwOnliners->clear();

     QStringList checked;

     int i = 0;
     foreach(const OnlinerInfo & info, newMembers) {

             QString current = QString::number(info.m_ip)+
                     ":"+QString::number(info.m_port);

             if(!checked.contains(current)) {
                 checked.append(current);

                ui->lwOnliners->addItem(info.m_name + " " +
                                        info.ipAsString() + ":" +
                                        info.portAsString());

                ui->lwOnliners->item(i)->setData(Qt::UserRole, info.m_id.toString());
                ++i;
             }
     }

    PROFILE_ENDER;
}

void mini::on_lwOnliners_itemDoubleClicked(QListWidgetItem *item)
{
    PROFILE_BEGINNER;

    const QMap <QUuid, OnlinerInfo> onliners(m_udpSignal.getOnliners());

    QUuid receiverID(item->data(Qt::UserRole).toString());

    qDebug() << receiverID << onliners[receiverID].m_ip << onliners[receiverID].m_port;

    PROFILE_FLOW_TO("message getter", Q_FUNC_INFO);
    sendMessage(onliners[receiverID].m_ip, onliners[receiverID].m_port);
    PROFILE_ENDER;
}

void mini::sendMessage(const quint32 ip, const quint16 port, const QString inputMessage) {

    PROFILE_BEGINNER;

    TcpData message;
    message.m_type = "string";

    OnlinerInfo myInfo = m_udpSignal.getMyInfo();

    if(inputMessage.isEmpty()) {

            QString page(ui->teMessage->toHtml());
            page = htmlRemoveHeaders(page);

            if(!page.isEmpty()) {
                QString timestamp = QDateTime::currentDateTime().toLocalTime().toString();

                QString mes("<div style=\"color:#ff0000;\"><small>"+timestamp+", from: " + myInfo.m_name +
                            " ("+ myInfo.ipAsString() +":" +myInfo.portAsString() +")");

                QString receiver("all");


                if(ip && port) {
                    if(ui->lwOnliners->currentItem()) {
                        receiver = ui->lwOnliners->currentItem()->text();
                    }
                    mes += " only to me";
                }
                else mes += " to all";


                mes += ":</small></div>" + page;


                addMesInPage("<div style=\"color:#0000ff;\"><small>"+timestamp+", I wrote to "+
                             receiver +":</small></div>"+page);

                message.m_value = qPrintable(mes);
                ui->teMessage->clear();

            }
    }
    else
        message.m_value = qPrintable(inputMessage);

    if(ip && port) m_tcpClient.sendMessage(message, ip, port);
    else {
        const QMap <QUuid, OnlinerInfo> onliners(m_udpSignal.getOnliners());
        foreach(const OnlinerInfo & info, onliners) {
            if(!(info.m_ip == myInfo.m_ip && info.m_port == myInfo.m_port))
                m_tcpClient.sendMessage(message, info.m_ip, info.m_port);
        }
    }

    PROFILE_ENDER;
}

void mini::on_leName_textChanged(const QString & loginName)
{
    PROFILE_BEGINNER;

    OnlinerInfo myInfo = m_udpSignal.getMyInfo();
    myInfo.m_name = loginName;
    m_udpSignal.setMyInfo(myInfo);

    PROFILE_ENDER;
}

void mini::closeEvent(QCloseEvent *event)
{
    PROFILE_BEGINNER
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Mini",
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
        PROFILE_ENDER
    }
    else {
        ON_EXIT();
        event->accept();
    }
}
