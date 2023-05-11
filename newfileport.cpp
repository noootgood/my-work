#include "newfileport.h"

newfileport::newfileport(QObject *parent)
    : QObject{parent}
{
    qDebug() << "newfileport - ";
    QFile* set = new QFile("./setting.txt");
    set -> open(QFile::ReadOnly);
    saveflord = set -> readAll();
    set -> close();
    //saveflord.append("/");
    threadnum = 0;
    totalsize = 0;
    writensize = 0;
    leftsize = 0;
    blocks.resize(0);
    tcpser = new QTcpServer(this);
    tcpser -> listen(QHostAddress::Any, 9996);
    connect(tcpser, &QTcpServer::newConnection, this, &newfileport::newconnected);
}

void newfileport::receivepass1(QString flord)
{
    qDebug() << "newfileport - receivepass1";
    saveflord = flord;
    qDebug() << saveflord;
    //saveflord.append("/");
}

void newfileport::newconnected()
{
    qDebug() << "newfileport - newconnected";
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
            receiveask(threadid);
        });
        connect(tcpskt[threadid], &QTcpSocket::bytesWritten, this, [=](){
            sendfile(threadid);
        });
        connect(tcpskt[threadid], &QTcpSocket::disconnected, this, [=](){
            tcpcheck[threadid] = 0;
            qDebug() << "free2";
        });
    });
}

void newfileport::receiveask(qint64 id)
{
    qDebug() << "newfileport - receiveask";
    QDataStream datain(tcpskt[id]);
    QString targetip, sourceip, name;
    datain >> targetip >> sourceip >> name;
    QString midpath = saveflord;
    midpath.append("/catch/");
    midpath.append(targetip);
    midpath.append("/");
    midpath.append(sourceip);
    midpath.append("/");
    midpath.append(name);
    file[id] = new QFile(midpath);
    file[id] -> open(QFile::ReadOnly);
    QDataStream dataout(&blocks, QIODevice::WriteOnly);
    qDebug() << name;
    totalsize = file[id] -> size();
    dataout << qint64(0) << qint64(0) << name << sourceip;
    totalsize += blocks.size();
    dataout.device() -> seek(0);
    dataout << totalsize << qint64(blocks.size() - sizeof(qint64) * 2);
    leftsize = totalsize - (tcpskt[id] -> write(blocks));
    writensize = blocks.size();
    blocks.resize(0);
}

void newfileport::sendfile(qint64 id)
{
    qDebug() << "newfileport - sendfile";
    if(leftsize > 0) {
        qint64 minsize = qMin(leftsize, 1024 * 4);
        blocks = file[id] -> read(minsize);
        leftsize -= tcpskt[id] -> write(blocks);
        writensize += blocks.size();
        blocks.resize(0);
    }else{
        totalsize = 0;
        writensize = 0;
        file[id] -> close();
        tcpskt[id] -> close();
    }
    if(totalsize == writensize){
        totalsize = 0;
        writensize = 0;
        file[id] -> close();
        tcpskt[id] -> close();
        QThread::msleep(100);
        file[id] -> remove();
    }
}
