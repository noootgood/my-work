#ifndef USERPAGE_H
#define USERPAGE_H

#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>

namespace Ui {
class userpage;
}

class userpage : public QDialog
{
    Q_OBJECT

public:
    explicit userpage(QWidget *parent = nullptr);
    ~userpage();
signals:
    void sendidandserip(QString, QString);
private:
    Ui::userpage *ui;

    QTcpSocket* tcpskt;

    //QString hostip = "";

    void loging();
    void login();
    void getconnnecting(QString id, QString pw);
    void checkconnect();

    QString id;
    QString pw;
    QString ser;
};

#endif // USERPAGE_H
