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
#include "MediaLayoutItemViewModel.h"
#include "MediaLayoutViewModel.h"
#include "QCoreServiceManager.h"
#include "LogSettings.h"
#include <QUuid>
#include <QDebug>

MediaLayoutItemViewModel::MediaLayoutItemViewModel(QObject *parent)
    : QObject(parent)
{
    SPDLOG_DEBUG("MediaLayoutItemViewModel::MediaLayoutItemViewModel(), has parent={}", parent!=nullptr);

    // create default uuid
    m_uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_itemId = m_uuid;
}

MediaLayoutItemViewModel::~MediaLayoutItemViewModel()
{
    SPDLOG_DEBUG("MediaLayoutItemViewModel::~MediaLayoutItemViewModel()");
}


QString MediaLayoutItemViewModel::uuid() const
{
    return m_uuid;
}


void MediaLayoutItemViewModel::setItemId(const QString &itemId)
{
    if (m_itemId != itemId) {
        m_itemId = itemId;
        emit itemIdChanged();
    }
}

QString MediaLayoutItemViewModel::itemId() const
{
    return m_itemId;
}

void MediaLayoutItemViewModel::setItemType(const ItemType &itemType)
{
    if (m_itemType != itemType) {
        m_itemType = itemType;
        emit itemTypeChanged();
    }
}

MediaLayoutItemViewModel::ItemType MediaLayoutItemViewModel::itemType() const
{
    return m_itemType;
}

void MediaLayoutItemViewModel::setLocked(const bool locked)
{
    // 개별 아이템의 lock 정보는 db에 저장하지 않는다.
    if (locked != m_locked) {
        m_locked = locked;
        emit lockedChanged();
    }
}

bool MediaLayoutItemViewModel::locked() const
{
    return m_locked;
}

void MediaLayoutItemViewModel::setColumn(const int column)
{
    if (column != m_column) {
        m_column = column;
        //SPDLOG_DEBUG("Column changed...{}", column);
        emit columnChanged();
        emit itemUpdated();
    }
}

int MediaLayoutItemViewModel::column() const
{
    return m_column;
}

void MediaLayoutItemViewModel::setRow(const int row)
{
    if (row != m_row) {
        m_row = row;
        emit rowChanged();
        emit itemUpdated();
    }
}

int MediaLayoutItemViewModel::row() const
{
    return m_row;
}

void MediaLayoutItemViewModel::setColumnCount(const int columnCount)
{
    if (m_columnCount != columnCount) {
        m_columnCount = columnCount;
        emit columnCountChanged();
        emit itemUpdated();
    }
}

int MediaLayoutItemViewModel::columnCount() const
{
    return m_columnCount;
}

void MediaLayoutItemViewModel::setRowCount(const int rowCount)
{
    if (m_rowCount != rowCount) {
        m_rowCount = rowCount;
        emit rowCountChanged();
        emit itemUpdated();
    }
}

int MediaLayoutItemViewModel::rowCount() const
{
    return m_rowCount;
}

void MediaLayoutItemViewModel::setRotation(const int rotation)
{
    if (m_rotation != rotation) {
        m_rotation = rotation;
        emit rotationChanged();
        emit itemUpdated();
    }
}

int MediaLayoutItemViewModel::rotation() const
{
    return m_rotation;
}

void MediaLayoutItemViewModel::setBrightness(const qreal brightness)
{
    if (m_brightness != brightness) {
        m_brightness = brightness;
        emit brightnessChanged();
        emit itemUpdated();
    }
}

qreal MediaLayoutItemViewModel::brightness() const
{
    return m_brightness;
}

void MediaLayoutItemViewModel::setContrast(const qreal contrast)
{
    if (m_contrast != contrast) {
        m_contrast = contrast;
        emit contrastChanged();
        emit itemUpdated();
    }
}

qreal MediaLayoutItemViewModel::contrast() const
{
    return m_contrast;
}

void MediaLayoutItemViewModel::setWebpageId(const QString &webpageId)
{
    if (m_webPageId != webpageId) {
        m_webPageId = webpageId;
        updateWebpageInfo();
        emit webPageIdChanged();
    }
}

