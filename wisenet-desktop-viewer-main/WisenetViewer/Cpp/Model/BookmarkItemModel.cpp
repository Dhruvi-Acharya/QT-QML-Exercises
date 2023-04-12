#include "BookmarkItemModel.h"

BookmarkItemModel::BookmarkItemModel(QObject *parent) : QObject(parent)
{
    //qDebug() << "BookmarkItemModel()";
    m_utcStartTime = 0;
    m_utcEndTime = 0;
    m_imageWidth = 0;
    m_imageHeight = 0;
}

BookmarkItemModel::BookmarkItemModel(QString bookmarkId, QString bookmarkName, QString sourceId,
                                     QString cameraName, QString description, QString displayTime,
                                     long long utcStartTime, long long utcEndTime, QImage& image) : BookmarkItemModel()
{
    m_bookmarkId = bookmarkId;
    m_bookmarkName = bookmarkName;
    m_sourceId = sourceId;
    m_cameraName = cameraName;
    m_description = description;
    m_displayTime = displayTime;
    m_utcStartTime = utcStartTime;
    m_utcEndTime = utcEndTime;
    m_image = image;
    m_imageWidth = m_image.width();
    m_imageHeight = m_image.height();
}

BookmarkItemModel::~BookmarkItemModel()
{
    qDebug() << "~BookmarkItemModel()";
}
