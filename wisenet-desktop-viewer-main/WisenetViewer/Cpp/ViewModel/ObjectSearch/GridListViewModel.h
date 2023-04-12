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
#include <QAbstractTableModel>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>
#include <QDateTime>
#include <QSharedPointer>
#include "QCoreServiceManager.h"
#include <QObject>
#include "GridListThread.h"
#include <QImage>

class GridListViewModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(int imageWidth READ imageWidth WRITE setImageWidth NOTIFY imageWidthChanged)
    Q_PROPERTY(int imageHeight READ imageHeight WRITE setImageHeight NOTIFY imageHeightChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
public:
    GridListViewModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void setData(std::vector<Wisenet::Device::MetaAttribute>* metaAttrs);
    void applyFilter(Wisenet::Device::MetaFilter& filter);

    QImage image() { return m_image; }
    int imageWidth();
    int imageHeight();
    bool visible();

    void setImage(const QImage& image);
    void setImageWidth(int imageWidth);
    void setImageHeight(int imageHeight);
    void setVisible(bool visible);
    void clearData();

    Q_INVOKABLE void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    Q_INVOKABLE void setHoveredRow(int rowNum, bool hovered);
    Q_INVOKABLE bool exportCSV(QString path, QString fileName);
    Q_INVOKABLE int totalCount(){return m_totalCount;}
    Q_INVOKABLE void filter(const QStringList &channels);
    Q_INVOKABLE void getBestShotImage(QString deviceID, QString imageUrl);

    enum Role{
        HoveredRole= Qt::UserRole,
        EventImageRole,
        DeviceIdRole,
        ChannelIdRole,
        ServiceTimeRole,
        DeviceTimeRole,
        EventLogIdRole,
        BookmarkIdRole,
        ImageUrlRole,
    };

private slots:
    void processComplated(std::vector<Wisenet::Device::MetaAttribute>* metaAttrs,
                          QSharedPointer<QVector<QVector<QVariant>>> data,
                          QSharedPointer<QVector<int>> dateTimeDescIndex,
                          QSharedPointer<QVector<int>> cameraDescIndex,
                          QVector<QStringList> cameraList,
                          QSet<QString> metaAttrSet);

signals:
    void totalCountChanged();
    void searchStarted();
    void searchFinished(QVector<QStringList> cameraList);
    void channelFilterChanged(QVector<QStringList> cameraList);
    void imageChanged();
    void imageWidthChanged(int imageWidth);
    void imageHeightChanged(int imageWidth);
    void visibleChanged(bool visible);
private:
    QVariant getDataDisplayRole(const QModelIndex &index) const;
    QVariant getDataEventImageRole(const QModelIndex &index) const;
    QVariant getDataFromMetaAttrs(const QModelIndex &index, Role roleName) const;
    QVariant getDataBookmarkIdRole(const QModelIndex &index) const;
    QString GetDeletedDeviceName(std::string &deviceID);
    QString GetDeletedChannelName(std::string &deviceID, std::string channelID);

    void sortFilter();

    bool containsChannelFilter(int index);
    bool containsFilteringOption(int index);

    std::map<int,QVector<int>> m_cameraName;
    std::map<int,QVector<int>> m_eventType;

    int m_hoveredRow;

    GridListThread m_thread;

    std::vector<Wisenet::Device::MetaAttribute>* m_metaAttrs = nullptr;
    std::shared_ptr<std::map<Wisenet::uuid_string,Wisenet::Core::DeletedDevice>> m_deletedDevices;
    int m_totalCount;

    QSharedPointer<QVector<QVector<QVariant>>> m_data;

    QVector<int> m_index;

    QSharedPointer<QVector<int>> m_dateTimeDescIndex;
    QSharedPointer<QVector<int>> m_cameraDescIndex;

    int m_sortIndex = 0;
    Qt::SortOrder m_sortOrder;
    QStringList m_filterChannels;
    Wisenet::Device::MetaFilter m_filter;
    bool m_isFiltering = false;
    QImage m_image;
    int m_width = 0;
    int m_height = 0;
    int m_imageWidth = 0;
    int m_imageHeight = 0;
    bool m_visible = false;
};

