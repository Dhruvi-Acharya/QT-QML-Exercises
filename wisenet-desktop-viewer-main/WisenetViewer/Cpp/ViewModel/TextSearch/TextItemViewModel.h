/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd.  All rights reserved.
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

#include <QAbstractListModel>
#include "QCoreServiceManager.h"

class TextItemViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit TextItemViewModel(QObject *parent = nullptr);
    ~TextItemViewModel();

    static TextItemViewModel* getInstance(){
        static TextItemViewModel instance;
        return &instance;
    }

public:
    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    enum Role{
        HoveredRole= Qt::UserRole,
        DateTimeIdRole,
        DeviceNameIdRole,
        PosNameIdRole,
        ChannelIdRole,
        ChannelIdsRole,
        TextDataIdRole,
        BookmarkIdRole,
        IsDeletedChannelRole
    };

private:
    std::vector<Wisenet::Device::TextSearchDataResult> m_textData;
    std::vector<Wisenet::Device::TextPosConfResult> m_confData;

public:
    void setPosConfData(std::vector<Wisenet::Device::TextPosConfResult> confData);
    void setTextItems(std::vector<Wisenet::Device::TextSearchDataResult> textItems);

    Q_INVOKABLE QString getDeviceID(QString deviceName);
    Q_INVOKABLE qint64 getServiceTime(QString deviceTime);
    Q_INVOKABLE QString convertTimestampToDatetime(qint64 timestamp);

private:
    QVariant getDataPlayTimeIdRole(const QModelIndex &index) const;
    QVariant getDataDeviceNameIdRole(const QModelIndex &index) const;
    QVariant getDataPosNameIdRole(const QModelIndex &index) const;
    QVariant getDataChannelIdRole(const QModelIndex &index) const;
    QVariant getDataChannelIdsRole(const QModelIndex &index) const;
    QVariant getDataTextIdRole(const QModelIndex &index) const;
};

