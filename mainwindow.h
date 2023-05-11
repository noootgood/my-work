#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include "sendpage.h"
#include "receivepage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void getipfromedit();
    void clickreceivebtn();
    void clicksavebtn();
    void toreceivepage();
    void checkuserexists();
    void entersendpage();
public slots:
    void sendpageclosed();
    void receivepageclosed();
    void getidandserip(QString id, QString serip);
signals:
    void passtargetip(QString, QString, QString);
    void passsavepath(QString, QString, QString);

private:
    QString hostipv4 = "";
    QString targetid = "";
    QString filename = "";
    QString filepath = "";
    QString userid;
    QString serverip;

    Ui::MainWindow *ui;

    sendwindow* sendpage;
    receivepage* receivepg;
};
#endif // MAINWINDOW_H
