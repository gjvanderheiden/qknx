#include <QDebug>
#include <QApplication>
#include "mainwindow.h"


int main(int argc, char *argv[]) {
    QApplication app(argc,argv);
    MainWindow win;
    win.show();

    int ret = app.exec();
    
    qDebug() << ret;
    return ret;
}
