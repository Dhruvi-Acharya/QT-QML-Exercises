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
#pragma once

#include <QObject>
#include <QString>
#include <QUrl>
#include <QRectF>
#include "CoreService/CoreServiceStructure.h"
#include "WisenetViewerDefine.h"

using namespace WisenetViewerDefine;

class MediaLayoutViewModel;
class MediaLayoutItemViewModel : public QObject
{
    friend class MediaLayoutViewModel;

    Q_OBJECT
    Q_PROPERTY(QString itemId READ itemId WRITE setItemId NOTIFY itemIdChanged)
    Q_PROPERTY(ItemType itemType READ itemType WRITE setItemType NOTIFY itemTypeChanged)

    Q_PROPERTY(bool locked READ locked WRITE setLocked NOTIFY lockedChanged)
    Q_PROPERTY(int column READ column WRITE setColumn NOTIFY columnChanged)
    Q_PROPERTY(int row READ row WRITE setRow NOTIFY rowChanged)
    Q_PROPERTY(int columnCount READ columnCount WRITE setColumnCount NOTIFY columnCountChanged)
    Q_PROPERTY(int rowCount READ rowCount WRITE setRowCount NOTIFY rowCountChanged)
    Q_PROPERTY(int rotation READ rotation WRITE setRotation NOTIFY rotationChanged)

    Q_PROPERTY(qreal brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
    Q_PROPERTY(qreal contrast READ contrast WRITE setContrast NOTIFY contrastChanged)

    Q_PROPERTY(QRectF viewPort READ viewPort WRITE setViewPort NOTIFY viewPortChanged)

    Q_PROPERTY(QUrl localResource READ localResource WRITE setLocalResource NOTIFY localResourceChanged)
    Q_PROPERTY(QString verifiedInfo READ verifiedInfo WRITE setVerifiedInfo NOTIFY verifiedInfoChanged)

    Q_PROPERTY(QString webPageId READ webPageId WRITE setWebpageId NOTIFY webPageIdChanged)
    Q_PROPERTY(QString webPageName READ webPageName WRITE setWebPageName NOTIFY webPageNameChanged)
    Q_PROPERTY(QUrl webPageUrl READ webPageUrl WRITE setWebPageUrl NOTIFY webPageUrlChanged)
    Q_PROPERTY(bool    webPageUseAuth READ webPageUseAuth WRITE setWebPageUseAuth NOTIFY webPageAuthChanged)
    Q_PROPERTY(QString webPageUser READ webPageUser WRITE setWebPageUser NOTIFY webPageAuthChanged)
    Q_PROPERTY(QString webPagePassword READ webPagePassword WRITE setWebPagePassword NOTIFY webPageAuthChanged)

    Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChagned)
    Q_PROPERTY(QString channelId READ channelId WRITE setChannelId NOTIFY channelIdChanged)
    Q_PROPERTY(QString zoomTargetId READ zoomTargetId WRITE setZoomTargetId NOTIFY zoomTargetIdChanged)

    Q_PROPERTY(bool imageBackground READ imageBackground WRITE setImageBackground NOTIFY imageBackgroundChanged)
    Q_PROPERTY(float imageOpacity READ imageOpacity WRITE setImageOpacity NOTIFY imageOpacityChanged)

    Q_PROPERTY(ItemFillMode itemFillMode READ itemFillMode WRITE setItemFillMode NOTIFY itemFillModeChanged)
    Q_PROPERTY(ItemProfile itemProfile READ itemProfile WRITE setItemProfile NOTIFY itemProfileChanged)

    Q_PROPERTY(bool videoStatus READ videoStatus WRITE setVideoStatus NOTIFY videoStatusChanged)

