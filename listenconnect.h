#ifndef LISTENCONNECT_H
#define LISTENCONNECT_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QHostAddress>
#include <QtConcurrent/QtConcurrent>
#include <QFile>
#include <QDir>
#include <QThread>
#define threadsize 10
class listenconnect : public QObject
{
    Q_OBJECT
public:
    explicit listenconnect(QObject *parent = nullptr);
public slots:
    void changepath(QString path);
signals:

private:
    QTcpServer* tcpser;
    QTcpSocket* tcpskt[threadsize];

    qint64 tcpcheck[threadsize];
    qint64 threadnum;

    QFile* tcpfile[threadsize];

    QString path;

    void gotconnect();
    void receivesignal(qint64 id);
};

#endif // LISTENCONNECT_H
