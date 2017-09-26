#include "QmlPixmapProvider.h"
#include <QtWidgets>

QmlPixmapProvider::QmlPixmapProvider()
    : QQuickImageProvider(QQmlImageProviderBase::Pixmap)
{
}

QmlPixmapProvider::~QmlPixmapProvider()
{
}

QPixmap QmlPixmapProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    const QStringList params = QDir::toNativeSeparators(id).split(QDir::separator(), QString::SkipEmptyParts);
    if(params.count() == 2)
    {
        if(params.first().toLower() == QString("stdpixmap"))
        {
            return getStdPixmap(params.last(), size, requestedSize);
        }
    }
    return QQuickImageProvider::requestPixmap(id, size, requestedSize);
}

QPixmap QmlPixmapProvider::getStdPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    // create StandardPixmap name to type map
    static const QHash<QString, QStyle::StandardPixmap> pixmaps = [](){
        QHash<QString, QStyle::StandardPixmap> map;

        const QMetaEnum standardPixmapEnum = QMetaEnum::fromType<QStyle::StandardPixmap>();
        if(standardPixmapEnum.isValid())
        {
            for(int i = 0; i < standardPixmapEnum.keyCount(); ++i)
            {
                map.insert(standardPixmapEnum.key(i), static_cast<QStyle::StandardPixmap>(standardPixmapEnum.value(i)));
            }
        }

        return map;
    }();

    // determine StandardPixmap type
    bool isInt = false;
    QStyle::StandardPixmap pixmapType = static_cast<QStyle::StandardPixmap>(id.toInt(&isInt));
    if(!isInt)
    {
        pixmapType = pixmaps.value(id, QStyle::SP_CustomBase);
    }

    if(pixmapType == QStyle::SP_CustomBase)
    {
        qWarning() << "unknown pixmap type" << id << pixmapType;
    }

    // get the pixmap
    const QPixmap pixmap(qApp->style()->standardPixmap(pixmapType));
    if(size)
        *size = pixmap.size();
    const QPixmap result = requestedSize.isEmpty()
                           ? pixmap
                           : pixmap.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return result;
}
