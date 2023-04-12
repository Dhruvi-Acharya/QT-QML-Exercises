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
#include "BaseTreeModel.h"
#include "DragItemListModel.h"

using namespace WisenetViewerDefine;

class ResourceTreeModel : public BaseTreeModel
{
    Q_OBJECT
public:
    explicit ResourceTreeModel(QObject *parent = nullptr);
    virtual ~ResourceTreeModel();

    BaseTreeItemInfo* newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name);
    BaseTreeItemInfo* newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name, const QString& modelName, const QString& deviceName, const QString& ipAddress, const int channelNumber = 0, const QString& deviceUuid = "");

    void coreServiceEventTriggered(QCoreServiceEventPtr event);
    void clear();
    void initializeTree();

public slots:
    bool checkAcceptable(QString parentUuid, DragItemListModel* model);
    void moveGroup(QString groupId, QStringList itemList);
    void dropSource(const QModelIndex& itemIndex, QString parentUuid, DragItemListModel* model);
    void deleteItems(QStringList groups, QStringList cameras, QStringList webpages, QStringList localFiles, bool removeChannels = false);
    void newGroupWithChannels(QString groupName, QStringList cameras, QStringList webpages);
    void changeDisplayName(QString uuid, int itemType, QString name);

    void addGroup(const QModelIndex& parentIndex, QString name);
    void addLocalFile(const QString fileUrl);
    void removeLocalFile(const QString fileUrl);
    void openLocalDirPath(const QString &dirPath);

    // reload file item list
    void onEndExport(const QString fileDir);
    void reloadFileItems();
    // save setting
    void onLocalRecordingPathChanged();

    // digital signature
    void verifySignature(const QStringList wnmList);

signals:
    void invalidate();
    void expandRoot();
    void expandSelectedIndex();
    void verifyFinished();

private:
    void composeChildrenList(QString groupId, QStringList& groupList, QSet<QString>& groupSet, QSet<QString>& itemSet);
    bool checkToCircle(QString uuid, QString moveParent);
    bool checkAncestor(QString groupId, QSet<QString> sourceSet);

    void SetupCategoryTree();
    void ChangeParentGroup(QString newGroupId, QStringList children);

    void Request_SaveChannels(std::vector<Wisenet::Device::Device::Channel>& channels);

    void Request_SaveGroup(Wisenet::Core::Group& group);
    void Request_RemoveGroup(const QString& uuid);
    void Request_AddGroupMapping(QString groupId, QList<QString> channelList);
    void Request_RemoveGroupMapping(QString groupId, QList<QString> channelList);
    void Request_UpdateGroupMapping(QString from, QString to, QList<QString> channelList);

    void Request_SaveWebage(Wisenet::Core::Webpage& webpage);
    void Request_RemoveWebpage(const QStringList& list);

    void Event_FullInfo(QCoreServiceEventPtr event);

    void Event_SaveGroup(QCoreServiceEventPtr event);
    void Event_RemoveGroup(QCoreServiceEventPtr event);
    void Event_AddGroupMapping(QCoreServiceEventPtr event);
    void Event_RemoveGroupMapping(QCoreServiceEventPtr event);
    void Event_UpdateGroupMapping(QCoreServiceEventPtr event);

    void Event_AddDevice(QCoreServiceEventPtr event);
    void Event_RemoveDevices(QCoreServiceEventPtr event);
    void Event_DeviceStatus(QCoreServiceEventPtr event);
    void Event_ChannelStatus(QCoreServiceEventPtr event);
    void Event_SaveChannels(QCoreServiceEventPtr event);

    void Event_SaveWebpage(QCoreServiceEventPtr event);
    void Event_RemoveWebpages(QCoreServiceEventPtr event);

    void Event_AddLocalFile(QCoreServiceEventPtr event);
    void Event_RemoveLocalFile(QCoreServiceEventPtr event);
    void Event_UpdateDevices(QCoreServiceEventPtr event);

    void Event_UpdateLocalFileIcon(QCoreServiceEventPtr event);

    TreeItemModel* AddTreeGroupItem(const Wisenet::Core::Group& group);
    void UpdateTreeGroupItem(const Wisenet::Core::Group& group);

    void AddTreeChannelItemToRoot(const Wisenet::Device::Device::Channel& channel);
    void AddTreeChannelItem(const Wisenet::Core::Group& group);

    void UpdateTreeChannelItem(const Wisenet::Device::Device::Channel& channel);

    void RemoveTreeChannelItem(const QString& channelId);

    void AddTreeDeviceItem(const Wisenet::Device::Device& device,
                           const Wisenet::Device::DeviceStatus& deviceStatus);

    void AddTreeWebpageItem(const Wisenet::Core::Webpage& webpage);
    void UpdateTreeWebpageItem(const Wisenet::Core::Webpage& webpage);

    void AddLocalDirectoryItem(const QString& folderUrl);
    void AddLocalFileItem(TreeItemModel* parentModel, const QString& fileUrl);
    void RemoveLocalFileItem(const QString& fileUrl);

    void GetLocalDirs(QMap<QString, QStringList>& dirFilePair, QSet<QString>& urlSet);
    void GetLocalFiles(const QString& dirUrl, const QString& dirPath, QMap<QString, QStringList>& dirFilePair, QSet<QString>& urlSet);

    TreeItemModel* m_resourceRootModel;
    TreeItemModel* m_localFileRootModel;

    QMap<QString, TreeItemModel*> m_treeItemModelMap;

    QFileSystemWatcher m_watcher;
    QMetaObject::Connection m_fileWatcherConnection;
};

