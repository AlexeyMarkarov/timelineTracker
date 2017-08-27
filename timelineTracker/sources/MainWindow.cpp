#include "MainWindow.h"
#include "ChartView.h"
#include "TimelineModel.h"

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
    connect(mWindow, SIGNAL(removeTimeEntry(const int)),
            this, SIGNAL(removeTimeEntry(const int)));
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
