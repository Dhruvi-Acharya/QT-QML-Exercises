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
#include "MediaLayoutViewModel.h"
#include <QUuid>
#include "LogSettings.h"
#include <QDebug>
#include <QFileInfo>
#include <QStringList>
#include <QDesktopServices>
#include <boost/algorithm/string.hpp>
#include <string>
#include <algorithm>

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[MediaLayoutViewModel] "}, level, __VA_ARGS__)



MediaLayoutViewModel::MediaLayoutViewModel(QObject *parent, bool receiveCoreServiceEvent)
    : QObject(parent)
{
#ifndef MEDIA_FILE_ONLY
    if (receiveCoreServiceEvent) {
        connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
                this, &MediaLayoutViewModel::onCoreServiceEventTriggered, Qt::QueuedConnection);
    }
    m_maxVideoItems = QCoreServiceManager::Instance().Settings()->maxLayoutVideoItems();
    m_maxWebItems = QCoreServiceManager::Instance().Settings()->maxLayoutWebItems();

    Wisenet::Core::User user;
    QCoreServiceManager::Instance().DB()->GetLoginUser(user);
    QString loginId = QString::fromUtf8(user.loginID.c_str());
    setLoginId(loginId);

    Wisenet::Core::UserGroup userGroup;
    QCoreServiceManager::Instance().DB()->GetLoginUserGroup(userGroup);
    setUserAdmin(userGroup.isAdminGroup);
#endif

    // create default uuid
    m_uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_layoutId = m_uuid;

    SPDLOG_DEBUG("MediaLayoutViewModel(), has parent={}, uuid={}, loginID={}",
                 parent!=nullptr, m_uuid.toStdString(), m_loginId.toUtf8().constData());
}

MediaLayoutViewModel::~MediaLayoutViewModel()
{
    SPDLOG_DEBUG("~MediaLayoutViewModel(), uuid={}", m_uuid.toStdString());
}

QString MediaLayoutViewModel::uuid() const
{
    return m_uuid;
}

bool MediaLayoutViewModel::isAcceptableUrl(const QUrl &url)
{
    //SPDLOG_DEBUG("MediaLayoutItemViewModel::isAcceptableUrl");
    qDebug() << "check url:" << url;
    if (!url.isLocalFile()) {
        return false;
    }

    QString filePath = url.toLocalFile();
    QFileInfo fileInfo(filePath);

    QString fileSuffix = fileInfo.suffix();

    if(m_isMaskingMode)
    {
        if (isVideoFileForMaskingSuffix(fileSuffix))
            return true;
    }
    else
    {
        if (isImageFileSuffix(fileSuffix) || isVideoFileSuffix(fileSuffix))
            return true;
    }



    return false;
}

bool MediaLayoutViewModel::isVideoFile(const QUrl &url)
{
    if (!url.isLocalFile()) {
        return false;
    }

    QString filePath = url.toLocalFile();
    QFileInfo fileInfo(filePath);

    QString fileSuffix = fileInfo.suffix();

    if (isVideoFileSuffix(fileSuffix))
        return true;

    return false;
}

bool MediaLayoutViewModel::isVideoFileSuffix(const QString fileSuffix)
{
    static const QStringList supporedFiles = {"mkv", "mp4", "avi", "mov", "webm", "gif", "wnm", "sec"};

    if (supporedFiles.contains(fileSuffix, Qt::CaseInsensitive))
        return true;

    return false;
}

bool MediaLayoutViewModel::isVideoFileForMaskingSuffix(const QString fileSuffix)
{
    static const QStringList supporedFiles = {"wnm"};

    if (supporedFiles.contains(fileSuffix, Qt::CaseInsensitive))
        return true;

    return false;
}

bool MediaLayoutViewModel::isImageFileSuffix(const QString fileSuffix)
{
    static const QStringList supporedFiles = {"jpeg", "jpg", "png", "bmp", "svg"};
    if (supporedFiles.contains(fileSuffix, Qt::CaseInsensitive))
        return true;

    return false;

}

bool MediaLayoutViewModel::isNewLayout()
{
    Wisenet::Core::Layout layout;
    bool hasLayout = false;
#ifndef MEDIA_FILE_ONLY
    hasLayout = QCoreServiceManager::Instance().DB()->FindLayout(m_layoutId, layout);
#endif

    return !hasLayout;
}

QRect MediaLayoutViewModel::getCurrentLayoutRect(const bool ignoreBackgroundRect /* = true */)
{
    bool first = true;
    int lx = 0, rx = 1, ty = 0, by = 1;

    QHash<QString, MediaLayoutItemViewModel *>::iterator i = m_items.begin();
    while (i != m_items.end()) {
        MediaLayoutItemViewModel* item = i.value();
        if (item->invalid()) {
            ++i;
            continue;
        }

        if (first) {
            first = false;
            lx = item->m_column;
            rx = item->m_column + item->m_columnCount;
            ty = item->m_row;
            by = item->m_row + item->m_rowCount;
        }
        // 백그라운드 이미지 무시 체크
        else if (!ignoreBackgroundRect || !item->imageBackground()) {
            int x = item->m_column;
            int y = item->m_row;
            int x1 = item->m_column + item->m_columnCount;
            int y1 = item->m_row + item->m_rowCount;

            if (x < lx)     lx = x;
            if (x1 > rx)    rx = x1;
            if (y < ty)     ty = y;
            if (y1 > by)    by = y1;

        }
        ++i;
    }

    return QRect(lx, ty, rx-lx, by-ty);
}

