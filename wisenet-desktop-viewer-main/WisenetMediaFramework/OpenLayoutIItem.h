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

class OpenLayoutItem
{
    Q_GADGET
    // 0 : channel, 1 : webpage, 2 : localresource, 3: layout
    Q_PROPERTY(int itemTypeId READ itemTypeId WRITE setItemTypeId)
    Q_PROPERTY(QString itemType READ itemType WRITE setItemType)
    Q_PROPERTY(QString itemId READ itemId WRITE setItemId)
    Q_PROPERTY(QString subId READ subId WRITE setSubId)
    Q_PROPERTY(QString name READ name WRITE setName)

public:
    int itemTypeId() const {return m_itemTypeId;}
    QString itemType() const {return m_itemType;}
    QString itemId() const {return m_itemId;}
    QString subId() const {return m_subId;}
    QString name() const {return m_name;}
    void setItemTypeId(const int itemTypeId) {m_itemTypeId = itemTypeId;}
    void setItemType(const QString &itemType) {m_itemType = itemType;}
    void setItemId(const QString &itemId) {m_itemId = itemId;}
    void setSubId(const QString &subId) {m_subId = subId;}
    void setName(const QString &name) {m_name = name;}

private:
    int m_itemTypeId = 0;
    QString m_itemType;
    QString m_itemId;
    QString m_subId;
    QString m_name;
};

Q_DECLARE_METATYPE(OpenLayoutItem)
