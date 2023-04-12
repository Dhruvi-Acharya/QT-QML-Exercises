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
#include <QPoint>
#include <QQmlListProperty>
#include <QHash>
#include <QRect>
#include "MediaLayoutItemViewModel.h"
#include "QCoreServiceManager.h"
#include "CoreService/CoreServiceStructure.h"
#include "OpenLayoutIItem.h"
#include "DragItemListModel.h"

class MediaLayoutViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)    
    Q_PROPERTY(QString layoutId READ layoutId WRITE setLayoutId NOTIFY layoutIdChanged)
    Q_PROPERTY(QString ownerId READ ownerId WRITE setOwnerId NOTIFY ownerIdChanged)
    Q_PROPERTY(bool locked READ locked WRITE setLocked NOTIFY lockedChanged)
    Q_PROPERTY(int cellRatioW READ cellRatioW WRITE setCellRatioW NOTIFY cellRatioWChanged)
    Q_PROPERTY(int cellRatioH READ cellRatioH WRITE setCellRatioH NOTIFY cellRatioHChanged)
    Q_PROPERTY(MediaLayoutItemViewModel::ItemFillMode layoutFillMode READ layoutFillMode WRITE setLayoutFillMode NOTIFY layoutFillModeChanged)
    Q_PROPERTY(MediaLayoutItemViewModel::ItemProfile layoutProfile READ layoutProfile WRITE setLayoutProfile NOTIFY layoutProfileChanged)
    Q_PROPERTY(QQmlListProperty<MediaLayoutItemViewModel> layoutItems READ layoutItems)
    Q_PROPERTY(QString loginId READ loginId NOTIFY loginIdChanged)
    Q_PROPERTY(bool userAdmin READ userAdmin NOTIFY userAdminChanged)


    // Core Service에는 저장하지 않는 정보
    Q_PROPERTY(bool videoStatus READ videoStatus WRITE setVideoStatus NOTIFY videoStatusChanged)
    Q_PROPERTY(bool saveStatus READ saveStatus WRITE setSaveStatus NOTIFY saveStatusChanged)

    // ViewingGrid의 ViewModel 사용 여부 (ViewingGrid 내부 뷰모델로 사용시에는 업데이트 시나리오가 다름)
    Q_PROPERTY(bool viewingGridViewModel READ viewingGridViewModel WRITE setViewingGridViewModel NOTIFY viewingGridViewModelChanged)

    Q_PROPERTY(int maxVideoItems READ maxVideoItems)
    Q_PROPERTY(int maxWebItems READ maxWebItems)

    // Masking
    Q_PROPERTY(bool isMaskingMode READ isMaskingMode WRITE setIsMaskingMode NOTIFY isMaskingModeChanged)

public:
    explicit MediaLayoutViewModel(QObject *parent = nullptr, bool receiveCoreServiceEvent = true);
    ~MediaLayoutViewModel();

    void loadFromCoreService(Wisenet::Core::Layout& layout);

    Q_INVOKABLE bool isAcceptableUrl(const QUrl& url);
    Q_INVOKABLE bool isVideoFile(const QUrl& url);
    Q_INVOKABLE bool isNewLayout();
    Q_INVOKABLE QRect getNewItemPosition(const int w, const int h);
    Q_INVOKABLE QRect getNewItemPositionWithStartPos(const int w, const int h,
                                                     const int startX, const int startY);
    Q_INVOKABLE static void openLocalDirPath(const QString& dirPath);
    Q_INVOKABLE QVariantList getChannelsInGroup(const QString& groupID);
    Q_INVOKABLE QVariantList parseDragItemListModel(DragItemListModel* source);
    Q_INVOKABLE QVariantList parseDragItemListModelForMaskingMode(DragItemListModel* source);
    Q_INVOKABLE void updateVerifyResult(QString filePath, int verifyResult);

    QString uuid() const;

    void setName(const QString &name);
    QString name() const;

    void setLayoutId(const QString &id);
    QString layoutId() const;

    void setOwnerId(const QString &id);
    QString ownerId() const;

    /* viewing grid에서만 사용 */
    QString loginId() const;
    void setLoginId(const QString loginId);

    void setLocked(const bool locked);
    bool locked() const;

    void setUserAdmin(const bool isAdmin);
    bool userAdmin() const;

    void setCellRatioW(const int ratioW);
    int cellRatioW() const;
    void setCellRatioH(const int ratioH);
    int cellRatioH() const;

    int maxVideoItems() const;
    int maxWebItems() const;

    MediaLayoutItemViewModel::ItemFillMode layoutFillMode() const;
    void setLayoutFillMode(const MediaLayoutItemViewModel::ItemFillMode mode);
    MediaLayoutItemViewModel::ItemProfile layoutProfile() const;
    void setLayoutProfile(const MediaLayoutItemViewModel::ItemProfile profile);

    bool videoStatus() const;
    void setVideoStatus(const bool status);

    void setSaveStatus(const bool flag);
    bool saveStatus() const;

    void setViewingGridViewModel(const bool flag);
    bool viewingGridViewModel() const;

    void setIsMaskingMode(const bool isMaskingMode);
    bool isMaskingMode() const;

    QQmlListProperty<MediaLayoutItemViewModel> layoutItems();

    void cleanUp();

