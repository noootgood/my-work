#ifndef NEWFILEPORT_H
#define NEWFILEPORT_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include <QThread>
#define threadsize 10
class newfileport : public QObject
{
    Q_OBJECT
public:
    explicit newfileport(QObject *parent = nullptr);
public slots:
    void receivepass1(QString flord);
signals:

private:
    QTcpServer* tcpser;
    QTcpSocket* tcpskt[threadsize];

    qint64 threadnum;
    qint64 tcpcheck[threadsize];
    qint64 totalsize;
    qint64 writensize;
    qint64 leftsize;

    QFile* file[threadsize];

    QString saveflord = "";

    QByteArray blocks;

    void newconnected();
    void receiveask(qint64 id);
    void sendfile(qint64 id);
};

#endif // NEWFILEPORT_H