    /* Fisheye Param */
    Q_PROPERTY(bool fisheyeDewarpEnable READ fisheyeDewarpEnable WRITE setFisheyeDewarpEnable NOTIFY fisheyeDewarpEnableChanged)
    Q_PROPERTY(FisheyeViewMode fisheyeViewMode READ fisheyeViewMode WRITE setFisheyeViewMode NOTIFY fisheyeViewModeChanged)
    Q_PROPERTY(QVector<qreal> fisheyeViewPosition READ fisheyeViewPosition WRITE setFisheyeViewPosition NOTIFY fisheyeViewPositionChanged)
    Q_PROPERTY(bool fileFisheyeEnable READ fileFisheyeEnable WRITE setFileFisheyeEnable NOTIFY fileFisheyeEnableChanged)
    Q_PROPERTY(QString fileLensType READ fileLensType WRITE setFileLensType NOTIFY fileLensTypeChanged)
    Q_PROPERTY(FisheyeLensLocation fileLensLocation READ fileLensLocation WRITE setFileLensLocation NOTIFY fileLensLocationChanged)

    /* 아이템 설정 사이 임시로 invalid로 설정 */
    Q_PROPERTY(bool invalid READ invalid WRITE setInvalid NOTIFY invalidChanged)

public:
    enum ItemType
    {
        NullItem, /* 초기 null item을 위한 speicial type */
        Camera,
        ZoomTarget,
        LocalResource,
        Webpage,
        Image
    };
    Q_ENUM(ItemType)

    enum ItemFillMode
    {
        PreserveAspectRatio,
        Stretch
    };
    Q_ENUM(ItemFillMode)

    enum ItemProfile
    {
        Auto,
        High,
        Low
    };
    Q_ENUM(ItemProfile)

    enum FisheyeViewMode
    {
        V_Original,
        V_Single,
        V_Quad,
        V_Panorama
    };
    Q_ENUM(FisheyeViewMode)

    enum FisheyeLensLocation
    {
        L_Ceiling,
        L_Wall,
        L_Ground
    };
    Q_ENUM(FisheyeLensLocation)

    explicit MediaLayoutItemViewModel(QObject *parent = nullptr);
    ~MediaLayoutItemViewModel();

    // qml helper
    Q_INVOKABLE bool isSVG(const QUrl& url);

    QString uuid() const;

    void setItemId(const QString& itemId);
    QString itemId() const;

    void setItemType(const ItemType& itemType);
    ItemType itemType() const;

    void setLocked(const bool locked);
    bool locked() const;

    void setColumn(const int column);
    int column() const;
    void setRow(const int row);
    int row() const;
    void setColumnCount(const int columnCount);
    int columnCount() const;
    void setRowCount(const int rowCount);
    int rowCount() const;

    void setRotation(const int rotation);
    int rotation() const;

    void setBrightness(const qreal brightness);
    qreal brightness() const;

    void setContrast(const qreal contrast);
    qreal contrast() const;

    void setWebpageId(const QString& webpageId);
    QString webPageId() const;
    void setWebPageName(const QString& webpageName);
    QString webPageName() const;
    void setWebPageUrl(const QUrl& webpageUrl);
    QUrl webPageUrl() const;
    void setWebPageUseAuth(const bool useAuth);
    bool webPageUseAuth() const;
    void setWebPageUser(const QString& webpageUser);
    QString webPageUser() const;
    void setWebPagePassword(const QString& webpagePassword);
    QString webPagePassword() const;

    void setLocalResource(const QUrl& path);
    QUrl localResource() const;
    void setVerifiedInfo(const QString& result);
    QString  verifiedInfo() const;
    void setDeviceId(const QString& deviceId);
    QString deviceId() const;
    void setChannelId(const QString& channelId);
    QString channelId() const;

    void setZoomTargetId(const QString& zoomTarget);
    QString zoomTargetId() const;

    QRectF viewPort() const;
    void setViewPort(const QRectF normalizedRect);

