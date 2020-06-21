#include "excelwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ExcelWidget w;
    w.show();

    return a.exec();
}
