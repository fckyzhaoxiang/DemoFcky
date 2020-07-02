#include "TimeDisplay.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtTimeDisplay w;
    w.show();

    return a.exec();
}
