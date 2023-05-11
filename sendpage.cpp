#include "sendpage.h"
#include "ui_sendpage.h"
#include <QThread>

sendwindow::sendwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sendwindow)
{
    ui->setupUi(this);
    setWindowTitle("文件发送窗口");
    ui -> outlinesend -> setEnabled(false);
    connect(ui -> choosebtn, &QPushButton::clicked, this, &sendwindow::getfile);
    //connect(ui -> filesendbtn, &QPushButton::clicked, this, &sendwindow::createconnect);
    connect(ui -> clearbtn, &QPushButton::clicked, this, &sendwindow::clearpath);
    connect(ui -> outlinesend, &QPushButton::clicked, this, &sendwindow::connectserverhost);
    newtcp = new QTcpSocket(this);
    connect(newtcp, &QTcpSocket::connected, this, &sendwindow::sendfilehead);
    connect(newtcp, &QTcpSocket::bytesWritten, this, &sendwindow::sendfile);
}

sendwindow::~sendwindow()
{
    delete ui;
}

void sendwindow::getfile()
{
    qDebug() << "sendpage - getfile";
    filepath = QFileDialog::getOpenFileName(this, tr("选择文件"), "c:\\",
                       tr("所有文件(*.*);;图片文件(*.jpg *.png *.jpeg *.bmp *gif);;压缩文件(*.zip *.rar *.7z);;文本文件(*.txt *.doc *.docx *.pdf *xlsx *.xls);;音频文件(*.wav *.mp3);;视频文件(*.mp4 *.avi *.wmv)"));
    int len = filepath.size();
    int nums = 0;
    for(int i = len - 1; i >= 0; i --) {
        nums ++;
        if(filepath.at(i) == '/'){
            break;
        }
    }
    filename = filepath.right(nums - 1);
    ui -> showfilename -> setText(filename);
    if(filepath != "") {
        ui -> outlinesend -> setEnabled(true);
        ui -> progressBar -> setValue(0);
    }
}

/*void sendwindow::createconnect()
{
    newtcp -> close();
    newtcp -> connectToHost(QHostAddress(tcpip),9999);
    writensize = 0;
}*/

void sendwindow::connectserverhost()
{
    newtcp -> close();
    newtcp -> connectToHost(QHostAddress(servershostip),9998);
    QThread::msleep(500);
}

void sendwindow::clearpath()
{
    qDebug() << "sendpage - clearpath";
    filepath = "";
    filename = "";
    ui -> showfilename -> setText(filename);
    ui -> progressBar -> setValue(0);
}

void sendwindow::sendfilehead()
{
    qDebug() << "sendpage - sendfilehead";
    QThread::msleep(1000);
    sfile = new QFile(filepath);
    if(!sfile -> open(QFile::ReadOnly)){
        QMessageBox::information(this,"error","文件读取失败");
        return;
    }

    totalsize = sfile -> size();
    QDataStream sendf(&blocks, QIODevice::WriteOnly);
    sendf.setVersion(QDataStream::Qt_6_3);
    //头部信息
    //总大小<头部大小<文件名<发送地id<目标id
    sendf << qint64(0) << qint64(0) << filename << userid << targetid;
    sendf.device() -> seek(0);
    totalsize += blocks.size();
    sendf << totalsize << qint64(blocks.size() - sizeof(qint64) * 2);

    targetwritesize = totalsize - newtcp -> write(blocks);
    writensize = blocks.size();
    qDebug() << blocks.size();
    blocks.resize(0);
    ui -> progressBar -> setMaximum(totalsize);
    ui -> progressBar -> setValue(writensize);
}

void sendwindow::sendfile()
{
    qDebug() << "sendpage - sendfile";
    if(targetwritesize > 0) {
        qint64 midsize = qMin(targetwritesize, 4 * 1024);
        blocks = sfile -> read(midsize);
        writensize += midsize;

        targetwritesize -= newtcp -> write(blocks);
        blocks.resize(0);
    }else {
        sfile -> close();
        newtcp -> close();
    }
    ui -> progressBar -> setMaximum(totalsize);
    ui -> progressBar -> setValue(writensize);
    if(writensize == totalsize){
        sfile -> close();
        newtcp -> close();
    }
}

void sendwindow::receiveip(QString targetid, QString userid, QString serverip)
{
    qDebug() << "sendpage - receiveip";
    this -> targetid = targetid;
    this -> userid = userid;
    this -> servershostip = serverip;
    ui -> targetid -> setText(targetid);
    ui -> userid -> setText(userid);
    ui -> serverip -> setText(serverip);
}

void sendwindow::closeEvent(QCloseEvent *event)
{
    emit sendclosesign();
    newtcp -> deleteLater();
    event -> accept();
}


