#ifndef MINI_H
#define MINI_H

#include <QMainWindow>
#include <QTime>

#include <QListWidgetItem>
#include <QTextCodec>
#include <QCloseEvent>
#include <QMessageBox>

#include "udpsignal.h"
#include "tcpserver.h"
#include "tcpclient.h"

namespace Ui {
class mini;
}

class mini : public QMainWindow
{
    Q_OBJECT

public:
    explicit mini(QWidget *parent = 0);
    ~mini();

private slots:
    void on_lwOnliners_itemDoubleClicked(QListWidgetItem *item);
    void newMember();
    void portChanged();

    void receivedSome();
    void on_pbSend_released();

    void on_leName_textChanged(const QString &arg1);

    void closeEvent (QCloseEvent *event);

signals:
private:
    void setText(const QString &);
    void sendMessage(const quint32 ip = 0, const quint16 = 0, const QString = "");
    QString htmlRemoveHeaders(QString);
    void addMesInPage(const QString &);

    Ui::mini *ui;

    UdpSignal m_udpSignal;
    TcpServer m_tcpServer;
    TcpClient m_tcpClient;

    QStringList m_nickNames;

};

#endif // MINI_H