    bool fisheyeDewarpEnable() const;
    void setFisheyeDewarpEnable(const bool enable);
    FisheyeViewMode fisheyeViewMode() const;
    void setFisheyeViewMode(const FisheyeViewMode viewMode);
    QVector<qreal> fisheyeViewPosition() const;
    void setFisheyeViewPosition(const QVector<qreal> position);
    bool fileFisheyeEnable() const;
    void setFileFisheyeEnable(const bool enable);
    QString fileLensType() const;
    void setFileLensType(const QString& lensType);
    FisheyeLensLocation fileLensLocation() const;
    void setFileLensLocation(const FisheyeLensLocation location);
    bool imageBackground() const;
    void setImageBackground(const bool isBackground);
    float imageOpacity() const;
    void setImageOpacity(const float opacity);
    ItemFillMode itemFillMode() const;
    void setItemFillMode(const ItemFillMode mode);
    ItemProfile itemProfile() const;
    void setItemProfile(const ItemProfile profile);

    bool videoStatus() const;
    void setVideoStatus(const bool status);

    bool invalid() const;
    void setInvalid(const bool invalid);

    void fromCoreServiceLayoutItem(Wisenet::Core::LayoutItem& layoutItem);
    Wisenet::Core::LayoutItem toCoreServiceLayoutItem();

    void updateVerified();
public slots:
    void updateProperty(MediaLayoutItemViewModel* item);
    void updateWebpageInfo();
    void initPosition(const QRect& r);

signals:
    void itemIdChanged();
    void itemTypeChanged();
    void lockedChanged();
    void localResourceChanged();
    void webPageIdChanged();
    void deviceIdChagned();
    void channelIdChanged();
    void zoomTargetIdChanged();
    void viewPortChanged();

    void columnChanged();
    void rowChanged();
    void columnCountChanged();
    void rowCountChanged();
    void rotationChanged();
    void brightnessChanged();
    void contrastChanged();

    void imageBackgroundChanged();
    void imageOpacityChanged();
    void itemFillModeChanged();
    void itemProfileChanged();

    void videoStatusChanged();

    void itemUpdated();
    void invalidChanged();

    void webpageClear();
    void webPageNameChanged();
    void webPageUrlChanged();
    void webPageAuthChanged();

    void fisheyeDewarpEnableChanged();
    void fisheyeViewModeChanged();
    void fisheyeViewPositionChanged();
    void fileFisheyeEnableChanged();
    void fileLensTypeChanged();
    void fileLensLocationChanged();
    void verifiedInfoChanged();

private:
    QString m_uuid; // 인스턴스별로 고유한 값
    QString m_itemId; // default는 m_uuid값을 사용하나, 아이템모델 복제시 set 할수 있음.
    ItemType m_itemType = ItemType::NullItem;
    QUrl m_localResource;
    QString m_verifiedInfo;

    QString m_webPageId;
    QString m_webPageName;
    QUrl m_webPageUrl;
    QString m_webPageUser;
    QString m_webPagePassword;
    bool    m_webPageUseAuth = false;

    QString m_deviceId;
    QString m_channelId;
    QString m_zoomTargetId;

    /* Fisheye params */
    bool m_fisheyeDewarpEnable = false;
    FisheyeViewMode m_fisheyeViewMode = FisheyeViewMode::V_Original;
    QVector<qreal> m_fisheyeViewPosition; // x,y,z,x1,y2,z2,x3,y3,z3...
    bool m_fileFisheyeEnable = false;
    FisheyeLensLocation m_fileLensLocation = FisheyeLensLocation::L_Ceiling;
    QString m_fileLensType;

    int m_column  = 0;
    int m_row = 0;
    int m_columnCount = 1;
    int m_rowCount = 1;
    QRectF m_viewPort = QRectF(0,0,1.0,1.0);
    int m_rotation = 0;
    qreal m_brightness = 0.0f;
    qreal m_contrast = 0.0f;

    bool m_isBackground = false;
    float m_imageOpacity = 1.0f;
    ItemFillMode m_itemFillMode = ItemFillMode::PreserveAspectRatio;
    ItemProfile m_itemProfile = ItemProfile::Auto;
    bool    m_videoStatusDisplay = false;

    bool    m_locked = false;

    // 아이템 위치를 새로 잡기 위해서 해당 아이템의 위치값을 무시하는 flag값
    bool m_invalid = false;

};
