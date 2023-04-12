/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd. All rights reserved.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Vision.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Vision. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Vision.
 *
 * Hanwha Vision reserves the right to modify this software without notice.
 *
 * Hanwha Vision Co., Ltd.
 * KOREA
 * https://www.hanwhavision.com/
 *********************************************************************************/

#include "BookmarkModel.h"
#include "LogSettings.h"

BookmarkModel::BookmarkModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleNames[IdRole] = "id";
    m_roleNames[NameRole] = "name";
    m_roleNames[DescriptionRole] = "description";
    m_roleNames[TagsRole] = "tags";
    m_roleNames[StartTimeRole] = "startTime";
    m_roleNames[EndTimeRole] = "endTime";
    m_roleNames[SelectedRole] = "selected";

#ifndef MEDIA_FILE_ONLY
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered, this, &BookmarkModel::onCoreServiceEventTriggered, Qt::QueuedConnection);
#endif
}

void BookmarkModel::onCoreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;
    if (serviceEvent->EventTypeId() == Wisenet::Core::SaveBookmarkEventType)
    {
        auto bookmarkEvent = std::static_pointer_cast<Wisenet::Core::SaveBookmarkEvent>(event->eventDataPtr);
        if(bookmarkEvent->bookmark.deviceID == m_selectedDeviceID && bookmarkEvent->bookmark.channelID == m_selectedChannelID)
        {
            auto idIter = m_idMap.find(bookmarkEvent->bookmark.bookmarkID);
            if(idIter != m_idMap.end())
            {
                m_bookmarks[idIter->second] = bookmarkEvent->bookmark;
                auto indexIter = std::find(m_indexVector.begin(), m_indexVector.end(), idIter->second);
                if(indexIter != m_indexVector.end())
                {
                    int row = std::distance(m_indexVector.begin(), indexIter);
                    QVector<int> rols {NameRole, DescriptionRole, TagsRole};
                    emit dataChanged(index(row,0,QModelIndex()), index(row,0,QModelIndex()), rols);
                }
            }
            else
            {
                append(bookmarkEvent->bookmark);
            }
        }
    }
    else if(serviceEvent->EventTypeId() == Wisenet::Core::RemoveBookmarksEventType)
    {
        auto bookmarkEvent = std::static_pointer_cast<Wisenet::Core::RemoveBookmarksEvent>(event->eventDataPtr);
        for(auto& bookmarkID : bookmarkEvent->bookmarkIDs)
        {
            auto idIter = m_idMap.find(bookmarkID);
            if(idIter != m_idMap.end())
            {
                auto indexIter = std::find(m_indexVector.begin(), m_indexVector.end(), idIter->second);
                if(indexIter != m_indexVector.end())
                {
                    int row = std::distance(m_indexVector.begin(), indexIter);
                    remove(row);
                }

                m_idMap.erase(idIter);
            }
        }
    }
}

void BookmarkModel::connectMediaControlModelSignals(MediaControlModel* sender)
{
    connect(sender, &MediaControlModel::selectedChannelChanged, this, &BookmarkModel::onSelectedChannelChanged);
}

void BookmarkModel::onSelectedChannelChanged(const QString& deviceID, const QString& channelID)
{
    clear();
    m_bookmarks.clear();
    m_idMap.clear();

    m_selectedDeviceID = deviceID.toStdString();
    m_selectedChannelID = channelID.toStdString();

    if(m_selectedDeviceID == "" || m_selectedChannelID == "")
        return;

#ifndef MEDIA_FILE_ONLY
    std::map<Wisenet::uuid_string, Wisenet::Core::Bookmark> bookmarks = QCoreServiceManager::Instance().DB()->GetBookmarks();

    for(auto& bookmark : bookmarks)
    {
        if(bookmark.second.deviceID == m_selectedDeviceID && bookmark.second.channelID == m_selectedChannelID)
        {
            append(bookmark.second);
        }
    }
#endif
}

int BookmarkModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_indexVector.size();
}

QVariant BookmarkModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row < 0 || row >= m_indexVector.size())
        return QVariant();

    const Wisenet::Core::Bookmark& bookmark = m_bookmarks[m_indexVector[row]];

    switch(role)
    {
    case IdRole:
        return QString(bookmark.bookmarkID.c_str());
    case NameRole:
        return QString(bookmark.name.c_str());
    case DescriptionRole:
        return QString(bookmark.description.c_str());
    case TagsRole:
        return QString(bookmark.tag.c_str());
    case StartTimeRole:
        return QVariant::fromValue(bookmark.startTime);
    case EndTimeRole:
        return QVariant::fromValue(bookmark.endTime);
    case SelectedRole:
        return m_selectedBookmark.bookmarkID == bookmark.bookmarkID;
    }

    return QVariant();
}

QHash<int, QByteArray> BookmarkModel::roleNames() const
{
    return m_roleNames;
}

