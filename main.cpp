#include "dialog.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
//    QApplication::setStyle(QStyleFactory::create("windows"));
//    QApplication::setPalette(QApplication::style()->standardPalette());
    w.show();
    //a.setStyle("macintosh");
    return a.exec();
}
