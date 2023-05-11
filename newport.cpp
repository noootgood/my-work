#include "newport.h"

newPort::newPort(QObject *parent)
    : QObject{parent}
{
    qDebug() << "newport - ";
    QFile* set = new QFile("./setting.txt");
    set -> open(QFile::ReadOnly);
    saveflord = set -> readAll();
    set -> close();
    //saveflord.append("/");
    threadnum = 0;
    tcpser = new QTcpServer(this);
    tcpser ->listen(QHostAddress::Any, 9997);
    connect(tcpser, &QTcpServer::newConnection, this, &newPort::newconnected);
}

void newPort::receivepass2(QString flord)
{
    qDebug() << "newport - receivepass2";
    saveflord = flord;
    qDebug() << saveflord;
    //saveflord.append("/");
}

void newPort::newconnected()
{
    qDebug() << "newport - newconnected";
    qint64 threadid = threadnum;
    threadnum = (threadnum + 1) % threadsize;
    while(tcpcheck[threadid] == 1){
        threadid = threadnum;
        threadnum = (threadnum + 1) % threadsize;
    }
    QtConcurrent::run([=](){
        qDebug() << threadid;
        tcpcheck[threadid] = 1;
        tcpskt[threadid] = tcpser -> nextPendingConnection();
        connect(tcpskt[threadid], &QTcpSocket::readyRead, this, [=](){
            askmessage(threadid);
        });
        connect(tcpskt[threadid], &QTcpSocket::disconnected, this,[=](){
            tcpcheck[threadid] = 0;
            tcpskt[threadid] ->close();
            qDebug() << "free";
        });
    });
}

void newPort::askmessage(qint64 id)
{
    qDebug() << "newport - askmessage";
    QDataStream datain(tcpskt[id]);
    QString ip;
    datain >> ip;
    QString midpath = saveflord;
    midpath.append("/catch/");
    midpath.append(ip);
    QString temppath = midpath;
    QDir* dir = new QDir(temppath);
    QByteArray blocks;
    QDataStream dataout(&blocks, QIODevice::WriteOnly);
    if(dir -> exists()){
        QStringList iplist = dir ->entryList(QDir::Dirs);
        iplist.removeOne(".");
        iplist.removeOne("..");
        for(auto s : iplist) {
            temppath = midpath;
            temppath.append("/");
            temppath.append(s);
            temppath.append("/");
            QDir* dirfile = new QDir(temppath);
            QStringList filelist = dirfile ->entryList(QDir::Files);
            filelist.removeOne(".");
            filelist.removeOne("..");
            for(auto d : filelist){
                QString filez = temppath;
                filez.append("/");
                filez.append(d);
                QFile* filezz = new QFile(filez);
                qint64 sizez = filezz -> size();
                filezz -> deleteLater();
                dataout << s << d << sizez;
            }
        }
    }
    tcpskt[id] -> write(blocks);
    blocks.resize(0);
}
