#pragma once
#include <QObject>

// TODO : type정보를 읽기 위해서 include
#include "WisenetViewerDefine.h"

class DragItemModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid)
    Q_PROPERTY(QString channelId READ channelId WRITE setChannelId)
    Q_PROPERTY(int itemType READ itemType WRITE setItemType)
public:
    explicit DragItemModel(QObject *parent = nullptr);
    QString uuid() const;
    void setUuid(const QString &uuid);
    QString channelId() const;
    void setChannelId(const QString &channelId);
    int itemType() const;
    void setItemType(int itemType);

private:
    QString m_uuid;
    QString m_channelId;
    int m_itemType = 0;
};
