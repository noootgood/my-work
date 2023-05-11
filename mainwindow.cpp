#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("文件传输");

    connect(ui -> acceptbtn, &QPushButton::clicked, this, &MainWindow::getipfromedit);
    connect(ui -> savebtn, &QPushButton::clicked, this, &MainWindow::clicksavebtn);
    connect(ui -> tootherpage, &QPushButton::clicked, this, &MainWindow::toreceivepage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getipfromedit()
{
    QString temptargetid = ui -> getipedit -> text();
    if(temptargetid != ""){
        targetid = temptargetid;
        entersendpage();
    }else{
        QMessageBox::information(this, "提示", "目标用户id不能为空");
    }
}


void MainWindow::clicksavebtn()
{
    QString midpath = QFileDialog::getExistingDirectory();
    if(midpath != ""){
        filepath = midpath;
        ui -> saveview -> setText(filepath);
    }

}

void MainWindow::toreceivepage()
{
    if(filepath == ""){
        QMessageBox::information(this, "提示", "请先指定存储目录");
    }else{
        receivepg = new receivepage(this);
        connect(this, &MainWindow::passsavepath, receivepg, &receivepage::receivepath);
        emit passsavepath(filepath, serverip, userid);
        connect(receivepg, SIGNAL(receiveclosed()), this, SLOT(receivepageclosed()));
        ui -> tootherpage -> setEnabled(false);
        receivepg -> show();
    }
}

void MainWindow::entersendpage()
{
    sendpage = new sendwindow(this);
    connect(this,&MainWindow::passtargetip, sendpage, &sendwindow::receiveip);
    emit passtargetip(targetid, userid, serverip);
    connect(sendpage, SIGNAL(sendclosesign()), this, SLOT(sendpageclosed()));
    ui -> acceptbtn -> setEnabled(false);
    sendpage -> show();
}

void MainWindow::sendpageclosed()
{
    ui -> acceptbtn -> setEnabled(true);
    sendpage -> close();
}

void MainWindow::receivepageclosed()
{
    ui ->tootherpage -> setEnabled(true);
    receivepg -> close();
}

void MainWindow::getidandserip(QString id, QString serip)
{
    ui -> userid -> setText(id);
    ui -> serverip -> setText(serip);
    userid = id;
    serverip = serip;
}