void BookmarkModel::insert(int index, const Wisenet::Core::Bookmark& bookmark)
{
    if(index < 0 || index > m_indexVector.size())
    {
        return;
    }

    beginInsertRows(QModelIndex(), index, index);
    m_indexVector.insert(m_indexVector.begin() + index, m_bookmarks.size());
    m_idMap[bookmark.bookmarkID] = m_bookmarks.size();
    m_bookmarks.push_back(bookmark);
    endInsertRows();
}

void BookmarkModel::append(const Wisenet::Core::Bookmark& bookmark)
{
    insert(m_indexVector.size(), bookmark);
}

void BookmarkModel::remove(int index)
{
    if(index < 0 || index > m_indexVector.size() - 1)
    {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_indexVector.erase(m_indexVector.begin() + index);
    endRemoveRows();
}

void BookmarkModel::clear()
{
    if(m_indexVector.size() == 0)
        return;

    beginRemoveRows(QModelIndex(), 0, m_indexVector.size()-1);
    m_indexVector.clear();
    endRemoveRows();
}

void BookmarkModel::setId(QString id)
{
    Wisenet::Core::Bookmark bookmark;
#ifndef MEDIA_FILE_ONLY
    if(QCoreServiceManager::Instance().DB()->FindBookmark(id.toStdString(), bookmark))
    {
        m_selectedBookmark = bookmark;
    }
    else
    {
        m_selectedBookmark.bookmarkID = id.toStdString();
    }
#else
    m_selectedBookmark = bookmark;
#endif

    /*
    auto iter = m_idMap.find(id.toStdString());
    if(iter != m_idMap.end())
    {
        m_selectedBookmark = m_bookmarks[iter->second];
    }
    else
    {
        m_selectedBookmark.bookmarkID = id.toStdString();
    }
    */

    emit nameChanged(name());
    emit descriptionChanged(description());
    emit tagsChanged(tags());
    emit startTimeChanged();
    emit endTimeChanged();
    emit cameraNameChanged();

    QVector<int> rols {SelectedRole};
    emit dataChanged(index(0,0,QModelIndex()), index(m_indexVector.size()-1,0,QModelIndex()), rols);
}

void BookmarkModel::newBookmark(QString deviceID, QString channelID, int overlappedID, qulonglong startTime, qulonglong endTime, QString eventLogID)
{
    Wisenet::Core::Bookmark bookmark;
#ifndef MEDIA_FILE_ONLY
    bookmark.bookmarkID = QCoreServiceManager::Instance().CreateNewUUidString();
#endif
    bookmark.deviceID = deviceID.toStdString();
    bookmark.channelID = channelID.toStdString();
    bookmark.name = "Bookmark";
    bookmark.description = "";
    bookmark.tag = "";
    bookmark.startTime = startTime;
    bookmark.endTime = endTime;
    bookmark.overlappedID = overlappedID;
    bookmark.eventLogID = eventLogID.toStdString();

    m_selectedBookmark = bookmark;
    emit idChanged(id());
    emit nameChanged(name());
    emit descriptionChanged(description());
    emit tagsChanged(tags());
    emit startTimeChanged();
    emit endTimeChanged();
    emit cameraNameChanged();

    QVector<int> rols {SelectedRole};
    emit dataChanged(index(0,0,QModelIndex()), index(m_indexVector.size()-1,0,QModelIndex()), rols);
}

void BookmarkModel::saveBookmark()
{
    auto bookmarkRequest = std::make_shared<Wisenet::Core::SaveBookmarkRequest>();
    bookmarkRequest->bookmark = m_selectedBookmark;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveBookmark,
                this, bookmarkRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        SPDLOG_INFO("saveBookmark, isFailed={}", response->isFailed());
    });
#endif
}

void BookmarkModel::removeBookmark(QString bookmarkID)
{
    auto bookmarkRequest = std::make_shared<Wisenet::Core::RemoveBookmarksRequest>();
    bookmarkRequest->bookmarkIDs.push_back(bookmarkID.toStdString());

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::RemoveBookmarks,
                this, bookmarkRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        SPDLOG_INFO("removeBookmark, isFailed={}", response->isFailed());
    });
#endif
}

void BookmarkModel::refreshBookmarkDataList(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth)
{

}

QString BookmarkModel::cameraName()
{
    Wisenet::Device::Device device;
#ifndef MEDIA_FILE_ONLY
    if(QCoreServiceManager::Instance().DB()->FindDevice(m_selectedDeviceID, device))
    {
        auto itr = device.channels.find(m_selectedChannelID);
        if (itr != device.channels.end())
            return QString::fromStdString(itr->second.name);
    }
#endif

    return "";
}

void BookmarkModel::setThumbnailImage(QImage image)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPEG");
    m_selectedBookmark.image = std::vector<unsigned char>(ba.begin(), ba.end());

    // std::vector<unsigned char> to QImage
    // QImage image = QImage::fromData(m_selectedBookmark.image.data(), m_selectedBookmark.image.size(), "JPEG");
}