bool MediaLayoutViewModel::isEmptyCell(int x, int y, int w, int h, const bool ignoreBackgroundRect /*= true*/)
{
    QHash<QString, MediaLayoutItemViewModel *>::iterator i = m_items.begin();
    while (i != m_items.end()) {
        MediaLayoutItemViewModel* item = i.value();
        if (item->invalid()) {
            ++i;
            continue;
        }
        if (ignoreBackgroundRect && item->imageBackground()) {
            ++i;
            continue;
        }

        if (x < item->m_column + item->m_columnCount && x+w > item->m_column &&
            y < item->m_row + item->m_rowCount && y+h > item->m_row) {
            return false;
        }
        ++i;
    }
    return true;
}



/* TODO :: 성능 및 알고리즘 최적화 */
QRect MediaLayoutViewModel::getNewItemPosition(const int w, const int h)
{
    SPDLOG_DEBUG("getNewItemPosition(), w={}, h={}", w, h);
    QRect dispRect = getCurrentLayoutRect();
    int startX = dispRect.left();
    int startY = dispRect.top();
    return getNewItemPositionWithStartPos(w, h, startX, startY);
}

QRect MediaLayoutViewModel::getNewItemPositionWithStartPos(const int w, const int h, const int startX, const int startY)
{

    SPDLOG_DEBUG("getNewItemPositionWithStartPos(), w={}, h={}, startX={}, startY={}",
                 w, h, startX, startY);
//    int w = 1;
//    int h = 1;
    int columnCount = 0;
    int rowCount = 0;
    int xInc = 0;
    int yInc = 0;

    bool found = false;
    while(!found) {
        if (rowCount >= columnCount) {
            for(xInc = 0 ; xInc <= columnCount ; xInc++) {
                if (isEmptyCell(startX+xInc, startY+rowCount, w, h)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                columnCount++;
            }
        }
        else { //if (columnCount > rowCount) {
            for(yInc = 0 ; yInc <= rowCount ; yInc++) {
                if (isEmptyCell(startX+columnCount, startY+yInc, w, h)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                rowCount++;
            }
        }
    }

    int x = startX+xInc;
    int y = startY+yInc;

    return QRect(x, y, w, h);
}

void MediaLayoutViewModel::openLocalDirPath(const QString &dirPath)
{
    QUrl url = QUrl::fromLocalFile(dirPath);
    QDesktopServices::openUrl(url);
}

QVariantList MediaLayoutViewModel::buildDropList(const QCoreServiceDatabase::GroupResource &groupResource,
                                                 const std::set<std::string>& localResource)
{
    QList<OpenLayoutItem> results;
    for (auto& channel : groupResource.channels) {
        const auto idx = channel.find_first_of('_');
        if (std::string::npos != idx) {
            OpenLayoutItem item;
            auto deviceId = channel.substr(0, idx);
            auto channelId = channel.substr(idx + 1);
            boost::trim(deviceId);
            boost::trim(channelId);
            item.setItemTypeId(0);
            item.setItemType("channel");
            item.setItemId(QString::fromStdString(deviceId));
            item.setSubId(QString::fromStdString(channelId));

            results.append(item);
        }
        else {
            SPDLOG_INFO("buildDropList(), channel parse failed, channel={}", channel);
        }
    }
    for (auto& webPage : groupResource.webPages) {
        OpenLayoutItem item;
        item.setItemTypeId(1);
        item.setItemType("webpage");
        item.setItemId(QString::fromStdString(webPage));
        results.append(item);
    }
    for (auto& lc : localResource) {
        OpenLayoutItem item;
        item.setItemTypeId(2);
        item.setItemType("localresource");
        item.setItemId(QString::fromStdString(lc));
        results.append(item);
    }
    // sort by name
#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().DB()->FillResourceNameFromOpenLayoutItems(results);
#endif
    std::sort(results.begin(), results.end(), [](const OpenLayoutItem& s1, const OpenLayoutItem& s2) {
        return s1.name() < s2.name();
    });

    QVariantList vResults;
    for (const auto& v : results) {
        vResults.append(QVariant::fromValue(v));
    }
    return vResults;
}

QVariantList MediaLayoutViewModel::getChannelsInGroup(const QString &groupID)
{
    QCoreServiceDatabase::GroupResource resource;
    std::set<std::string> localResource;
#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().DB()->AllResourcesInGroup(groupID, resource);
#endif

    return buildDropList(resource, localResource);
}

QVariantList MediaLayoutViewModel::parseDragItemListModel(DragItemListModel *source)
{
    if (!source) {
        SPDLOG_WARN("parseDragItemListModel(), source is null");
        QVariantList results;
        return results;
    }

    if(m_isMaskingMode)
    {
        return parseDragItemListModelForMaskingMode(source);
    }

    QCoreServiceDatabase::GroupResource resource;
    std::set<std::string> localResource;
    std::set<std::string> layouts;
    std::set<std::string> sequences;

    QList<DragItemModel*> itemList = source->itemList();
    SPDLOG_DEBUG("parseDragItemListModel(), size={}", itemList.count());
    for (auto& item : itemList) {
        SPDLOG_DEBUG("DragItemModel type={}, uuid={}", item->itemType(), item->uuid().toStdString());
        if (item->itemType() == (int)WisenetViewerDefine::WisenetItemType::Group) {
#ifndef MEDIA_FILE_ONLY
            QCoreServiceManager::Instance().DB()->AllResourcesInGroup(item->uuid(), resource);
#endif
        }
        else if (item->itemType() == (int)WisenetViewerDefine::WisenetItemType::Camera){
            resource.channels.insert(item->uuid().toStdString());
        }
        else if (item->itemType() == (int)WisenetViewerDefine::WisenetItemType::WebPage){
            resource.webPages.insert(item->uuid().toStdString());
        }
        else if (item->itemType() == (int)WisenetViewerDefine::WisenetItemType::LocalFile){
            localResource.insert(item->uuid().toStdString());
        }
        else if (item->itemType() == (int)WisenetViewerDefine::WisenetItemType::Layout){
            layouts.insert(item->uuid().toStdString());
        }
        else if (item->itemType() == (int)WisenetViewerDefine::WisenetItemType::Sequence){
            sequences.insert(item->uuid().toStdString());
        }
    }
    // layout이 하나라도 들어가 있는 경우에는 새탭에서 열기 시그널 리턴해야 한다.
    if (sequences.size() > 0) {
        QList<OpenLayoutItem> results;
        for (auto& s : sequences) {
            OpenLayoutItem item;
            item.setItemTypeId(4);
            item.setItemType("sequence");
            item.setItemId(QString::fromStdString(s));
            results.append(item);
        }
#ifndef MEDIA_FILE_ONLY
        QCoreServiceManager::Instance().DB()->FillResourceNameFromOpenLayoutItems(results);
#endif

        // sort by name
        std::sort(results.begin(), results.end(), [](const OpenLayoutItem& s1, const OpenLayoutItem& s2) {
            return s1.name() < s2.name();
        });

        QVariantList vResults;
        for (const auto& v : results) {
            vResults.append(QVariant::fromValue(v));
        }

        return vResults;
    }
    else if (layouts.size() > 0) {
        QList<OpenLayoutItem> results;
        for (auto& l : layouts) {
            OpenLayoutItem item;
            item.setItemTypeId(3);
            item.setItemType("layout");
            item.setItemId(QString::fromStdString(l));
            results.append(item);
        }
#ifndef MEDIA_FILE_ONLY
        QCoreServiceManager::Instance().DB()->FillResourceNameFromOpenLayoutItems(results);
#endif

        // sort by name
        std::sort(results.begin(), results.end(), [](const OpenLayoutItem& s1, const OpenLayoutItem& s2) {
            return s1.name() < s2.name();
        });

        QVariantList vResults;
        for (const auto& v : results) {
            vResults.append(QVariant::fromValue(v));
        }

        return vResults;
    }
    else {
        return buildDropList(resource, localResource);
    }
}

QVariantList MediaLayoutViewModel::parseDragItemListModelForMaskingMode(DragItemListModel *source)
{
    QCoreServiceDatabase::GroupResource resource;
    std::set<std::string> localResource;

    QList<DragItemModel*> itemList = source->itemList();
    qDebug() << "parseDragItemListModelForMaskingMode" << itemList.count();
    SPDLOG_DEBUG("parseDragItemListModelForMaskingMode(), size={}", itemList.count());
    for (auto& item : itemList) {
        SPDLOG_DEBUG("DragItemModel type={}, uuid={}", item->itemType(), item->uuid().toStdString());

        if (item->itemType() == (int)WisenetViewerDefine::WisenetItemType::LocalFile)
        {
            qDebug() << "parseDragItemListModelForMaskingMode" << item->uuid();

            if(item->uuid().endsWith(".wnm"))
            {
                qDebug() << "parseDragItemListModelForMaskingMode insert" << item->uuid();
                localResource.insert(item->uuid().toStdString());
                break;
            }
        }
    }

    return buildDropList(resource, localResource);
}

void MediaLayoutViewModel::setName(const QString &name) {
    if (name != m_name) {
        m_name = name;
        emit nameChanged();
        setSaveStatus(false);
    }
}

QString MediaLayoutViewModel::name() const {
    return m_name;
}

void MediaLayoutViewModel::setLayoutId(const QString &id)
{
    if (m_layoutId != id) {
        m_layoutId = id;
        emit layoutIdChanged();
    }
}

QString MediaLayoutViewModel::layoutId() const
{
    return m_layoutId;
}

void MediaLayoutViewModel::setOwnerId(const QString &id)
{
    if (m_ownerId != id) {
        m_ownerId = id;
        emit ownerIdChanged();
    }
}

QString MediaLayoutViewModel::ownerId() const
{
    return m_ownerId;
}

QString MediaLayoutViewModel::loginId() const
{
    return m_loginId;
}

void MediaLayoutViewModel::setLoginId(const QString loginId)
{
    if (m_loginId != loginId) {
        m_loginId = loginId;
        emit loginIdChanged();
    }
}

void MediaLayoutViewModel::setLocked(const bool locked)
{
    if (locked != m_locked) {
        m_locked = locked;
        emit lockedChanged();
        setSaveStatus(false);

        // 현재 포함된 아이템들의 전체 lock 속성을 변경한다.
        QHash<QString, MediaLayoutItemViewModel *>::iterator i = m_items.begin();
        while (i != m_items.end()) {
            MediaLayoutItemViewModel* item = i.value();
            item->setLocked(locked);
            ++i;
        }
    }
}

bool MediaLayoutViewModel::locked() const
{
    return m_locked;
}

void MediaLayoutViewModel::setUserAdmin(const bool isAdmin)
{
    if (m_userAdmin != isAdmin) {
        m_userAdmin = isAdmin;
        emit userAdminChanged();
    }
}

bool MediaLayoutViewModel::userAdmin() const
{
    return m_userAdmin;
}

void MediaLayoutViewModel::setCellRatioW(const int ratioW)
{
    if (m_cellRatioW != ratioW) {
        m_cellRatioW = ratioW;
        emit cellRatioWChanged();
        setSaveStatus(false);
    }
}

int MediaLayoutViewModel::cellRatioW() const
{
    return m_cellRatioW;
}


void MediaLayoutViewModel::setCellRatioH(const int ratioH)
{
    if (m_cellRatioH != ratioH) {
        m_cellRatioH = ratioH;
        emit cellRatioHChanged();
        setSaveStatus(false);
    }
}

int MediaLayoutViewModel::cellRatioH() const
{
    return m_cellRatioH;
}

int MediaLayoutViewModel::maxVideoItems() const
{
    //return 3;
    return m_maxVideoItems;
}

int MediaLayoutViewModel::maxWebItems() const
{
    //return 1;
    return m_maxWebItems;
}

MediaLayoutItemViewModel::ItemFillMode MediaLayoutViewModel::layoutFillMode() const
{
    return m_layoutFillMode;
}

void MediaLayoutViewModel::setLayoutFillMode(const MediaLayoutItemViewModel::ItemFillMode mode)
{
    if (m_layoutFillMode != mode) {
        m_layoutFillMode = mode;
        emit layoutFillModeChanged();

        // 현재 포함된 아이템들의 전체 fill mode 속성을 변경한다.
        QHash<QString, MediaLayoutItemViewModel *>::iterator i = m_items.begin();
        while (i != m_items.end()) {
            MediaLayoutItemViewModel* item = i.value();
            if (item->itemType() == MediaLayoutItemViewModel::Camera ||
                item->itemType() == MediaLayoutItemViewModel::LocalResource) {
                item->setItemFillMode(mode);
            }
            ++i;
        }
    }
}

MediaLayoutItemViewModel::ItemProfile MediaLayoutViewModel::layoutProfile() const
{
    return m_layoutProfile;
}

void MediaLayoutViewModel::setLayoutProfile(const MediaLayoutItemViewModel::ItemProfile profile)
{
    if (m_layoutProfile != profile) {
        m_layoutProfile = profile;
        emit layoutProfileChanged();

        // 현재 포함된 아이템들의 전체 profile 속성을 변경한다.
        QHash<QString, MediaLayoutItemViewModel *>::iterator i = m_items.begin();
        while (i != m_items.end()) {
            MediaLayoutItemViewModel* item = i.value();
            if (item->itemType() == MediaLayoutItemViewModel::Camera) {
                item->setItemProfile(profile);
            }
            ++i;
        }
    }
}

bool MediaLayoutViewModel::videoStatus() const
{
    return m_videoStatusDisplay;
}

void MediaLayoutViewModel::setVideoStatus(const bool status)
{
    if (m_videoStatusDisplay != status) {
        m_videoStatusDisplay = status;
        emit videoStatusChanged();

        // 현재 포함된 아이템들의 전체 video status 속성을 변경한다.
        QHash<QString, MediaLayoutItemViewModel *>::iterator i = m_items.begin();
        while (i != m_items.end()) {
            MediaLayoutItemViewModel* item = i.value();
            item->setVideoStatus(status);
            ++i;
        }
    }
}

void MediaLayoutViewModel::setSaveStatus(const bool flag)
{
    if (flag != m_saveStatus) {
        m_saveStatus = flag;
        SPDLOG_DEBUG("setSaveStatus(), {}", flag);
        emit saveStatusChanged();
    }
}

bool MediaLayoutViewModel::saveStatus() const
{
    return m_saveStatus;
}

void MediaLayoutViewModel::setViewingGridViewModel(const bool flag)
{
    if (flag != m_viewingGridViewModel) {
        m_viewingGridViewModel = flag;
        emit viewingGridViewModelChanged();
    }
}

bool MediaLayoutViewModel::viewingGridViewModel() const
{
    return m_viewingGridViewModel;
}

void MediaLayoutViewModel::setIsMaskingMode(const bool isMaskingMode)
{
    m_isMaskingMode = isMaskingMode;
    emit isMaskingModeChanged();
}

bool MediaLayoutViewModel::isMaskingMode() const
{
    return m_isMaskingMode;
}

QQmlListProperty<MediaLayoutItemViewModel> MediaLayoutViewModel::layoutItems()
{
    m_itemlist = m_items.values();
    return QQmlListProperty<MediaLayoutItemViewModel>(this, &m_itemlist);
}

void MediaLayoutViewModel::cleanUp()
{
    QHash<QString, MediaLayoutItemViewModel *>::iterator i = m_items.begin();
    while (i != m_items.end()) {
        MediaLayoutItemViewModel* item = i.value();
        if (item->parent() == this) {
            delete item;
        }
        ++i;
    }
    m_items.clear();
}

void MediaLayoutViewModel::loadFromCoreService(Wisenet::Core::Layout &layout)
{
    SPDLOG_DEBUG("loadFromCoreService(), uuid={}", m_uuid.toStdString());

    // 이전데이터가 있으면 초기화한다.
    cleanUp();

    m_name = QString::fromUtf8(layout.name.c_str());
    m_layoutId = QString::fromStdString(layout.layoutID);
    m_ownerId = QString::fromUtf8(layout.userName.c_str());
    m_locked = layout.isLocked;
    m_cellRatioW = layout.cellWidth;
    m_cellRatioH = layout.cellHeight;

    if (layout.fillMode == Wisenet::Core::LayoutItem::FillMode::PreserveAspectRatio)
        m_layoutFillMode = MediaLayoutItemViewModel::ItemFillMode::PreserveAspectRatio;
    else if (layout.fillMode == Wisenet::Core::LayoutItem::FillMode::Stretch)
        m_layoutFillMode = MediaLayoutItemViewModel::ItemFillMode::Stretch;

    if (layout.profile == Wisenet::Core::LayoutItem::StreamProfile::Auto)
        m_layoutProfile = MediaLayoutItemViewModel::ItemProfile::Auto;
    else if (layout.profile == Wisenet::Core::LayoutItem::StreamProfile::High)
        m_layoutProfile = MediaLayoutItemViewModel::ItemProfile::High;
    else if (layout.profile == Wisenet::Core::LayoutItem::StreamProfile::Low)
        m_layoutProfile = MediaLayoutItemViewModel::ItemProfile::Low;

    for (auto &layoutItem : layout.layoutItems) {
        MediaLayoutItemViewModel* itemModel = new MediaLayoutItemViewModel(this);
        itemModel->fromCoreServiceLayoutItem(layoutItem);
        itemModel->setLocked(m_locked); // layout의 lock정보를 item에 복사한다.
        QString key = itemModel->itemId();
        m_items[key] = itemModel;
    }
    setSaveStatus(true);
}


void MediaLayoutViewModel::save(const bool saveAs /*=false*/, const QString &newName/*=""*/)
{
    bool isSaveAs = saveAs;

    auto saveRequest = std::make_shared<Wisenet::Core::SaveLayoutRequest>();
    Wisenet::Core::Layout& layout = saveRequest->layout;

    if (isSaveAs) {
        // 새로운 레이아웃이름과 레이아웃 아이디를 할당한다.
        // lock = false 상태여야 한다.
        QString newLayoutId = QUuid::createUuid().toString(QUuid::WithoutBraces);
        layout.layoutID = newLayoutId.toStdString();
        layout.name = newName.toUtf8().constData();
        layout.isLocked = false;
        layout.userName = m_loginId.toUtf8().constData();
    }
    else {
        layout.layoutID = m_layoutId.toStdString();
        layout.name = m_name.toUtf8().constData();
        layout.isLocked = m_locked;
        // owner가 set되어 있지 않으면 login id를 ownerid로 세팅한다.
        if (m_ownerId.isEmpty()) {
            setOwnerId(m_loginId);
        }
        layout.userName = m_ownerId.toUtf8().constData();
    }

    layout.cellWidth = m_cellRatioW;
    layout.cellHeight = m_cellRatioH;
    layout.cellSpacing = 0.0;

    if (m_layoutFillMode == MediaLayoutItemViewModel::ItemFillMode::PreserveAspectRatio)
        layout.fillMode = Wisenet::Core::LayoutItem::FillMode::PreserveAspectRatio;
    else if (m_layoutFillMode == MediaLayoutItemViewModel::ItemFillMode::Stretch)
        layout.fillMode = Wisenet::Core::LayoutItem::FillMode::Stretch;

    if ( m_layoutProfile == MediaLayoutItemViewModel::ItemProfile::Auto)
        layout.profile = Wisenet::Core::LayoutItem::StreamProfile::Auto;
    else if ( m_layoutProfile == MediaLayoutItemViewModel::ItemProfile::High)
        layout.profile = Wisenet::Core::LayoutItem::StreamProfile::High;
    else if ( m_layoutProfile == MediaLayoutItemViewModel::ItemProfile::Low)
        layout.profile = Wisenet::Core::LayoutItem::StreamProfile::Low;

    QHash<QString, MediaLayoutItemViewModel *>::iterator i;

    // save as시에는 item id 생성 및 zoom target id를 다시 연결시켜줘야 한다.
    if (isSaveAs) {
        std::unordered_map<Wisenet::uuid_string, Wisenet::uuid_string> idMap;
        for (i = m_items.begin(); i != m_items.end() ; ++i) {
            Wisenet::Core::LayoutItem layoutItem = i.value()->toCoreServiceLayoutItem();
            auto originID = layoutItem.itemID;
            QString newItemId = QUuid::createUuid().toString(QUuid::WithoutBraces);
            layoutItem.itemID = newItemId.toStdString();
            idMap[originID] = layoutItem.itemID;
            layout.layoutItems.emplace_back(layoutItem);
        }
        for (auto& v : layout.layoutItems) {
            if (v.itemType == Wisenet::Core::LayoutItem::LayoutItemType::ZoomTarget) {
                auto origTargtetID = v.zoomTargetParam.zoomTargetID ;
                v.zoomTargetParam.zoomTargetID = idMap[origTargtetID];
            }
        }

    }
    else {
        for (i = m_items.begin(); i != m_items.end() ; ++i) {
            Wisenet::Core::LayoutItem layoutItem = i.value()->toCoreServiceLayoutItem();
            layout.layoutItems.emplace_back(layoutItem);
        }
    }


    SPDLOG_DEBUG("SaveLayout(), isSaveAs={}, REQUEST START ITEMS={}, userName={}",
                 isSaveAs, layout.layoutItems.size(), layout.userName);
#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveLayout,
                this, saveRequest,
                [this](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        SPDLOG_DEBUG("SaveLayout, isSuccess={}", response->isSuccess());
    });
#endif
}

// 저장하기
void MediaLayoutViewModel::saveToCoreService()
{
    save();
}

// 다른 이름이름으로 저장하기
void MediaLayoutViewModel::saveAsToCoreService(const QString &layoutName)
{
    save(true, layoutName);
}

void MediaLayoutViewModel::addItem(MediaLayoutItemViewModel *itemModel)
{
    QString key = itemModel->m_itemId;
    SPDLOG_DEBUG("addItem={}", key.toStdString());
    if (m_items.contains(key) == true) {
        SPDLOG_WARN("addItem() failed, already exist itemId={}", key.toStdString());
        return;
    }
    m_items[key] = itemModel;

    // 아이템의 변경사항을 layout에서 받는다.
    connect(itemModel, &MediaLayoutItemViewModel::itemUpdated, this, [=]() {
        //SPDLOG_DEBUG("itemUpdated from ItemViewModel!!, uuid={}", m_uuid.toStdString());
        setSaveStatus(false);
    });

    setSaveStatus(false);
}

void MediaLayoutViewModel::addCameraItem(const QString &deviceId, QString channelId)
{
    QRect r = getNewItemPosition(1, 1);
    MediaLayoutItemViewModel* itemModel = new MediaLayoutItemViewModel(this);
    itemModel->m_itemType = MediaLayoutItemViewModel::ItemType::Camera;
    itemModel->m_deviceId = deviceId;
    itemModel->m_channelId = channelId;
    itemModel->initPosition(r);
    addItem(itemModel);
}

void MediaLayoutViewModel::addLocalVideoItem(const QString &fileUrl)
{
    QRect r = getNewItemPosition(1, 1);
    MediaLayoutItemViewModel* itemModel = new MediaLayoutItemViewModel(this);
    itemModel->m_itemType = MediaLayoutItemViewModel::ItemType::LocalResource;
    itemModel->m_localResource = fileUrl;
    itemModel->initPosition(r);
    addItem(itemModel);
}

void MediaLayoutViewModel::addLocalImageItem(const QString &fileUrl)
{
    QRect r = getNewItemPosition(1, 1);
    MediaLayoutItemViewModel* itemModel = new MediaLayoutItemViewModel(this);
    itemModel->m_itemType = MediaLayoutItemViewModel::ItemType::Image;
    itemModel->m_localResource = fileUrl;
    itemModel->initPosition(r);

    addItem(itemModel);
}

void MediaLayoutViewModel::addWebpageItem(const QString &webPageId)
{
    QRect r = getNewItemPosition(1, 1);
    MediaLayoutItemViewModel* itemModel = new MediaLayoutItemViewModel(this);
    itemModel->m_itemType = MediaLayoutItemViewModel::ItemType::Webpage;
    itemModel->m_webPageId = webPageId;
    itemModel->initPosition(r);
    addItem(itemModel);
}

void MediaLayoutViewModel::removeItem(const QString& itemId, bool changeSaveStstus)
{
    QString key = itemId;
    SPDLOG_DEBUG("removeItem={}", key.toStdString());
    if (m_items.contains(key)) {
        MediaLayoutItemViewModel* ptr = m_items.take(key);

        // 연결관계를 끊는다.
        disconnect(ptr, nullptr, this, nullptr);

        // 자기가 생성한 item인 경우에는 삭제해야 한다.
        if (ptr && ptr->parent() == this) {
            delete ptr;
        }

        if(changeSaveStstus)
            setSaveStatus(false);
    }
    else {
        SPDLOG_WARN("removeItem() failed, could not find itemId={}", key.toStdString());
    }
}

void MediaLayoutViewModel::syncRemovedItems(QList<QString> itemIds)
{
    SPDLOG_DEBUG("syncRemovedItems={}", itemIds.size());
    foreach(const QString& itemId, itemIds) {
        removeItem(itemId);
    }
}

void MediaLayoutViewModel::syncAddedItems(QVariantList items)
{
    SPDLOG_DEBUG("syncAddedItems={}", items.size());
    foreach(QVariant v, items) {
        if (v.canConvert<MediaLayoutItemViewModel*>()) {
            MediaLayoutItemViewModel* srcItem = v.value<MediaLayoutItemViewModel*>();
            if (m_items.contains(srcItem->m_itemId)) {
                SPDLOG_WARN("syncAddedItems(), already exist itemId={}", srcItem->m_itemId.toStdString());
                continue;
            }

            MediaLayoutItemViewModel* newItem = new MediaLayoutItemViewModel(this);
            newItem->updateProperty(srcItem);
            SPDLOG_DEBUG("syncAddedItems(), newItem={}, srcItem={}",
                         newItem->m_itemId.toStdString(),
                         srcItem->m_itemId.toStdString());
            addItem(newItem);
        }
        else {
            SPDLOG_ERROR("syncAddedItems(), can not convert to MediaLayoutItemViewModel*");
        }
    }
}

void MediaLayoutViewModel::syncUpdatedItems(QVariantList items)
{
    //SPDLOG_DEBUG("syncUpdatedItems={}, uuid={}", items.size(), m_uuid.toStdString());
    foreach(QVariant v, items) {
        if (v.canConvert<MediaLayoutItemViewModel*>()) {
            MediaLayoutItemViewModel* srcItem = v.value<MediaLayoutItemViewModel*>();
            auto i = m_items.find(srcItem->m_itemId);
            if (i == m_items.end()) {
                SPDLOG_WARN("syncUpdatedItems(), can not find itemId={}", srcItem->m_itemId.toStdString());
                continue;
            }
            i.value()->updateProperty(srcItem);
            //SPDLOG_DEBUG("syncUpdatedItems(), updateItem={}", srcItem->m_itemId.toStdString());
            setSaveStatus(false);
        }
        else {
            SPDLOG_ERROR("syncUpdatedItems(), can not convert to MediaLayoutItemViewModel*");
        }
    }
}

void MediaLayoutViewModel::syncPropertyUpdatdLayout(MediaLayoutViewModel *otherModel)
{
    SPDLOG_DEBUG("syncPropertyUpdatdLayout()");
    updateProperty(otherModel);
    setSaveStatus(false);
}


void MediaLayoutViewModel::updateProperty(MediaLayoutViewModel *layout)
{
    setOwnerId(layout->m_ownerId);
    setName(layout->m_name);
    setLayoutId(layout->m_layoutId);
    setLocked(layout->m_locked);
    setCellRatioW(layout->m_cellRatioW);
    setCellRatioH(layout->m_cellRatioH);
    setLayoutFillMode(layout->m_layoutFillMode);
    setLayoutProfile(layout->m_layoutProfile);
}


void MediaLayoutViewModel::onCoreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    if (serviceEvent->EventTypeId() ==  Wisenet::Core::SaveLayoutEventType) {
        auto layoutEvent = std::static_pointer_cast<Wisenet::Core::SaveLayoutEvent>(serviceEvent);
        std::string layoutId = m_layoutId.toStdString();
        if (layoutEvent->layout.layoutID == layoutId) {
            if (m_viewingGridViewModel) {
                SPDLOG_DEBUG("onCoreServiceEventTriggered(0), update ViewingGrid layout. laytoutId={}, uuid={}",
                             layoutId, m_uuid.toStdString());
                MediaLayoutViewModel *newModel = new MediaLayoutViewModel(this, false);
                newModel->loadFromCoreService(layoutEvent->layout);
                emit layoutModelUpdated(newModel);
                newModel->deleteLater();
            }
            else {
                SPDLOG_DEBUG("onCoreServiceEventTriggered(0), reload layout. layoutId={}, uuid={}",
                             layoutId, m_uuid.toStdString());
                loadFromCoreService(layoutEvent->layout);
            }
        }
    }
    else if (serviceEvent->EventTypeId() ==  Wisenet::Core::SaveLayoutsEventType) {
        auto layoutsEvent = std::static_pointer_cast<Wisenet::Core::SaveLayoutsEvent>(serviceEvent);
        std::string layoutId = m_layoutId.toStdString();
        auto itr = layoutsEvent->layouts.find(layoutId);
        if (itr != layoutsEvent->layouts.end()) {
            SPDLOG_DEBUG("Update layout data from Wisenet::Core::SaveLayoutsEventType, layoutId={}", layoutId);
            if (m_viewingGridViewModel) {
                SPDLOG_DEBUG("onCoreServiceEventTriggered(1), update ViewingGrid layout. laytoutId={}, uuid={}",
                             layoutId, m_uuid.toStdString());
                MediaLayoutViewModel *newModel = new MediaLayoutViewModel(this, false);
                newModel->loadFromCoreService(itr->second);
                emit layoutModelUpdated(newModel);
                newModel->deleteLater();
            }
            else {
                SPDLOG_DEBUG("onCoreServiceEventTriggered(1), reload layout. layoutId={}, uuid={}",
                             layoutId, m_uuid.toStdString());
                loadFromCoreService(itr->second);
            }
        }
    }
    else if(serviceEvent->EventTypeId() == Wisenet::Core::RemoveDevicesEventType) {
        // 저장된 레이아웃이 아닌 경우, ViewingGird에서 처리 후 Tree쪽으로 remove item 이벤트 전송한다.
        if (m_viewingGridViewModel && isNewLayout()) {
            SPDLOG_DEBUG("onCoreServiceEventTriggered(), RemoveDevicesEventType");
            auto removeDeviceEvent = std::static_pointer_cast<Wisenet::Core::RemoveDevicesEvent>(serviceEvent);
            QList<QString> deviceList;
            for (auto& deviceID : removeDeviceEvent->deviceIDs) {
                deviceList.push_back(QString::fromStdString(deviceID));
            }
            emit deviceRemoved(deviceList);
        }
    }
    else if(serviceEvent->EventTypeId() == Wisenet::Core::SaveChannelsEventType) {
        // 저장된 레이아웃이 아닌 경우, ViewingGird에서 처리 후 Tree쪽으로 remove item 이벤트 전송한다.
        if (m_viewingGridViewModel && isNewLayout()) {
            SPDLOG_DEBUG("onCoreServiceEventTriggered(), RemoveDevicesEventType");
            auto saveChannelsEvent = std::static_pointer_cast<Wisenet::Core::SaveChannelsEvent>(serviceEvent);

            QList<QString> channelList;
            for (auto const&channel : saveChannelsEvent->channels) {
                if (channel.use == false){
                    channelList.push_back(QString::fromStdString(channel.deviceID + "_" + channel.channelID));
                }
            }

            emit cameraRemoved(channelList);
        }
    }
    else if(serviceEvent->EventTypeId() == Wisenet::Core::UpdateVerifyResultEventType){
        // 서명 검증 결과가 업데이트 되었을 때
        auto updateVerifyResultEvent = std::static_pointer_cast<Wisenet::Core::UpdateVerifyResultEvent>(event->eventDataPtr);
        QString filePath = QString::fromStdString(updateVerifyResultEvent->fileUuid);
        int verifyResult = updateVerifyResultEvent->verifyResult;
        SPDLOG_DEBUG("MediaLayoutViewModel::onCoreServiceEventTriggered(), UpdateVerifyResultEventType");
        updateVerifyResult(filePath, verifyResult);
    }
    // 웹페이지가 변경되었을 때
    else if(m_viewingGridViewModel &&
            serviceEvent->EventTypeId() == Wisenet::Core::SaveWebpageEventType) {
        SPDLOG_DEBUG("onCoreServiceEventTriggered(), SaveWebpageEventType");
        auto saveWebpageEvent = std::static_pointer_cast<Wisenet::Core::SaveWebpageEvent>(serviceEvent);
        QString webpageId = QString::fromStdString(saveWebpageEvent->webpage.webpageID);
        emit webpageSaved(webpageId);
    }

    // 로그인 정보 갱신
    else if (m_viewingGridViewModel &&
             serviceEvent->EventTypeId() ==  Wisenet::Core::FullInfoEventType) {
#ifndef MEDIA_FILE_ONLY
        Wisenet::Core::User user;
        QCoreServiceManager::Instance().DB()->GetLoginUser(user);
        QString loginId = QString::fromUtf8(user.loginID.c_str());
        setLoginId(loginId);

        Wisenet::Core::UserGroup userGroup;
        QCoreServiceManager::Instance().DB()->GetLoginUserGroup(userGroup);
        setUserAdmin(userGroup.isAdminGroup);
#endif
    }
}

void MediaLayoutViewModel::updateVerifyResult(QString filePath, int verifyResult)
{
    if (verifyResult > 0) {
        for(auto &itemModel : m_items)
        {
            if(filePath == itemModel->localResource().toString()){
                QString result = "Unknown";
                switch(verifyResult)
                {
                case 4:
                    result = "Authenticated";
                    break;
                case 5:
                    result = "Manipualated";
                    break;
                case 6:
                    result = "Not signed";
                    break;
                case 7:
                    result = "Verifying";
                    break;
                }
                itemModel->setVerifiedInfo(result);
            }
        }
    }
}

