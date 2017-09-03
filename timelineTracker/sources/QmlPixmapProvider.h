#ifndef QMLPIXMAPPROVIDER_H
#define QMLPIXMAPPROVIDER_H

#include <QtCore>
#include <QtQuick>

class QmlPixmapProvider : public QQuickImageProvider
{
public:
    QmlPixmapProvider();
    ~QmlPixmapProvider();

    // QQuickImageProvider interface
    virtual QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    static QPixmap getStdPixmap(const QString &id, QSize *size, const QSize &requestedSize);
};

#endif // QMLPIXMAPPROVIDER_H
