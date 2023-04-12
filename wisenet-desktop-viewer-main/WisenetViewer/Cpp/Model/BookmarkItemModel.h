#pragma once

#include <QObject>
#include <QDebug>
#include <QImage>

class BookmarkItemModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString bookmarkId READ bookmarkId WRITE setBookmarkId NOTIFY bookmarkIdChanged)
    Q_PROPERTY(QString bookmarkName READ bookmarkName WRITE setBookmarkName NOTIFY bookmarkNameChanged)
    Q_PROPERTY(QString sourceId READ sourceId WRITE setSourceId NOTIFY sourceIdChanged)
    Q_PROPERTY(QString cameraName READ cameraName WRITE setCameraName NOTIFY cameraNameChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString displayTime READ displayTime WRITE setDisplayTime NOTIFY displayTimeChanged)
    Q_PROPERTY(long long utcStartTime READ utcStartTime WRITE setUtcStartTime NOTIFY utcStartTimeChanged)
    Q_PROPERTY(long long utcEndTime READ utcEndTime WRITE setUtcEndTime NOTIFY utcEndTimeChanged)
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(int imageWidth READ imageWidth WRITE setImageWidth NOTIFY imageWidthChanged)
    Q_PROPERTY(int imageHeight READ imageHeight WRITE setImageWidth NOTIFY imageHeightChanged)

public:
    explicit BookmarkItemModel(QObject *parent = nullptr);
    BookmarkItemModel(QString bookmarkId, QString bookmarkName, QString sourceId,
                      QString cameraName, QString description, QString displayTime,
                      long long utcStartTime, long long utcEndTime, QImage& image);

    ~BookmarkItemModel();

    QString bookmarkId() { return m_bookmarkId; }
    QString bookmarkName() { return m_bookmarkName; }
    QString sourceId() { return m_sourceId; }
    QString cameraName() { return m_cameraName; }
    QString description() { return m_description; }
    QString displayTime() { return m_displayTime; }
    long long utcStartTime() { return m_utcStartTime; }
    long long utcEndTime() { return m_utcEndTime; }
    QImage image() { return m_image; }
    int imageWidth() { return m_imageWidth; }
    int imageHeight() { return m_imageHeight; }

    void setBookmarkId(QString bookmarkId)
    {
        m_bookmarkId = bookmarkId;
        emit bookmarkIdChanged(bookmarkId);
    }

    void setBookmarkName(QString bookmarkName)
    {
        m_bookmarkName = bookmarkName;
        emit bookmarkNameChanged(bookmarkName);
    }

    void setSourceId(QString sourceId)
    {
        m_sourceId = sourceId;
        emit sourceIdChanged(sourceId);
    }

    void setCameraName(QString cameraName)
    {
        m_cameraName = cameraName;
        emit cameraNameChanged(cameraName);
    }

    void setDescription(QString description)
    {
        m_description = description;
        emit descriptionChanged(description);
    }

    void setDisplayTime(QString displayTime)
    {
        m_displayTime = displayTime;
        emit displayTimeChanged(displayTime);
    }

    void setUtcStartTime(long long utcStartTime)
    {
        m_utcStartTime = utcStartTime;
        emit utcStartTimeChanged(utcStartTime);
    }

    void setUtcEndTime(long long utcEndTime)
    {
        m_utcEndTime = utcEndTime;
        emit utcEndTimeChanged(utcEndTime);
    }

    void setImage(const QImage& image)
    {
        m_image = image;
        emit imageChanged();
    }

    void setImageWidth(const int imageWidth)
    {
        m_imageWidth = imageWidth;
        emit imageWidthChanged();
    }

    void setImageHeight(const int imageHeight)
    {
        m_imageHeight = imageHeight;
        emit imageHeightChanged();
    }

signals:
    void bookmarkIdChanged(QString bookmarkId);
    void bookmarkNameChanged(QString bookmarkName);
    void sourceIdChanged(QString sourcdId);
    void cameraNameChanged(QString cameraName);
    void descriptionChanged(QString description);
    void displayTimeChanged(QString startTime);
    void utcStartTimeChanged(long long utcStartTime);
    void utcEndTimeChanged(long long utcEndTime);
    void imageChanged();
    void imageWidthChanged();
    void imageHeightChanged();

private:
    QString m_bookmarkId;
    QString m_bookmarkName;
    QString m_sourceId;
    QString m_cameraName;
    QString m_description;
    QString m_displayTime;
    long long m_utcStartTime;
    long long m_utcEndTime;
    QImage m_image;
    int m_imageWidth;
    int m_imageHeight;
};
