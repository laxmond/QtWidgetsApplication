#include "QtServerApplication.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtServerApplication w;
    w.show();
    return a.exec();
}
