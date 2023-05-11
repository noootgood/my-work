#ifndef SETTING_H
#define SETTING_H

#include <QDialog>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QStringListModel>
#include <QFileDialog>

namespace Ui {
class setting;
}

class setting : public QDialog
{
    Q_OBJECT

public:
    explicit setting(QWidget *parent = nullptr);
    ~setting();
signals:
    void flordchange(QString);
private:
    Ui::setting *ui;

    void addusers();
    void delusers();
    void changepath();

    QStringListModel* model;
    QStringList slist;
    QString path;
};

#endif // SETTING_H
