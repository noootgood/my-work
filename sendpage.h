#ifndef SENDPAGE_H
#define SENDPAGE_H

#include <QDialog>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include <QDataStream>
#include <QCloseEvent>

namespace Ui {
class sendwindow;
}

class sendwindow : public QDialog
{
    Q_OBJECT

public:
    explicit sendwindow(QWidget *parent = nullptr);
    ~sendwindow();

    void getfile();
    void sendtext();
    void sendfile();
    void sendfilehead();
    void createconnect();
    void clearpath();
    void connectserverhost();

public slots:
    void receiveip(QString targetid, QString userid, QString serverip);

signals:
    void sendclosesign();

private:
    Ui::sendwindow *ui;

    QTcpSocket* newtcp;
    //getip* gethostip;
    QFile* sfile;
    qint64 totalsize;
    QByteArray blocks;
    qint64 targetwritesize;
    qint64 writensize;

    QString filepath = "";
    QString filename = "";
    QString textcontent = "";
    QString targetid;
    QString userid;
    QString servershostip = "";//"47.113.147.98";
    void closeEvent(QCloseEvent* event);
};

#endif // SENDPAGE_H
