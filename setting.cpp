#include "setting.h"
#include "ui_setting.h"

setting::setting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setting)
{
    ui->setupUi(this);
    qDebug() << "setting - ";
    QFile* file = new QFile("./usersetting.txt");
    if(!file -> exists()){
        file -> open(QFile::WriteOnly);
        file -> write("D:/");
        file -> close();
    }
    file -> open(QFile::ReadOnly);
    QString tmp = file -> readAll();
    file -> close();
    path = tmp;
    ui -> path -> setText(path);
    tmp.append("userflord");
    QDir* dirs = new QDir(tmp);
    if(!dirs -> exists()){
        qDebug() << dirs -> mkdir(tmp);
    }
    model = new QStringListModel(this);
    slist = dirs -> entryList(QDir::Dirs);
    slist.removeOne(".");
    slist.removeOne("..");
    model -> setStringList(slist);
    ui -> userlist -> setModel(model);
    connect(ui -> addbtn, &QPushButton::clicked, this, &setting::addusers);
    connect(ui -> delbtn, &QPushButton::clicked, this, &setting::delusers);
    connect(ui -> changebtn, &QPushButton::clicked, this, &setting::changepath);
}

setting::~setting()
{
    delete ui;
}

void setting::addusers()
{
    qDebug() << "setting - addusers";
    QString id = ui -> adduserid -> text();
    QString pw = ui -> adduserpw -> text();
    if(id == "" || pw == ""){
        QMessageBox::information(this, "错误", "id或密码不能为空");
    }else{
        QString root = path;
        root.append("userflord/");
        root.append(id);
        QDir* dir = new QDir(root);
        if(!dir -> exists()){
            dir -> mkdir(root);
            root.append("/");
            root.append(pw);
            QFile* file = new QFile(root);
            file -> open(QFile::WriteOnly);
            file -> close();
            //qDebug() << slist.size();
            slist.append(id);
            model -> setStringList(slist);
            ui -> userlist -> setModel(model);
            ui -> adduserid -> clear();
            ui -> adduserpw -> clear();
        }else{
            QMessageBox::information(this, "错误", "id已经存在");
        }
    }
}

void setting::delusers()
{
    qDebug() << "setting - delusers";
    QString id = ui -> deluserid -> text();
    if(id == ""){
        QMessageBox::information(this, "错误", "id不能为空");
    }else{
        QString root = path;
        root.append("userflord/");
        root.append(id);
        QDir* dir = new QDir(root);
        if(!dir -> exists()){
            QMessageBox::information(this, "错误", "id不存在");
        }else{
            for(int i = 0; i < slist.size(); i ++){
                if(slist.at(i) == id){
                    slist.removeAt(i);
                    break;
                }
            }
            model -> setStringList(slist);
            ui -> userlist -> setModel(model);
            dir -> removeRecursively();
            ui -> deluserid -> clear();
        }
    }
}

void setting::changepath()
{
    qDebug() << "setting - changepath";
    QString newpath = QFileDialog::getExistingDirectory();
    if(newpath == ""){
        return;
    }
    ui -> path -> setText(newpath);
    QString oldpath = path;
    path = newpath;
    emit flordchange(path);
    QFile* ust = new QFile("./usersetting.txt");
    ust -> open(QFile::WriteOnly);
    ust -> write(path.toUtf8());
    ust -> close();
    oldpath.append("userflord/");
    newpath.append("userflord/");
    QDir* newdir = new QDir(newpath);
    if(!newdir -> exists()){
        newdir -> mkdir(newpath);
    }
    QDir* dir = new QDir(oldpath);
    QStringList dirs = dir -> entryList(QDir::Dirs);
    dirs.removeOne(".");
    dirs.removeOne("..");
    for(auto d : dirs){
        QString temppath = oldpath;
        QString midpath = newpath;
        temppath.append(d);
        midpath.append(d);
        QDir* newfil = new QDir(midpath);
        if(!newfil -> exists()){
            newfil -> mkdir(midpath);
        }
        QDir* fil = new QDir(temppath);
        QStringList files = fil -> entryList(QDir::Files);
        files.removeOne(".");
        files.removeOne("..");
        for(auto f : files){
            QString df = temppath;
            QString newmidpath = midpath;
            newmidpath.append("/");
            newmidpath.append(f);
            df.append("/");
            df.append(f);
            QFile cpfile(df);
            cpfile.copy(df, newmidpath);
            cpfile.remove();
        }
        fil ->rmdir(temppath);
    }
    dir -> rmdir(oldpath);
    //dir -> removeRecursively();
}
