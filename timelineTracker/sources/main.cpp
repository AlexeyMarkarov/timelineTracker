#include <QApplication>
#include "MainController.h"
#include "Version.h"

int main(int argc, char *argv[])
{
    qputenv("QML_DISABLE_DISK_CACHE", QByteArray::number(1));

    QApplication app(argc, argv);
    app.setOrganizationName("Scopic Software");
    app.setApplicationName("Timeline Tracker");
    app.setApplicationVersion(QString("%1.%2.%3.%4 (%5)%6")
                              .arg(VERSION_MAJOR)
                              .arg(VERSION_MINOR)
                              .arg(VERSION_PATCH)
                              .arg(VERSION_BUILD_NUMBER)
                              .arg(VERSION_COMMIT_HASH)
                              .arg(VERSION_HAS_MODIFICATIONS ? QString("+") : QString()));

    MainController ctrl;
    const int appResult = ctrl.init() ? app.exec() : -1;
    ctrl.release();

    return appResult;
}
