#ifndef RECEIVEPAGE_H
#define RECEIVEPAGE_H

#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QCloseEvent>
#include <QStringList>
#include <QComboBox>
#include <QThread>
#include <QVector>

namespace Ui {
class receivepage;
}

class receivepage : public QDialog
{
    Q_OBJECT

public:
    explicit receivepage(QWidget *parent = nullptr);
    ~receivepage();

    void changepath();
    void comboboxchanged();
    void askfile();
    void askconnect();
    void getfilelist();
    void finishask();
    void askpassfile();
    void asknextconnect();
    void getfile();
    void selectitemss();
public slots:
    void receivepath(QString data, QString serip, QString userid);
signals:
    void receiveclosed();
private:
    Ui::receivepage *ui;

    QString filepath = "";
    QString filename = "";
    QString fileip = "";
    QString userid;
    QString servershostip = "";//"47.113.147.98";
    QStringList filenamelist;
    QStringList fileidlist;
    QTcpSocket* tcpskt_askback;
    QTcpSocket* tcpskt_askfile;

    qint64 receiveddata = 0;
    qint64 filenamesize = 0;
    qint64 totalsize = 0;
    QFile* rfile;
    QByteArray blocks;
    int currentrowzz = -1;
    QVector<qint64> filesizelist;
    void closeEvent(QCloseEvent* event);
};

#endif // RECEIVEPAGE_H
