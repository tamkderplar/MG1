#include "mainwindow.h"
#include <QApplication>

#include <QGLFormat>

int main(int argc, char *argv[])
{
    QGLFormat format;
    format.setVersion(3,3);
    format.setProfile(QGLFormat::CoreProfile);
    format.setSampleBuffers(true);
    QGLFormat::setDefaultFormat(format);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
