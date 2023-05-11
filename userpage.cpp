#include "userpage.h"
#include "mainwindow.h"
#include "ui_userpage.h"
#include <QThread>

userpage::userpage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::userpage)
{
    ui->setupUi(this);
    connect(ui -> acceptbtn, &QPushButton::clicked, this, &userpage::loging);
    tcpskt = new QTcpSocket(this);
}

userpage::~userpage()
{
    delete ui;
}

void userpage::loging()
{
    qDebug() << "userpage - lgoing";
    id = ui -> userid -> text();
    pw = ui -> userpw -> text();
    ser = ui -> serverip -> text();
    if(id == "" || pw == "" || ser == ""){
        QMessageBox::information(NULL, "错误", "id或密码或服务器IP不能为空");
    }else{
        int pt = 0, flag = 0, len = 0;
        for(int i = 0; i < ser.size(); i ++){
            if(ser.at(i) == '.') {
                pt ++;
                if(len > 3) flag = 1;
                len = 0;
            }
            else len ++;
        }
        if(pt != 3) flag = 1;
        if(flag == 1){
            QMessageBox::information(NULL, "错误", "服务器IP格式错误");
        }else{
            tcpskt -> connectToHost(QHostAddress(ser), 9992);
            ui -> acceptbtn -> setEnabled(false);
            ui -> acceptbtn -> setText("登陆中...");
            QThread::msleep(100);
            connect(tcpskt, &QTcpSocket::connected, this, [=](){
                getconnnecting(id, pw);
            });
            connect(tcpskt, &QTcpSocket::readyRead, this, &userpage::checkconnect);
        }
    }

}

void userpage::login()
{
    qDebug() << "userpage - login";
    MainWindow *w = new MainWindow;
    connect(this, &userpage::sendidandserip, w, &MainWindow::getidandserip);
    emit sendidandserip(id, ser);
    w -> show();
    userpage::close();
}

void userpage::getconnnecting(QString id, QString pw)
{
    qDebug() << "userpage - getconnecting";
    QByteArray blocks;
    QDataStream dataout(&blocks, QIODevice::WriteOnly);
    QString checks = "loging";
    dataout << id << pw << checks;
    tcpskt -> write(blocks);
    blocks.resize(0);
}

void userpage::checkconnect()
{
    qDebug() << "userpage - checkconnect";
    QString back = tcpskt -> readAll();
    if(back == "true"){
        tcpskt -> close();
        login();
    }else{
        QMessageBox::information(NULL, "错误", "账号信息或服务器IP错误");
        ui -> acceptbtn -> setEnabled(true);
        ui -> acceptbtn -> setText("登陆");
        tcpskt -> close();
    }
}
