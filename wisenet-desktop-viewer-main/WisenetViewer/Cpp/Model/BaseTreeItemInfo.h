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

#include <QVector>
#include <QString>
#include <QVariant>
#include "WisenetViewerDefine.h"
#include "MediaLayoutViewModel.h"

using namespace WisenetViewerDefine;

class BaseTreeItemInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid CONSTANT)
    Q_PROPERTY(int itemType READ itemType CONSTANT)
    Q_PROPERTY(int itemStatus READ itemStatus WRITE setItemStatus NOTIFY itemStatusChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString itemId READ itemId CONSTANT)

public:
    explicit BaseTreeItemInfo(QObject *parent = 0);
    BaseTreeItemInfo(const BaseTreeItemInfo &other);
    virtual ~BaseTreeItemInfo();

    QString uuid() const { return m_uuid; }
    int itemType() const { return m_itemType; }
    int itemStatus() const { return m_itemStatus; }
    QString displayName() const { return m_displayName; }
    QString itemId() const {return m_itemId;}

    void setUuid(const QString uuid);
    void setType(const ItemType itemType);
    void setItemStatus(const int status);
    void setDisplayName(QString displayName);
    void setItemId(QString itemId);

signals:
    void itemStatusChanged(int status);
    void displayNameChanged(QString str);

protected:
    QString m_uuid;
    ItemType m_itemType;
    ItemStatus m_itemStatus;
    QString m_displayName;
    QString m_itemId = "";
};
Q_DECLARE_METATYPE(BaseTreeItemInfo)
