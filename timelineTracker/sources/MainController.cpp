#include "MainController.h"
#include "MainWindow.h"

MainController::MainController(QObject *parent)
    : QObject(parent)
{
}

MainController::~MainController()
{
}

bool MainController::init()
{
    mWnd = new MainWindow(this);
    return mWnd->isCreated();
}

void MainController::release()
{
    delete mWnd;
}
