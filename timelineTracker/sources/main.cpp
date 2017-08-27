#include <QApplication>
#include "MainController.h"

int main(int argc, char *argv[])
{
    qputenv("QML_DISABLE_DISK_CACHE", QByteArray::number(1));

    QApplication app(argc, argv);

    MainController ctrl;
    const int appResult = ctrl.init() ? app.exec() : -1;
    ctrl.release();

    return appResult;
}