QString MediaLayoutItemViewModel::webPageId() const
{
    return m_webPageId;
}

void MediaLayoutItemViewModel::setWebPageName(const QString &webpageName)
{
    if (m_webPageName != webpageName) {
        m_webPageName = webpageName;
        emit webPageNameChanged();
    }
}

QString MediaLayoutItemViewModel::webPageName() const
{
    return m_webPageName;
}

void MediaLayoutItemViewModel::setWebPageUrl(const QUrl &webpageUrl)
{
    if (m_webPageUrl != webpageUrl) {
        m_webPageUrl = webpageUrl;
        emit webPageUrlChanged();
    }
}

QUrl MediaLayoutItemViewModel::webPageUrl() const
{
    return m_webPageUrl;
}

void MediaLayoutItemViewModel::setWebPageUseAuth(const bool useAuth)
{
    if (m_webPageUseAuth != useAuth) {
        m_webPageUseAuth = useAuth;
        emit webPageAuthChanged();
    }
}

bool MediaLayoutItemViewModel::webPageUseAuth() const
{
    return m_webPageUseAuth;
}

void MediaLayoutItemViewModel::setWebPageUser(const QString &webpageUser)
{
    if (m_webPageUser != webpageUser) {
        m_webPageUser = webpageUser;
        emit webPageAuthChanged();
    }
}

QString MediaLayoutItemViewModel::webPageUser() const
{
    return m_webPageUser;
}

void MediaLayoutItemViewModel::setWebPagePassword(const QString &webpagePassword)
{
    if (m_webPagePassword != webpagePassword) {
        m_webPagePassword = webpagePassword;
        emit webPageAuthChanged();
    }
}

QString MediaLayoutItemViewModel::webPagePassword() const
{
    return m_webPagePassword;
}


void MediaLayoutItemViewModel::updateWebpageInfo()
{
#ifndef MEDIA_FILE_ONLY
    Wisenet::Core::Webpage wp;
    bool result = false;

    // 2022.12.29. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    // 2023.01.02. coverity
    if (db != nullptr) {
        result = db->FindWebpage(m_webPageId, wp);
    }

    if (result) {
        SPDLOG_DEBUG("MediaLayoutItemViewModel::updateWebpageInfo(), id={}", m_webPageId.toStdString());

        QString name = QString::fromStdString(wp.name);
        QString url = QString::fromStdString(wp.url);
        bool useAuth = wp.useAuth;
        QString user = QString::fromStdString(wp.userId);
        QString password = QString::fromStdString(wp.userPw);

        bool nameChanged = false;
        bool urlChanged = false;
        bool authChanged = false;
        bool userChanged = false;
        bool passwordChanged = false;

        //qDebug() << "webItem name: from" << m_webPageName << " to " << name;
        //qDebug() << "webItem url: from" << m_webPageUrl.toString() << " to " << url;
        //qDebug() << "webItem useAuth: from" << m_webPageUseAuth << " to " << useAuth;
        //qDebug() << "webItem user: from" << m_webPageUser << " to " << user;
        //qDebug() << "webItem password: from" << m_webPagePassword << " to " << password;

        if(m_webPageName != name)
        {
            nameChanged = true;
            m_webPageName = name;
        }

        if(m_webPageUrl.toString() != url)
        {
            urlChanged = true;
            m_webPageUrl = url;
        }

        if(m_webPageUseAuth != useAuth)
        {
            authChanged = true;
            m_webPageUseAuth = useAuth;
        }

        if(m_webPageUser != user)
        {
            userChanged = true;
            m_webPageUser = user;
        }

        if(m_webPagePassword != password)
        {
            passwordChanged = true;
            m_webPagePassword = password;
        }

        if(urlChanged)
            emit webPageUrlChanged();

        if(nameChanged)
            emit webPageNameChanged();

        if(authChanged || userChanged || passwordChanged)
        {
            emit webPageAuthChanged();
            emit webpageClear();
        }
    }
    else {
        SPDLOG_INFO("MediaLayoutItemViewModel::updateWebpageInfo() cannot find webpage ID={}", m_webPageId.toStdString());
    }
#endif
}

