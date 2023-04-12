#include "BookmarkListViewModel.h"

BookmarkListViewModel::BookmarkListViewModel(QObject* parent) :
    QObject(parent)
{
    m_bookmarkListSortFilterProxyModel.setSourceModel(&m_bookmarkListModel);

    qDebug() << "BookmarkListViewModel::~BookmarkListViewModel()";
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &BookmarkListViewModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

BookmarkListViewModel::~BookmarkListViewModel()
{
    qDebug() << "BookmarkListViewModel::~BookmarkListViewModel()";
}

void BookmarkListViewModel::clear()
{
    qDebug() << "BookmarkListViewModel::clear()";
    m_bookmarkListModel.removeAll();
}

void BookmarkListViewModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;
    //SPDLOG_INFO("MainTreeModel get CoreServiceEvent!! EventType={}", serviceEvent->EventType());

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::FullInfoEventType:
        Event_FullInfo(event);
        break;
    case Wisenet::Core::SaveBookmarkEventType:
        Event_SaveBookmark(event);
        break;
    case Wisenet::Core::RemoveBookmarksEventType:
        Event_RemoveBookmark(event);
        break;
    case Wisenet::Core::SaveChannelsEventType:
        Event_SaveChannels(event);
        break;
    default:
        break;
    }
}

void BookmarkListViewModel::initializeList()
{
    std::map<Wisenet::uuid_string, Wisenet::Core::Bookmark> bookmarkMap = QCoreServiceManager::Instance().DB()->GetBookmarks();

    for (auto& kv : bookmarkMap) {
        auto& bookmark = kv.second;
        AddBookmarkItem(bookmark);
    }
}

void BookmarkListViewModel::Event_FullInfo(QCoreServiceEventPtr event)
{
    qDebug() << "BookmarkListViewModel::Event_FullInfo()";
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::FullInfoEvent>(event->eventDataPtr);

    for (auto& kv : coreEvent->bookmarks) {
        auto& bookmark = kv.second;
        AddBookmarkItem(bookmark);
    }
}

void BookmarkListViewModel::Event_SaveBookmark(QCoreServiceEventPtr event)
{
    qDebug() << "BookmarkListViewModel::Event_SaveBookmark()";
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::SaveBookmarkEvent>(event->eventDataPtr);

    Wisenet::Core::Bookmark bookmark = coreEvent->bookmark;

    QString bookmarkId = QString::fromStdString(bookmark.bookmarkID);

    if(m_bookmarkListModel.contains(bookmarkId))
        UpdateBookmarkItem(bookmark);
    else
    {
        AddBookmarkItem(bookmark);

        QString bookmarkId = QString::fromStdString(bookmark.bookmarkID);
        m_bookmarkListModel.selectedBookmarkChanged(bookmarkId);
    }
}

void BookmarkListViewModel::Event_RemoveBookmark(QCoreServiceEventPtr event)
{
    qDebug() << "BookmarkListViewModel::Event_RemoveBookmark()";
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::RemoveBookmarksEvent>(event->eventDataPtr);

    for (auto& id : coreEvent->bookmarkIDs)
    {
        QString bookmarkId = QString::fromStdString(id);

        m_bookmarkListModel.remove(bookmarkId);
    }
}

void BookmarkListViewModel::Event_SaveChannels(QCoreServiceEventPtr event)
{
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::SaveChannelsEvent>(event->eventDataPtr);

    for(auto& channel : coreEvent->channels)
    {
        QString id = QString::fromStdString(channel.deviceID) + "_" + QString::fromStdString(channel.channelID);
        QString name = QString::fromUtf8( channel.name.c_str());

        m_bookmarkListModel.applySaveChannels(id, name);

        qDebug() << "BookmarkListViewModel::Event_SaveChannels() Channel:"  << id << name;
    }
}

