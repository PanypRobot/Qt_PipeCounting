#include "loginwindow.h"
#include "countwindow.h"
#include "circlepainter.h"
#include "camerawindow.h"
#include "mycamera.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    LoginWindow w;
//    w.show();

        countwindow w;
        w.show();

//        MyCamera w;
//        w.show();

    return a.exec();
}