void MediaLayoutItemViewModel::initPosition(const QRect &r)
{
    m_column = r.x();
    m_row = r.y();
    m_columnCount = r.width();
    m_rowCount = r.height();
}

void MediaLayoutItemViewModel::setLocalResource(const QUrl &path)
{
    if (m_localResource != path) {
        m_localResource = path;
        emit localResourceChanged();

        updateVerified();
    }
}

QUrl MediaLayoutItemViewModel::localResource() const
{
    return m_localResource;
}

void MediaLayoutItemViewModel::setVerifiedInfo(const QString &result)
{
    if (m_verifiedInfo != result){
        m_verifiedInfo = result;
        emit verifiedInfoChanged();
    }
}

QString MediaLayoutItemViewModel::verifiedInfo() const
{
    return m_verifiedInfo;
}

void MediaLayoutItemViewModel::setDeviceId(const QString &deviceId)
{
    if (m_deviceId != deviceId) {
        m_deviceId = deviceId;
        emit deviceIdChagned();
    }
}

QString MediaLayoutItemViewModel::deviceId() const
{
    return m_deviceId;
}

void MediaLayoutItemViewModel::setChannelId(const QString &channelId)
{
    if (m_channelId != channelId) {
        m_channelId = channelId;
        emit channelIdChanged();
    }
}

QString MediaLayoutItemViewModel::channelId() const
{
    return m_channelId;
}

void MediaLayoutItemViewModel::setZoomTargetId(const QString &zoomTarget)
{
    if (m_zoomTargetId != zoomTarget) {
        m_zoomTargetId = zoomTarget;
        emit zoomTargetIdChanged();
    }
}

QString MediaLayoutItemViewModel::zoomTargetId() const
{
    return m_zoomTargetId;
}

QRectF MediaLayoutItemViewModel::viewPort() const
{
    return m_viewPort;
}

void MediaLayoutItemViewModel::setViewPort(const QRectF normalizedRect)
{
    if (m_viewPort != normalizedRect) {
        m_viewPort = normalizedRect;
        emit viewPortChanged();
        emit itemUpdated();
    }
}
bool MediaLayoutItemViewModel::fisheyeDewarpEnable() const
{
    return m_fisheyeDewarpEnable;
}

void MediaLayoutItemViewModel::setFisheyeDewarpEnable(const bool enable)
{
    if (m_fisheyeDewarpEnable != enable) {
        m_fisheyeDewarpEnable = enable;
        emit fisheyeDewarpEnableChanged();
        emit itemUpdated();
    }
}

MediaLayoutItemViewModel::FisheyeViewMode MediaLayoutItemViewModel::fisheyeViewMode() const
{
    return m_fisheyeViewMode;
}

void MediaLayoutItemViewModel::setFisheyeViewMode(const FisheyeViewMode viewMode)
{
    if (m_fisheyeViewMode != viewMode) {
        m_fisheyeViewMode = viewMode;
        emit fisheyeViewModeChanged();
        emit itemUpdated();
    }
}

QVector<qreal> MediaLayoutItemViewModel::fisheyeViewPosition() const
{
    return m_fisheyeViewPosition;
}

void MediaLayoutItemViewModel::setFisheyeViewPosition(const QVector<qreal> position)
{
    m_fisheyeViewPosition = position;
    emit fisheyeViewPositionChanged();
    emit itemUpdated();
}

bool MediaLayoutItemViewModel::fileFisheyeEnable() const
{
    return m_fileFisheyeEnable;
}

void MediaLayoutItemViewModel::setFileFisheyeEnable(const bool enable)
{
    if (m_fileFisheyeEnable != enable) {
        m_fileFisheyeEnable = enable;
        emit fileFisheyeEnableChanged();
        emit itemUpdated();
    }
}

QString MediaLayoutItemViewModel::fileLensType() const
{
    return m_fileLensType;
}

void MediaLayoutItemViewModel::setFileLensType(const QString &lensType)
{
    if (m_fileLensType != lensType) {
        m_fileLensType = lensType;
        emit fileLensTypeChanged();
        emit itemUpdated();
    }
}

