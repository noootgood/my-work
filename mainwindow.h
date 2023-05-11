#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QDataStream>
#include <QDir>
#include <QStringList>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include "setting.h"
#define threadsize 10

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void passflord(QString);

private:
    Ui::MainWindow *ui;

    QTcpServer* tcpser_listenfile;
    QTcpSocket* tcpskt_savefiles[threadsize];

    void starlisten();
    void stoplisten();
    void savefile(qint64 id);
    void makefile(QString name, qint64 id);
    void newconnectiongot();
    void makedir(QString name);
    void tosettingpage();
    void changesavepos();

    qint64 receiveddata;
    qint64 filenamesize;
    qint64 totalsize;
    qint64 threadnum;
    qint64 tcpskt_check[threadsize];

    QFile* file_received[threadsize];

    QByteArray blocks;

    QString filename = "";
    QString targetid = "";
    QString sourceid = "";
    QString saveflord = "";

    setting* w;
};
#endif // MAINWINDOW_H
