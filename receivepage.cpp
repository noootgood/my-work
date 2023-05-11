#include "receivepage.h"
#include "ui_receivepage.h"

receivepage::receivepage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::receivepage)
{
    ui->setupUi(this);
    setWindowTitle("文件接收窗口");
    tcpskt_askback = new QTcpSocket(this);
    tcpskt_askfile = new QTcpSocket(this);
    filenamesize = 0;
    totalsize = 0;
    ui -> getoutlinefile -> setEnabled(false);
    connect(ui -> outlinefilelist, &QListWidget::itemSelectionChanged, this, &receivepage::selectitemss);
    connect(ui -> changebtn, &QPushButton::clicked, this, &receivepage::changepath);
    connect(ui -> viewoutlinefile, &QPushButton::clicked, this, &receivepage::askconnect);
    connect(ui -> getoutlinefile, &QPushButton::clicked, this, &receivepage::asknextconnect);
}

receivepage::~receivepage()
{
    delete ui;
}

void receivepage::changepath()
{
    qDebug() << "receivepage - changepath";
    QString midpath = QFileDialog::getExistingDirectory();
    if(midpath != ""){
        filepath = midpath;
        ui -> savepath -> setText(filepath);
    }
}

void receivepage::receivepath(QString data, QString serip, QString userid)
{
    qDebug() << "receivepage - receivepath";
    filepath = data;
    ui -> savepath -> setText(data);
    servershostip = serip;
    this -> userid = userid;
}

void receivepage::askfile()
{
    qDebug() << "receivepage - askfile";
    blocks.resize(0);
    QDataStream dataout(&blocks, QIODevice::WriteOnly);
    dataout << userid;
    tcpskt_askback -> write(blocks);
    blocks.resize(0);
}

void receivepage::askconnect()
{
    for(int i = 0; i < ui -> outlinefilelist -> count(); i ++){
        ui -> outlinefilelist -> takeItem(0);
    }
    filenamelist.clear();
    fileidlist.clear();
    filesizelist.clear();
    currentrowzz = -1;
    qDebug() << "receivepage - askconnect";
    tcpskt_askback -> connectToHost(QHostAddress(servershostip), 9997);
    connect(tcpskt_askback, &QTcpSocket::connected, this, &receivepage::askfile);
    connect(tcpskt_askback, &QTcpSocket::readyRead, this, &receivepage::getfilelist);
}

void receivepage::getfilelist()
{
    qDebug() << "receivepage - getfilelist";
    QDataStream datain(tcpskt_askback);
    QString sourceid, filename;
    qint64 sizez;
    while(1){
        datain >> sourceid >> filename >> sizez;
        qDebug() << sourceid << " " << filename;
        if(sourceid == "" && filename == "")break;
        fileidlist.append(sourceid);
        filenamelist.append(filename);
        filesizelist.append(sizez);
        ui -> outlinefilelist -> addItem("文件名:" + filename + "  来自用户:" + sourceid);
    }
    tcpskt_askback -> close();
    //
}

void receivepage::askpassfile()
{
    qDebug() << "receivepage - askpassfile";
    QString file, ip;
    blocks.resize(0);
    QDataStream dataout(&blocks, QIODevice::WriteOnly);
    ip = fileidlist.at(currentrowzz);
    file = filenamelist.at(currentrowzz);
    dataout << userid << ip << file;
    tcpskt_askfile -> write(blocks);
    blocks.resize(0);
    QString mid = filepath;
    mid.append("/");
    mid.append(file);
    rfile = new QFile(mid);
    rfile -> open(QFile::WriteOnly);
}

void receivepage::asknextconnect()
{
    qDebug() << "receivepage - asknextconnet";
    tcpskt_askfile -> connectToHost(QHostAddress(servershostip), 9996);
    connect(tcpskt_askfile, &QTcpSocket::connected, this, &receivepage::askpassfile);
    connect(tcpskt_askfile, &QTcpSocket::readyRead, this, &receivepage::getfile);
    connect(tcpskt_askfile, &QTcpSocket::disconnected, this, &receivepage::finishask);
}

void receivepage::finishask()
{
    ui -> outlinefilelist ->takeItem(currentrowzz);
    filenamelist.removeAt(currentrowzz);
    fileidlist.removeAt(currentrowzz);
    filesizelist.removeAt(currentrowzz);
    currentrowzz = -1;
}

void receivepage::getfile()
{
    qDebug() << "receivepage - getfile";
    QDataStream datain(tcpskt_askfile);
    if(receiveddata <= sizeof(qint64) * 2){
        if((tcpskt_askfile -> bytesAvailable() >= sizeof(qint64) * 2) && (filenamesize == 0)){
            datain >> totalsize >> filenamesize;
            receiveddata += sizeof(qint64) * 2;
        }
        if((tcpskt_askfile -> bytesAvailable() >= filenamesize) && (filenamesize != 0)) {
            datain >> filename >> fileip;
            receiveddata += filenamesize;
        }
    }
    if(receiveddata < totalsize) {
        receiveddata += tcpskt_askfile -> bytesAvailable();
        blocks = tcpskt_askfile -> readAll();
        rfile -> write(blocks);
        blocks.resize(0);
    }
    ui -> saveprogress -> setValue(receiveddata);
    ui -> saveprogress -> setMaximum(totalsize);
    if(receiveddata == totalsize) {
        rfile -> close();
        tcpskt_askfile -> close();
        totalsize = 0;
        receiveddata = 0;
        filenamesize = 0;
    }
}

void receivepage::selectitemss()
{
    ui -> getoutlinefile -> setEnabled(true);
    currentrowzz = ui -> outlinefilelist -> currentRow();
    ui -> filenameview -> setText(filenamelist.at(currentrowzz));
    ui -> useridview -> setText(fileidlist.at(currentrowzz));
    QString dw = "B";
    qint64 cursize = filesizelist.at(currentrowzz);
    qint64 pot = 0;
    if(cursize > 1024) {
        cursize = cursize / 1024;
        pot = cursize % 1024;
        dw = "KB";
    }
    if(cursize > 1024) {
        cursize = cursize / 1024;
        pot = cursize % 1024;
        dw = "MB";
    }
    if(cursize > 1024) {
        cursize = cursize / 1024;
        pot = cursize % 1024;
        dw = "GB";
    }
    QString viewz = QString::number(cursize) + "." + QString::number(pot) + " " + dw;
    ui -> filesizeview -> setText(viewz);
    ui -> saveprogress -> setValue(0);
}

void receivepage::closeEvent(QCloseEvent *event)
{
    emit receiveclosed();
    event -> accept();
}