MediaLayoutItemViewModel::FisheyeLensLocation MediaLayoutItemViewModel::fileLensLocation() const
{
    return m_fileLensLocation;
}

void MediaLayoutItemViewModel::setFileLensLocation(const FisheyeLensLocation location)
{
    if (m_fileLensLocation != location) {
        m_fileLensLocation = location;
        emit fileLensLocationChanged();
        emit itemUpdated();
    }
}

bool MediaLayoutItemViewModel::imageBackground() const
{
    return m_isBackground;
}

void MediaLayoutItemViewModel::setImageBackground(const bool isBackground)
{
    if (m_isBackground != isBackground) {
        m_isBackground = isBackground;
        emit imageBackgroundChanged();
        emit itemUpdated();
    }
}

float MediaLayoutItemViewModel::imageOpacity() const
{
    return m_imageOpacity;
}

void MediaLayoutItemViewModel::setImageOpacity(const float opacity)
{
    if (m_imageOpacity != opacity) {
        m_imageOpacity = opacity;
        emit imageOpacityChanged();
        emit itemUpdated();
    }
}

MediaLayoutItemViewModel::ItemFillMode MediaLayoutItemViewModel::itemFillMode() const
{
    return m_itemFillMode;
}

void MediaLayoutItemViewModel::setItemFillMode(const ItemFillMode mode)
{
    if (m_itemFillMode != mode) {
        m_itemFillMode = mode;
        emit itemFillModeChanged();
        emit itemUpdated();
    }
}

MediaLayoutItemViewModel::ItemProfile MediaLayoutItemViewModel::itemProfile() const
{
    return m_itemProfile;
}

void MediaLayoutItemViewModel::setItemProfile(const ItemProfile profile)
{
    if (m_itemProfile != profile) {
        m_itemProfile = profile;
        emit itemProfileChanged();
        emit itemUpdated();
    }
}

bool MediaLayoutItemViewModel::videoStatus() const
{
    return m_videoStatusDisplay;
}

void MediaLayoutItemViewModel::setVideoStatus(const bool status)
{
    if (m_videoStatusDisplay != status) {
        m_videoStatusDisplay = status;
        emit videoStatusChanged();
    }
}

bool MediaLayoutItemViewModel::invalid() const
{
    return m_invalid;
}

void MediaLayoutItemViewModel::setInvalid(const bool invalid)
{
    if (m_invalid != invalid) {
        m_invalid = invalid;
        emit invalidChanged();
    }
}

