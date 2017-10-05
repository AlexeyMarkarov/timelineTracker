#include "MainWindow.h"
#include "ChartView.h"
#include "TimelineModel.h"
#include "QmlPixmapProvider.h"

MainWindow::MainWindow(QObject *parent)
    : QObject(parent)
    , mChart(new ChartView(this))
{
    createMembers();
    connectSignals();
}

MainWindow::~MainWindow()
{
}

void MainWindow::createMembers()
{
    mEngine.addImageProvider("pixmapProvider", new QmlPixmapProvider);
    mRootComponent = new QQmlComponent(&mEngine);
    mRootComponent->loadUrl(QUrl("qrc:/qml/MainWindow.qml"), QQmlComponent::PreferSynchronous);
    if(QObject *obj = mRootComponent->create())
    {
        mWindow = obj;
        mWindow->setParent(this);
        mChart->setWrappedObject(QQmlProperty::read(mWindow, "chart").value<QObject*>());

        // set default text for intervals view which will change window's minimum size
        QQmlProperty::write(mWindow, "timeModelStdText", TimelineModel::getStdText());
        // and refresh window actual size based on minimum size
        QQmlProperty::write(mWindow, "width", QQmlProperty::read(mWindow, "minimumWidth"));
        QQmlProperty::write(mWindow, "height", QQmlProperty::read(mWindow, "minimumHeight"));
    }
    else
    {
        qDebug() << "mainWindow init fail" << mRootComponent->errors();
        mWindow = new QObject(this);
    }
}

void MainWindow::connectSignals()
{
    connect(mWindow, SIGNAL(addTimeClicked(const QDateTime, const QDateTime)),
            this, SIGNAL(addTimeClicked(const QDateTime, const QDateTime)));
    connect(mWindow, SIGNAL(removeTimeEntryClicked(const int)),
            this, SIGNAL(removeTimeEntryClicked(const int)));
    connect(mWindow, SIGNAL(clearTimeClicked()),
            this, SIGNAL(clearTimeClicked()));
    connect(mWindow, SIGNAL(logsClicked()),
            this, SIGNAL(logsClicked()));
    connect(mWindow, SIGNAL(closeRequested()),
            this, SIGNAL(closing()));
}

bool MainWindow::isCreated() const
{
    return !mRootComponent->isError();
}

void MainWindow::setTimeModel(QAbstractItemModel *model)
{
    QQmlProperty::write(mWindow, "timeModel", QVariant::fromValue(model));
}

void MainWindow::setTotalTimeText(const QString text)
{
    QQmlProperty::write(mWindow, "totalTimeText", text);
}

ChartView &MainWindow::getChartView() const
{
    return *mChart;
}

QWindow::Visibility MainWindow::getVisibility() const
{
    return static_cast<QWindow::Visibility>(QQmlProperty::read(mWindow, "visibility").toInt());
}

void MainWindow::setVisibility(const QWindow::Visibility visibility)
{
    QQmlProperty::write(mWindow, "visibility", visibility);
}

QSize MainWindow::getSize() const
{
    return QSize(QQmlProperty::read(mWindow, "width").toInt(), QQmlProperty::read(mWindow, "height").toInt());
}

void MainWindow::setSize(const QSize &size)
{
    QQmlProperty::write(mWindow, "width", size.width());
    QQmlProperty::write(mWindow, "height", size.height());
}

QSize MainWindow::getMinimumSize() const
{
    return QSize(QQmlProperty::read(mWindow, "minimumWidth").toInt(), QQmlProperty::read(mWindow, "minimumHeight").toInt());
}

void MainWindow::setMinimumSize(const QSize &size)
{
    QQmlProperty::write(mWindow, "minimumWidth", size.width());
    QQmlProperty::write(mWindow, "minimumHeight", size.height());
}

QPoint MainWindow::getPosition() const
{
    return QPoint(QQmlProperty::read(mWindow, "x").toInt(), QQmlProperty::read(mWindow, "y").toInt());
}

void MainWindow::setPosition(const QPoint &pos)
{
    QQmlProperty::write(mWindow, "x", pos.x());
    QQmlProperty::write(mWindow, "y", pos.y());
}

void MainWindow::setFlashHelpIndicator(const bool flash)
{
    QQmlProperty::write(mWindow, "flashHelpIndicator", flash);
}
