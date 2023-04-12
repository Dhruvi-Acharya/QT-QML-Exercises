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

#ifndef BOOKMARKMODEL_H
#define BOOKMARKMODEL_H

#include <QAbstractItemModel>
#include <unordered_set>
#include <QImage>
#include "CoreService/CoreServiceStructure.h"
#include "QCoreServiceManager.h"
#include "MediaControlModel.h"

class BookmarkModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString tags READ tags WRITE setTags NOTIFY tagsChanged)
    Q_PROPERTY(qint64 startTime READ startTime WRITE setStartTime NOTIFY startTimeChanged)
    Q_PROPERTY(qint64 endTime READ endTime WRITE setEndTime NOTIFY endTimeChanged)
    Q_PROPERTY(QString cameraName READ cameraName NOTIFY cameraNameChanged)
    Q_PROPERTY(QString deviceId READ deviceId)
    Q_PROPERTY(QString channelId READ channelId)
    Q_PROPERTY(int trackId READ trackId)

public:
    enum RoleNames
    {
        IdRole = Qt::UserRole,
        NameRole,
        DescriptionRole,
        TagsRole,
        StartTimeRole,
        EndTimeRole,
        SelectedRole,
    };

    explicit BookmarkModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Data edit functionality:
    void insert(int index, const Wisenet::Core::Bookmark& bookmark);
    void append(const Wisenet::Core::Bookmark& bookmark);
    void remove(int index);
    void clear();

    // Q_PROPERTY
    QString id() { return QString(m_selectedBookmark.bookmarkID.c_str()); }
    QString name() { return QString(m_selectedBookmark.name.c_str()); }
    QString description() { return QString(m_selectedBookmark.description.c_str()); }
    QString tags() { return QString(m_selectedBookmark.tag.c_str()); }
    qint64 startTime() { return m_selectedBookmark.startTime; }
    qint64 endTime() { return m_selectedBookmark.endTime; }
    QString cameraName();
    QString deviceId() { return QString(m_selectedBookmark.deviceID.c_str()); }
    QString channelId() { return QString(m_selectedBookmark.channelID.c_str()); }
    int trackId() { return m_selectedBookmark.overlappedID; }
    void setId(QString id);
    void setName(QString name) { m_selectedBookmark.name = name.toStdString(); }
    void setDescription(QString description) { m_selectedBookmark.description = description.toStdString(); }
    void setTags(QString tags) { m_selectedBookmark.tag = tags.toStdString(); }
    void setStartTime(qint64 time) { m_selectedBookmark.startTime = time; }
    void setEndTime(qint64 time) { m_selectedBookmark.endTime = time; }

    // Q_INVOKABLE
    Q_INVOKABLE void connectMediaControlModelSignals(MediaControlModel* sender);
    Q_INVOKABLE void refreshBookmarkDataList(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth);
    Q_INVOKABLE void newBookmark(QString deviceID, QString channelID, int overlappedID, qulonglong startTime, qulonglong endTime, QString eventLogID = "");
    Q_INVOKABLE void saveBookmark();
    Q_INVOKABLE void removeBookmark(QString bookmarkID);
    Q_INVOKABLE void setThumbnailImage(QImage image);

public slots:
    void onCoreServiceEventTriggered(QCoreServiceEventPtr event);
    void onSelectedChannelChanged(const QString& deviceID, const QString& channelID);

protected:
    virtual QHash<int, QByteArray> roleNames() const override;

signals:
    void idChanged(QString id);
    void nameChanged(QString name);
    void descriptionChanged(QString description);
    void tagsChanged(QString tags);
    void startTimeChanged();
    void endTimeChanged();
    void cameraNameChanged();

private:
    QHash<int, QByteArray> m_roleNames;
    std::vector<Wisenet::Core::Bookmark> m_bookmarks;
    std::unordered_map<Wisenet::uuid_string, int> m_idMap;  // bookmarkID to bookmarksIndex
    std::vector<int> m_indexVector;    // modelIndex to bookmarksIndex

    Wisenet::Core::Bookmark m_selectedBookmark;
    Wisenet::uuid_string m_selectedDeviceID = "";
    std::string m_selectedChannelID = "";
};

#endif // BOOKMARKMODEL_H