void BookmarkListViewModel::AddBookmarkItem(const Wisenet::Core::Bookmark& bookmark)
{    
    Wisenet::Device::Device device;
    QCoreServiceManager::Instance().DB()->FindDevice(bookmark.deviceID, device);
    Wisenet::Device::Device::Channel channel = device.channels.at(bookmark.channelID);

    QString deviceId = QString::fromStdString(bookmark.deviceID);
    QString channelId = QString::fromStdString(bookmark.channelID);

    QString bookmarkId = QString::fromStdString(bookmark.bookmarkID);
    QString bookmarkName = QString::fromUtf8(bookmark.name.c_str());
    QString sourceId = deviceId + "_" + channelId;
    QString cameraName = QString::fromUtf8(channel.name.c_str());
    QString description = QString::fromUtf8(bookmark.description.c_str());

    long long utcStartTime = bookmark.startTime;
    long long utcEndTime = bookmark.endTime;

    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(utcStartTime);
    QString displayTime = QLocaleManager::Instance()->getDateTime(dateTime);

    QImage image = QImage::fromData(bookmark.image.data(), bookmark.image.size(), "JPEG");

    BookmarkItemModel* bookmarkItemModel = new BookmarkItemModel(bookmarkId, bookmarkName, sourceId,
                                                                 cameraName, description, displayTime,
                                                                 utcStartTime, utcEndTime, image);


    m_bookmarkListModel.append(bookmarkItemModel);
}

void BookmarkListViewModel::UpdateBookmarkItem(const Wisenet::Core::Bookmark& bookmark)
{
    qDebug() << "BookmarkListViewModel::UpdateBookmarkItem()";
    Wisenet::Device::Device device;
    QCoreServiceManager::Instance().DB()->FindDevice(bookmark.deviceID, device);
    Wisenet::Device::Device::Channel channel = device.channels.at(bookmark.channelID);

    QString deviceId = QString::fromStdString(bookmark.deviceID);
    QString channelId = QString::fromStdString(bookmark.channelID);

    QString bookmarkId = QString::fromStdString(bookmark.bookmarkID);
    QString bookmarkName = QString::fromUtf8(bookmark.name.c_str());
    QString sourceId = deviceId + "_" + channelId;
    QString cameraName = QString::fromUtf8(channel.name.c_str());
    QString description = QString::fromUtf8(bookmark.description.c_str());

    long long utcStartTime = bookmark.startTime;
    long long utcEndTime = bookmark.endTime;

    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(utcStartTime);
    QString displayTime = QLocaleManager::Instance()->getDateTime(dateTime);

    QImage image = QImage::fromData(bookmark.image.data(), bookmark.image.size(), "JPEG");

    m_bookmarkListModel.update(bookmarkId, bookmarkName, sourceId,
                               cameraName, description, displayTime,
                               utcStartTime, utcEndTime, image);
    /*
    BookmarkItemModel* itemModel = m_bookmarkListModel.getItem(bookmarkId);

    itemModel->setBookmarkName(bookmarkName);
    itemModel->setSourceId(sourceId);
    itemModel->setCameraName(cameraName);
    itemModel->setDescription(description);
    itemModel->setDisplayTime(displayTime);
    itemModel->setUtcStartTime(utcStartTime);
    itemModel->setUtcEndTime(utcEndTime);
    itemModel->setImage(image);*/
}

void BookmarkListViewModel::removeBookmarkItem(QString bookmarkId)
{
    qDebug() << "ResourceTreeModel::removeBookmarkItem()";
    auto removeBookmarksRequest = std::make_shared<Wisenet::Core::RemoveBookmarksRequest>();
    removeBookmarksRequest->bookmarkIDs.push_back(bookmarkId.toStdString());

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::RemoveBookmarks,
                this, removeBookmarksRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = reply->responseDataPtr;
        if(response->isSuccess()){
            qDebug() << "ResourceTreeModel::removeBookmarkItem success";
        }
        else
            qDebug() << "ResourceTreeModel::removeBookmarkItem fail";
    });
}