void MediaLayoutItemViewModel::fromCoreServiceLayoutItem(Wisenet::Core::LayoutItem &layoutItem)
{
    m_itemId = QString::fromStdString(layoutItem.itemID);

    if (layoutItem.itemType == Wisenet::Core::LayoutItem::LayoutItemType::Camera)
        m_itemType = ItemType::Camera;
    else if (layoutItem.itemType == Wisenet::Core::LayoutItem::LayoutItemType::LocalResource)
        m_itemType = ItemType::LocalResource;
    else if (layoutItem.itemType == Wisenet::Core::LayoutItem::LayoutItemType::Webpage) {
        m_itemType = ItemType::Webpage;
    }
    else if (layoutItem.itemType == Wisenet::Core::LayoutItem::LayoutItemType::ZoomTarget) {
        m_itemType = ItemType::ZoomTarget;
        m_viewPort = QRectF(layoutItem.zoomTargetParam.normX, layoutItem.zoomTargetParam.normY,
                            layoutItem.zoomTargetParam.normWidth, layoutItem.zoomTargetParam.normHeight);
    }
    else if (layoutItem.itemType == Wisenet::Core::LayoutItem::LayoutItemType::Image) {
        m_itemType = ItemType::Image;
        m_isBackground = layoutItem.imageParam.isBackground;
        m_imageOpacity = layoutItem.imageParam.opacity;
    }

    if (!layoutItem.deviceID.empty())
        m_deviceId = QString::fromStdString(layoutItem.deviceID);
    if (!layoutItem.channelID.empty())
        m_channelId = QString::fromStdString(layoutItem.channelID);
    if (!layoutItem.resourcePath.empty())
        m_localResource = QString::fromStdString(layoutItem.resourcePath);
    if (!layoutItem.webpageID.empty()) {
        m_webPageId = QString::fromStdString(layoutItem.webpageID);
        updateWebpageInfo();
    }
    if (!layoutItem.zoomTargetParam.zoomTargetID.empty())
        m_zoomTargetId = QString::fromStdString(layoutItem.zoomTargetParam.zoomTargetID);

    if (layoutItem.fillMode == Wisenet::Core::LayoutItem::FillMode::PreserveAspectRatio)
        m_itemFillMode = ItemFillMode::PreserveAspectRatio;
    else if (layoutItem.fillMode == Wisenet::Core::LayoutItem::FillMode::Stretch)
        m_itemFillMode = ItemFillMode::Stretch;

    if (layoutItem.profile == Wisenet::Core::LayoutItem::StreamProfile::Auto)
        m_itemProfile = ItemProfile::Auto;
    else if (layoutItem.profile == Wisenet::Core::LayoutItem::StreamProfile::High)
        m_itemProfile = ItemProfile::High;
    else if (layoutItem.profile == Wisenet::Core::LayoutItem::StreamProfile::Low)
        m_itemProfile = ItemProfile::Low;

    m_fisheyeDewarpEnable = layoutItem.fisheyeDewarpParam.dewarpEnable;
    if (layoutItem.fisheyeDewarpParam.viewMode == Wisenet::FisheyeViewMode::Original) {
        m_fisheyeViewMode = FisheyeViewMode::V_Original;
    }
    else if (layoutItem.fisheyeDewarpParam.viewMode == Wisenet::FisheyeViewMode::Single) {
        m_fisheyeViewMode = FisheyeViewMode::V_Single;
    }
    else if (layoutItem.fisheyeDewarpParam.viewMode == Wisenet::FisheyeViewMode::Quad) {
        m_fisheyeViewMode = FisheyeViewMode::V_Quad;
    }
    else if (layoutItem.fisheyeDewarpParam.viewMode == Wisenet::FisheyeViewMode::Panorama) {
        m_fisheyeViewMode = FisheyeViewMode::V_Panorama;
    }

    m_fisheyeViewPosition.clear();
    for (auto& position : layoutItem.fisheyeDewarpParam.viewParam.positions) {
        m_fisheyeViewPosition.append(position.pan);
        m_fisheyeViewPosition.append(position.tilt);
        m_fisheyeViewPosition.append(position.zoom);
    }
    m_fileFisheyeEnable = layoutItem.fisheyeDewarpParam.fileDewarpEnable;
    if (m_fileFisheyeEnable) {
        if (layoutItem.fisheyeDewarpParam.fileLensLocation == Wisenet::FisheyeLensLocation::Ceiling)
            m_fileLensLocation = FisheyeLensLocation::L_Ceiling;
        else if (layoutItem.fisheyeDewarpParam.fileLensLocation == Wisenet::FisheyeLensLocation::Wall)
            m_fileLensLocation = FisheyeLensLocation::L_Wall;
        else if (layoutItem.fisheyeDewarpParam.fileLensLocation == Wisenet::FisheyeLensLocation::Ground)
            m_fileLensLocation = FisheyeLensLocation::L_Ground;
        m_fileLensType = QString::fromStdString(layoutItem.fisheyeDewarpParam.fileLensType);
    }

    m_rotation = layoutItem.rotation;
    m_column = layoutItem.column;
    m_row = layoutItem.row;
    m_columnCount = layoutItem.columnCount;
    m_rowCount = layoutItem.rowCount;
    m_brightness = (qreal)layoutItem.bncParam.brightness;
    m_contrast = (qreal)layoutItem.bncParam.contrast;
}