public slots:
    void saveToCoreService();
    void saveAsToCoreService(const QString& layoutName);

    // Layout 속성복사 (layout items는 copy하지 않는다.)
    void updateProperty(MediaLayoutViewModel* layout);

    // 외부에서 itemModel을 생성하여 주입하는 경우, 이경우 parent!=this
    // 뷰잉그리드에서만 사용
    void addItem(MediaLayoutItemViewModel* itemModel);

    // deviceID, channelID로 카메라 아이템 추가, 이경우 parent==this
    // 외부 리소스에서 MediaLayoutViewModel을 모델로 사용할 때 호출
    void addCameraItem(const QString& deviceId, QString channelId);
    void addLocalVideoItem(const QString& fileUrl);
    void addLocalImageItem(const QString& fileUrl);
    void addWebpageItem(const QString& webPageId);

    // itemId로 아이템 삭제
    void removeItem(const QString& itemId, bool changeSaveStstus = true);

    // 외부 MediaLayoutViewModel과 데이터 동기화
    void syncAddedItems(QVariantList items);
    void syncRemovedItems(QList<QString> itemIds);
    void syncUpdatedItems(QVariantList items);
    void syncPropertyUpdatdLayout(MediaLayoutViewModel* otherModel);

    void onCoreServiceEventTriggered(QCoreServiceEventPtr event);
signals:
    void nameChanged();
    void lockedChanged();
    void cellRatioWChanged();
    void cellRatioHChanged();
    void ownerIdChanged();
    void layoutIdChanged();
    void layoutFillModeChanged();
    void layoutProfileChanged();
    void videoStatusChanged();
    void saveStatusChanged();
    void viewingGridViewModelChanged();
    void isMaskingModeChanged();
    void layoutModelUpdated(MediaLayoutViewModel* newModel);
    void deviceRemoved(QList<QString> deviceIDs);
    void cameraRemoved(QList<QString> cameraIDs);
    void loginIdChanged();
    void userAdminChanged();
    void webpageSaved(QString webpageId);
    void verificationStatusChanged(QString filePath, int result);

private:
    QVariantList buildDropList(const QCoreServiceDatabase::GroupResource& groupResource,
                               const std::set<std::string>& localResource);
    bool isVideoFileSuffix(const QString fileSuffix);
    bool isVideoFileForMaskingSuffix(const QString fileSuffix);
    bool isImageFileSuffix(const QString fileSuffix);

    QRect getCurrentLayoutRect(const bool ignoreBackgroundRect = true);
    bool isEmptyCell(int x, int y , int w, int h, const bool ignoreBackgroundRect = true);

    void save(const bool saveAs = false, const QString& newName="");
private:
    bool    m_viewingGridViewModel = false;
    bool    m_saveStatus = true;
    QString m_name;
    QString m_uuid;
    QString m_layoutId;
    QString m_ownerId;
    QString m_loginId;
    bool    m_locked = false;
    bool    m_userAdmin = true;
    int     m_cellRatioW = 160;
    int     m_cellRatioH = 90;
    bool    m_videoStatusDisplay = false;
    int     m_maxVideoItems = 64;
    int     m_maxWebItems = 8;
    bool    m_isMaskingMode = false;

    MediaLayoutItemViewModel::ItemFillMode m_layoutFillMode = MediaLayoutItemViewModel::ItemFillMode::PreserveAspectRatio;
    MediaLayoutItemViewModel::ItemProfile m_layoutProfile = MediaLayoutItemViewModel::ItemProfile::Auto;

    QHash<QString, MediaLayoutItemViewModel *> m_items;
    QList<MediaLayoutItemViewModel *> m_itemlist;
};

