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

#include <QAbstractTableModel>
#include "QCoreServiceManager.h"
#include "TextItemViewModel.h"

class TextSearchViewModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)
    Q_PROPERTY(TextItemViewModel* textItemViewModel READ textItemViewModel NOTIFY textItemViewModelChangedEvent)

public:
    explicit TextSearchViewModel(QObject* parent = nullptr);
    ~TextSearchViewModel();

    static TextSearchViewModel* getInstance(){
        static TextSearchViewModel instance;
        return &instance;
    }

public:
    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    TextItemViewModel* textItemViewModel()
    {
        return &m_textItemViewModel;
    }

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

public:    
    Q_INVOKABLE void initialize();
    Q_INVOKABLE bool search(bool allDevice, QStringList channels, const QString& keyword, const QDateTime &from, const QDateTime &to, const bool &isWholeWord, const bool &isCaseSensitive);
    Q_INVOKABLE int totalCount(){return m_totalCount;}
    Q_INVOKABLE void setHoveredRow(int rowNum, bool hovered);
    Q_INVOKABLE void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    Q_INVOKABLE QString getDeviceID(QString deviceName);
    Q_INVOKABLE qint64 getServiceTime(QString deviceTime);

private:
    void searchPosConf(const std::string deviceID);
    void search(const std::string deviceID, const std::string chID, const QString& keyword, const QDateTime &from, const QDateTime &to, const bool &isWholeWord, const bool &isCaseSensitive);
    void setTextData(std::vector<Wisenet::Device::TextSearchDataResult> textData);

    QVariant getDataPlayTimeIdRole(const QModelIndex &index) const;
    QVariant getDataDeviceNameIdRole(const QModelIndex &index) const;
    QVariant getDataPosNameIdRole(const QModelIndex &index) const;
    QVariant getDataChannelIdRole(const QModelIndex &index) const;
    QVariant getDataChannelIdsRole(const QModelIndex &index) const;
    QVariant getDataTextIdRole(const QModelIndex &index) const;

private:
    TextItemViewModel m_textItemViewModel;

    int m_hoveredRow;
    int m_totalCount;
    int m_sortIndex;

    Qt::SortOrder m_sortOrder;
    QVector<int> m_index;

    // POS 검색 결과
    std::vector<Wisenet::Device::TextSearchDataResult> m_textData;
    // POS 구성 정보
    std::vector<Wisenet::Device::TextPosConfResult> m_confData;
    // 등록 장비 정보 (전체)
    std::map<Wisenet::uuid_string, Wisenet::Device::Device> m_devices;

signals:
    void textItemViewModelChangedEvent(TextItemViewModel* arg);
    void totalCountChanged();
    void searchStarted();
    void searchFinished();
};

