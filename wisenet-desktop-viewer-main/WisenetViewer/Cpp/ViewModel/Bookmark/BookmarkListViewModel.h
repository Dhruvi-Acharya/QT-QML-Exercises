#pragma once

#include <QObject>
#include <QDebug>
#include <QDateTime>
#include "QCoreServiceManager.h"
#include "QLocaleManager.h"
#include "LogSettings.h"
#include "../../Model/BookmarkListModel.h"
#include "../../Model/BookmarkItemModel.h"
#include "../../Model/BookmarkListSortFilterProxyModel.h"

class BookmarkListViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY (BookmarkListModel* bookmarkListModel READ bookmarkListModel NOTIFY bookmarkListModelChangedEvent)
    Q_PROPERTY (BookmarkListSortFilterProxyModel* bookmarkListSortFilterProxyModel READ bookmarkListSortFilterProxyModel NOTIFY bookmarkListSortFilterProxyModelChangedEvent)

public:
    explicit BookmarkListViewModel(QObject *parent = nullptr);
    ~BookmarkListViewModel();

    BookmarkListModel* bookmarkListModel()
    {
        return &m_bookmarkListModel;
    }

    BookmarkListSortFilterProxyModel* bookmarkListSortFilterProxyModel()
    {
        return &m_bookmarkListSortFilterProxyModel;
    }

    void clear();
    void coreServiceEventTriggered(QCoreServiceEventPtr event);
    void initializeList();

public slots:
    void removeBookmarkItem(QString bookmarkId);

private:
    void Event_FullInfo(QCoreServiceEventPtr event);
    void Event_SaveBookmark(QCoreServiceEventPtr event);
    void Event_RemoveBookmark(QCoreServiceEventPtr event);
    void Event_SaveChannels(QCoreServiceEventPtr event);

    void AddBookmarkItem(const Wisenet::Core::Bookmark& bookmark);
    void UpdateBookmarkItem(const Wisenet::Core::Bookmark& bookmark);

signals:
    void bookmarkListModelChangedEvent(BookmarkListModel* arg);
    void bookmarkListSortFilterProxyModelChangedEvent(BookmarkListSortFilterProxyModel* arg);

private:
    BookmarkListModel m_bookmarkListModel;
    BookmarkListSortFilterProxyModel m_bookmarkListSortFilterProxyModel;

};
