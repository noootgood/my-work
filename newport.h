#ifndef NEWPORT_H
#define NEWPORT_H

#include <QObject>
#include <QThread>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtConcurrent/QtConcurrent>
#include <QFile>
#include <QDir>
#include <QStringList>
#define threadsize 10
class newPort : public QObject
{
    Q_OBJECT
public:
    explicit newPort(QObject *parent = nullptr);
public slots:
    void receivepass2(QString flord);
signals:

private:
    QTcpServer* tcpser;
    QTcpSocket* tcpskt[threadsize];

    qint64 tcpcheck[threadsize];
    qint64 threadnum;

    QString saveflord = "";
    QStringList sourceiplist;
    QStringList filenamelist;

    void askmessage(qint64 id);
    void newconnected();
};

#endif // NEWPORT_H
