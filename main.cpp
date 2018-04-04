#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QCoreApplication::setApplicationName("nRF52-Flasher");
    QCoreApplication::setOrganizationName("Vega-Absolute");
    QCoreApplication::setOrganizationDomain("Vega-Absolute.ru");
    QCoreApplication::setApplicationVersion("1.0");

    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    return a.exec();
}
