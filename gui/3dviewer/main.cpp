#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("ScheiseKraft");
    w.setFixedSize(800, 600);
    w.setCursor(Qt::BlankCursor);
    w.show();
    return a.exec();
}
