/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#pragma once

#include <QVector>
#include <QString>
#include <QVariant>



class TreeItemInfo
{
    Q_GADGET
    Q_PROPERTY(QString modelName READ modelName CONSTANT)
    Q_PROPERTY(QString userDefineName READ userDefineName CONSTANT)    
    Q_PROPERTY(QString deviceName READ deviceName CONSTANT)
    Q_PROPERTY(QString deviceAddress READ deviceAddress CONSTANT)
    Q_PROPERTY(QString deviceUUID READ deviceUUID CONSTANT)
    Q_PROPERTY(ItemType itemType READ itemType CONSTANT)
    Q_PROPERTY(ItemStatus itemStatus READ itemStatus CONSTANT)

public:
    enum ItemType
    {
        MainRoot = 0,
        DeviceRoot,
        LayoutRoot,
        GroupRoot,
        UserRoot,
        CoreServer,
        Recorder,
        Camera,
        Channel,
        UserGroup,
        User,
        Layout,
        SequenceLayout
    };
    Q_ENUM(ItemType)

    enum ItemStatus
    {
        Loading = 0,
        Normal,
        Abnormal
    };
    Q_ENUM(ItemStatus)

    TreeItemInfo(){};
    TreeItemInfo(
            const ItemType itemType,
            const ItemStatus itemStatus)
        : m_itemType(itemType), m_itemStatus(itemStatus)
    {}

    QString userDefineName() const {return m_userDefineName;}
    QString modelName() const {return m_modelName;}
    QString deviceName() const {return m_deviceName;}
    QString deviceAddress() const {return m_deviceAddress;}
    QString deviceUUID() const { return m_deviceUUID; }
    ItemType itemType() const {return m_itemType;}
    ItemStatus itemStatus() const {return m_itemStatus;}
    int channelNumber() const {return m_channelNumber;}


    void SetNames(QString userDefineName, QString modelName="", QString deviceName="", QString address="", QString deviceUUID="")
    {
        m_userDefineName = userDefineName;
        m_modelName = modelName;
        m_deviceName = deviceName;
        m_deviceAddress = address;
        m_deviceUUID = deviceUUID;
    }

    void SetChannelNumber(const int channelIndex)
    {
        m_channelNumber = channelIndex;
    }

    void SetStatus(const ItemStatus status)
    {
        m_itemStatus = status;
    }
    void SetType(const ItemType itemType)
    {
        m_itemType = itemType;
    }

private:
    ItemType    m_itemType;
    QString     m_userDefineName="";
    QString     m_modelName="";
    QString     m_deviceName="";
    QString     m_deviceAddress="";
    QString     m_deviceUUID = "";
    ItemStatus  m_itemStatus;
    int         m_channelNumber = 0;
};
Q_DECLARE_METATYPE(TreeItemInfo)

class MainTreeItemBase
{
public:
    explicit MainTreeItemBase(const QString& uuid,
                              const TreeItemInfo& itemInfo,
                              MainTreeItemBase *parent = nullptr);
    virtual ~MainTreeItemBase();

    MainTreeItemBase *child(int index);
    int childCount() const;
    int IndexOfParent() const;

    bool insertChild(int position, MainTreeItemBase* child);
    bool pushBackChild(MainTreeItemBase* child);

    bool removeChild(int position);
    MainTreeItemBase *parent();

    TreeItemInfo::ItemType itemType() const
    {
        return m_itemInfo.itemType();
    }
    void setItemStatus(TreeItemInfo::ItemStatus itemStatus)
    {
        m_itemInfo.SetStatus(itemStatus);
    }

    void setData(const TreeItemInfo& itemInfo);
    TreeItemInfo getData();

    void removeAllChild();

protected:
    QVector<MainTreeItemBase*> m_childItems;
    MainTreeItemBase *m_parentItem;

    QString     m_uuid;
    TreeItemInfo m_itemInfo;
};

