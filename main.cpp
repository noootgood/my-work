#include "userpage.h"
//#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    userpage x;
    x.show();
    //MainWindow w;
    //w.show();
    return a.exec();
}
