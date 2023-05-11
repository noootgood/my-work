#include "listenconnect.h"

listenconnect::listenconnect(QObject *parent)
    : QObject{parent}
{
    threadnum = 0;
    qDebug() << "listenconnect - ";
    QFile* file = new QFile("./usersetting.txt");
    if(!file -> exists()){
        file -> open(QFile::WriteOnly);
        file -> write("D:/");
        file -> close();
    }
    file -> open(QFile::ReadOnly);
    QString tmp = file -> readAll();
    file -> close();
    path = tmp;
    tcpser = new QTcpServer(this);
    tcpser -> listen(QHostAddress::Any, 9992);
    connect(tcpser, &QTcpServer::newConnection, this, &listenconnect::gotconnect);
}

void listenconnect::changepath(QString path)
{
    qDebug() << "listenconnect - changepath";
    this -> path = path;
}

void listenconnect::gotconnect()
{
    qDebug() << "listenconnect - gotconnect";
    qint64 threadid = threadnum;
    threadnum = (threadnum + 1) % threadsize;
    while(tcpcheck[threadid] == 1){
        threadid = threadnum;
        threadnum = (threadnum + 1) % threadsize;
    }
    QtConcurrent::run([=](){
        qDebug() << threadid;
        tcpskt[threadid] = tcpser -> nextPendingConnection();
        connect(tcpskt[threadid], &QTcpSocket::readyRead, this, [=](){
            receivesignal(threadid);
        });
        connect(tcpskt[threadid], &QTcpSocket::disconnected, this, [=](){
            tcpcheck[threadid] = 0;
        });
    });
}

void listenconnect::receivesignal(qint64 id)
{
    qDebug() << "listenconnect - receivesignal";
    QDataStream datain(tcpskt[id]);
    QString uid, upw, checks;
    datain >> uid >> upw >> checks;
    QString userpath = path;
    userpath.append("/userflord/");
    userpath.append(uid);
    QDir* dir = new QDir(userpath);
    if(!dir -> exists()){
        tcpskt[id] -> write("false");
        //tcpskt[id] -> close();
    }else{
        if(checks == "loging"){
            userpath.append("/");
            userpath.append(upw);
            QFile* files = new QFile(userpath);
            if(!files -> exists()){
                tcpskt[id] -> write("false");
                //tcpskt[id] -> close();
            }else{
                tcpskt[id] -> write("true");
                //tcpskt[id] -> close();
            }
        }else{
            tcpskt[id] -> write("true");
        }
    }
}