Wisenet::Core::LayoutItem MediaLayoutItemViewModel::toCoreServiceLayoutItem()
{
    Wisenet::Core::LayoutItem layoutItem;

    layoutItem.itemID = m_itemId.toStdString();

    if (m_itemType == ItemType::Camera)
        layoutItem.itemType = Wisenet::Core::LayoutItem::LayoutItemType::Camera;
    else if (m_itemType == ItemType::LocalResource)
        layoutItem.itemType = Wisenet::Core::LayoutItem::LayoutItemType::LocalResource;
    else if (m_itemType == ItemType::Webpage)
        layoutItem.itemType = Wisenet::Core::LayoutItem::LayoutItemType::Webpage;
    else if (m_itemType == ItemType::ZoomTarget) {
        layoutItem.itemType = Wisenet::Core::LayoutItem::LayoutItemType::ZoomTarget;
        layoutItem.zoomTargetParam.normX = m_viewPort.x();
        layoutItem.zoomTargetParam.normY = m_viewPort.y();
        layoutItem.zoomTargetParam.normWidth = m_viewPort.width();
        layoutItem.zoomTargetParam.normHeight = m_viewPort.height();
    }
    else if (m_itemType == ItemType::Image) {
        layoutItem.itemType = Wisenet::Core::LayoutItem::LayoutItemType::Image;
        layoutItem.imageParam.isBackground = m_isBackground;
        layoutItem.imageParam.opacity = m_imageOpacity;
    }

    if (!m_deviceId.isEmpty())
        layoutItem.deviceID = m_deviceId.toStdString();
    if (!m_channelId.isEmpty())
        layoutItem.channelID = m_channelId.toStdString();
    if (!m_localResource.isEmpty())
        layoutItem.resourcePath = m_localResource.toString().toStdString();
    if (!m_webPageId.isEmpty())
        layoutItem.webpageID = m_webPageId.toStdString();
    if (!m_zoomTargetId.isEmpty())
        layoutItem.zoomTargetParam.zoomTargetID = m_zoomTargetId.toStdString();

    layoutItem.column = m_column;
    layoutItem.row = m_row;
    layoutItem.columnCount = m_columnCount;
    layoutItem.rowCount = m_rowCount;
    layoutItem.rotation = m_rotation;
    layoutItem.bncParam.brightness = (float)m_brightness;
    layoutItem.bncParam.contrast = (float)m_contrast;

    if (m_itemFillMode == ItemFillMode::PreserveAspectRatio)
        layoutItem.fillMode = Wisenet::Core::LayoutItem::FillMode::PreserveAspectRatio;
    else if (m_itemFillMode == ItemFillMode::Stretch)
        layoutItem.fillMode = Wisenet::Core::LayoutItem::FillMode::Stretch;

    if ( m_itemProfile == ItemProfile::Auto)
        layoutItem.profile = Wisenet::Core::LayoutItem::StreamProfile::Auto;
    else if ( m_itemProfile == ItemProfile::High)
        layoutItem.profile = Wisenet::Core::LayoutItem::StreamProfile::High;
    else if ( m_itemProfile == ItemProfile::Low)
        layoutItem.profile = Wisenet::Core::LayoutItem::StreamProfile::Low;

    layoutItem.fisheyeDewarpParam.dewarpEnable = m_fisheyeDewarpEnable;
    if (m_fisheyeViewMode == FisheyeViewMode::V_Original) {
        layoutItem.fisheyeDewarpParam.viewMode = Wisenet::FisheyeViewMode::Original;
    }
    else if (m_fisheyeViewMode == FisheyeViewMode::V_Single) {
        layoutItem.fisheyeDewarpParam.viewMode = Wisenet::FisheyeViewMode::Single;
    }
    else if (m_fisheyeViewMode == FisheyeViewMode::V_Quad) {
        layoutItem.fisheyeDewarpParam.viewMode = Wisenet::FisheyeViewMode::Quad;
    }
    else if (m_fisheyeViewMode == FisheyeViewMode::V_Panorama) {
        layoutItem.fisheyeDewarpParam.viewMode = Wisenet::FisheyeViewMode::Panorama;
    }

    if (m_fisheyeViewMode != FisheyeViewMode::V_Original && m_fisheyeDewarpEnable) {
        for (int i = 0 ; i < m_fisheyeViewPosition.length() ; i=i+3) {
            float p = m_fisheyeViewPosition[i];
            float t = m_fisheyeViewPosition[i+1];
            float z = m_fisheyeViewPosition[i+2];
            Wisenet::Core::LayoutItem::FisheyeViewParam::PTZ ptz;
            ptz.pan = p;
            ptz.tilt = t;
            ptz.zoom = z;
            layoutItem.fisheyeDewarpParam.viewParam.positions.push_back(ptz);
        }
    }

    layoutItem.fisheyeDewarpParam.fileDewarpEnable = m_fileFisheyeEnable;
    if (m_fileFisheyeEnable) {
        if (m_fileLensLocation == FisheyeLensLocation::L_Ceiling) {
            layoutItem.fisheyeDewarpParam.fileLensLocation = Wisenet::FisheyeLensLocation::Ceiling;
        }
        else if (m_fileLensLocation == FisheyeLensLocation::L_Wall) {
            layoutItem.fisheyeDewarpParam.fileLensLocation = Wisenet::FisheyeLensLocation::Wall;
        }
        else if (m_fileLensLocation == FisheyeLensLocation::L_Ground) {
            layoutItem.fisheyeDewarpParam.fileLensLocation = Wisenet::FisheyeLensLocation::Ground;
        }
        layoutItem.fisheyeDewarpParam.fileLensType = m_fileLensType.toStdString();
    }

    return layoutItem;
}


