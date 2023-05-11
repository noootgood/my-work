#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newport.h"
#include "newfileport.h"
#include "listenconnect.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tcpser_listenfile = new QTcpServer(this);
    QFile* set = new QFile("./setting.txt");
    if(!set -> exists()){
        set -> open(QFile::WriteOnly);
        set -> write("D:");
        set -> close();
    }
    set -> open(QFile::ReadOnly);
    saveflord = set -> readAll();
    set -> close();
    ui -> closebtn -> setEnabled(false);
    ui -> savepath -> setText(saveflord);

    w = new setting(this);

    receiveddata = 0;
    filenamesize = 0;
    totalsize = 0;
    threadnum = 0;
    blocks.resize(0);

    connect(ui -> startbtn, &QPushButton::clicked, this, &MainWindow::starlisten);
    connect(ui -> closebtn, &QPushButton::clicked, this, &MainWindow::stoplisten);
    connect(ui -> adduserbtn, &QPushButton::clicked, this, &MainWindow::tosettingpage);
    connect(ui -> changepos, &QPushButton::clicked, this, &MainWindow::changesavepos);

    connect(tcpser_listenfile, &QTcpServer::newConnection, this, &MainWindow::newconnectiongot);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::starlisten()
{
    ui -> startbtn -> setEnabled(false);
    ui -> closebtn -> setEnabled(true);
    tcpser_listenfile -> listen(QHostAddress::Any, 9998);

    QThread* sub = new QThread;
    newPort* receiveask = new newPort;
    receiveask -> moveToThread(sub);
    sub->start();
    connect(this, &MainWindow::passflord, receiveask, &newPort::receivepass2);

    QThread* subs = new QThread;
    newfileport* sendfile = new newfileport;
    sendfile -> moveToThread(subs);
    subs -> start();
    connect(this, &MainWindow::passflord, sendfile, &newfileport::receivepass1);

    QThread* subl = new QThread;
    listenconnect* liscon = new listenconnect;
    liscon -> moveToThread(subl);
    subl -> start();
    connect(w, &setting::flordchange, liscon, &listenconnect::changepath);
}

void MainWindow::stoplisten()
{
    ui -> startbtn -> setEnabled(true);
    ui -> closebtn -> setEnabled(false);
    tcpser_listenfile ->close();
}

void MainWindow::newconnectiongot()
{
    qDebug() << "newconnectiongot";
    qint64 threadid = threadnum;
    threadnum = (threadnum + 1) % threadsize;
    while(tcpskt_check[threadid] == 1){
        threadid = threadnum;
        threadnum = (threadnum + 1) % threadsize;
    }
    QtConcurrent::run([=](){
        qDebug() << threadid;
        tcpskt_check[threadid] = 1;
        tcpskt_savefiles[threadid] = tcpser_listenfile -> nextPendingConnection();
        connect(tcpskt_savefiles[threadid], &QTcpSocket::readyRead, this, [=](){
            savefile(threadid);
        });
        connect(tcpskt_savefiles[threadid], &QTcpSocket::disconnected, this, [=](){
            //tcpskt_savefiles[threadid] ->deleteLater();
            tcpskt_check[threadid] = 0;
        });
    });

}

void MainWindow::makedir(QString name)
{
    QDir* dir = new QDir(name);
    if(!dir ->exists()){
        dir -> mkdir(name);
    }
}

void MainWindow::tosettingpage()
{
    w -> exec();
}

void MainWindow::changesavepos()
{
    QString temppath = QFileDialog::getExistingDirectory();
    QString newflord, oldflord;
    if(temppath != ""){
        newflord = temppath;
        oldflord = saveflord;
        saveflord = temppath;
        QFile* filess = new QFile("./setting.txt");
        filess -> open(QFile::WriteOnly);
        filess -> write(temppath.toUtf8());
        filess -> close();
        emit passflord(saveflord);
    }
    ui -> savepath -> setText(saveflord);

    newflord.append("/catch/");
    oldflord.append("/catch/");
    QDir nf(newflord);
    nf.mkdir(newflord);
    QDir* of1 = new QDir(oldflord);
    QStringList list1 = of1 -> entryList(QDir::Dirs);
    list1.removeOne(".");
    list1.removeOne("..");
    for(auto d1 : list1){
        QString newf1 = newflord;
        newf1.append(d1);
        QDir nf(newf1);
        nf.mkdir(newf1);
        QString oldf1 = oldflord;
        oldf1.append(d1);
        QDir* of2 = new QDir(oldf1);
        QStringList list2 = of2 -> entryList(QDir::Dirs);
        list2.removeOne(".");
        list2.removeOne("..");
        for(auto d2 : list2){
            QString newf2 = newf1;
            newf2.append("/");
            newf2.append(d2);
            QDir nf(newf2);
            nf.mkdir(newf2);
            QString oldf2 = oldf1;
            oldf2.append("/");
            oldf2.append(d2);
            QDir* of3 = new QDir(oldf2);
            QStringList list3 = of3 -> entryList(QDir::Files);
            list3.removeOne(".");
            list3.removeOne("..");
            for(auto f : list3){
                QString newf3 = newf2;
                newf3.append("/");
                newf3.append(f);
                QString oldf3 = oldf2;
                oldf3.append("/");
                oldf3.append(f);
                qDebug() << newf3;
                qDebug() << oldf3;
                QFile* cpfl = new QFile(oldf3);
                cpfl -> copy(oldf3, newf3);
                cpfl -> remove();
            }
            of3 -> rmdir(oldf2);
        }
        of2 -> rmdir(oldf1);
    }
    of1 -> rmdir(oldflord);
    //of1 -> removeRecursively();
}


void MainWindow::savefile(qint64 tcpsktid)
{
    qDebug() << "savefile";
    QDataStream datain(tcpskt_savefiles[tcpsktid]);
    if(receiveddata <= qint64(sizeof(qint64) * 2)){
        if(tcpskt_savefiles[tcpsktid] -> bytesAvailable() >= qint64(sizeof(qint64) * 2) && (filenamesize == 0)){
            datain >> totalsize >> filenamesize;
            receiveddata += sizeof(qint64) * 2;
        }
        if((tcpskt_savefiles[tcpsktid] -> bytesAvailable() >= filenamesize) && (filenamesize != 0)){
            datain >> filename >> sourceid >> targetid;
            receiveddata += filenamesize;
            QString midpath = saveflord;
            midpath.append("/catch/");
            makedir(midpath);
            midpath.append(targetid);
            makedir(midpath);
            midpath.append("/");
            midpath.append(sourceid);
            makedir(midpath);
            midpath.append("/");
            midpath.append(filename);
            makefile(midpath, tcpsktid);
        }
    }
    if(receiveddata < totalsize) {
        receiveddata += tcpskt_savefiles[tcpsktid] -> bytesAvailable();
        blocks = tcpskt_savefiles[tcpsktid] -> readAll();
        file_received[tcpsktid] -> write(blocks);
        blocks.resize(0);
    }
    if(receiveddata == totalsize) {
        receiveddata = 0;
        filenamesize = 0;
        totalsize = 0;
        file_received[tcpsktid] -> close();
    }
}

void MainWindow::makefile(QString name, qint64 id)
{
    file_received[id] = new QFile(name);
    file_received[id] ->open(QFile::WriteOnly);
}