void MediaLayoutItemViewModel::updateProperty(MediaLayoutItemViewModel *item)
{
    SPDLOG_DEBUG("MediaLayoutItemViewModel::updateProperty()");
    setItemId(item->m_itemId);
    setItemType(item->m_itemType);
    setLocalResource(item->m_localResource);
    setWebpageId(item->m_webPageId);
    setWebPageName(item->m_webPageName);
    setWebPageUrl(item->m_webPageUrl);
    setWebPageUseAuth(item->m_webPageUseAuth);
    setWebPageUser(item->m_webPageUser);
    setWebPagePassword(item->m_webPagePassword);
    setDeviceId(item->m_deviceId);
    setChannelId(item->m_channelId);
    setZoomTargetId(item->m_zoomTargetId);
    setColumn(item->m_column);
    setRow(item->m_row);
    setColumnCount(item->m_columnCount);
    setRowCount(item->m_rowCount);
    setViewPort(item->m_viewPort);
    setRotation(item->m_rotation);
    setBrightness(item->m_brightness);
    setContrast(item->m_contrast);
    setItemProfile(item->m_itemProfile);
    setItemFillMode(item->m_itemFillMode);
    setImageBackground(item->m_isBackground);
    setImageOpacity(item->m_imageOpacity);
    setFisheyeDewarpEnable(item->m_fisheyeDewarpEnable);
    setFisheyeViewMode(item->m_fisheyeViewMode);
    setFisheyeViewPosition(item->m_fisheyeViewPosition);
    setFileFisheyeEnable(item->m_fileFisheyeEnable);
    setFileLensType(item->m_fileLensType);
    setFileLensLocation(item->m_fileLensLocation);
    setLocked(item->m_locked);
}

void MediaLayoutItemViewModel::updateVerified()
{
#ifndef MEDIA_FILE_ONLY
    // 2022.12.29. coverity
    auto signature = QCoreServiceManager::Instance().Signature();
    if (signature == nullptr) {
        return;
    }

    // 2023.01.02. coverity
    QString result = "";
    bool contain = false;
    int resultVal = 6;
    if (signature != nullptr) {
        contain = signature->getVerifyResultMap().contains(m_localResource.toString());
        resultVal = signature->getVerifyResultMap()[m_localResource.toString()];
    }

    if (contain)
    {
        switch(resultVal)
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
        setVerifiedInfo(result);
    }
    else
        setVerifiedInfo("Unknown");
#endif
}

// qml helper function
bool MediaLayoutItemViewModel::isSVG(const QUrl &url)
{
    if (!url.isLocalFile()) {
        return false;
    }

    QString filePath = url.toLocalFile();
    QFileInfo fileInfo(filePath);

    QString fileSuffix = fileInfo.suffix();
    if (fileSuffix == "svg") {
        return true;
    }

    return false;
}
