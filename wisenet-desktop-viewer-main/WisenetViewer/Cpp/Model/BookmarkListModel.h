#pragma once

#include <QAbstractListModel>
#include <QDebug>
#include "BookmarkItemModel.h"

class BookmarkListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum ModelRoles {
        BookmarkIdRole = Qt::UserRole + 1,
        BookmarkNameRole,
        SourceIdRole,
        CameraNameRole,
        DescriptionRole,
        DisplayTimeRole,
        UtcStartTimeRole,
        UtcEndTimeRole,
        ImageRole,
        ImageWidthRole,
        ImageHeightRole,
    };

    explicit BookmarkListModel(QObject *parent = nullptr);
    ~BookmarkListModel();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const;

    int count(){
        return m_data.size();
    }

    void applySaveChannels(QString id, QString name);
    void setSelectedBookmark(QString bookmarkId);

public slots:
    void insertFirst(BookmarkItemModel* model);
    void append(BookmarkItemModel* model);
    void remove(QString bookmarkId);
    void removeAt(int position);
    void removeAll();

    bool contains(QString bookmarkId);
    BookmarkItemModel* getItem(QString bookmarkId);

    void update(QString bookmarkId, QString bookmarkName, QString sourceId,
                QString cameraName, QString description, QString displayTime,
                long long utcStartTime, long long utcEndTime, QImage& image);

signals:
    void countChanged();
    void selectedBookmarkChanged(QString bookmarkId);

private:
    QList<BookmarkItemModel*> m_data;
    QMap<QString, BookmarkItemModel*> m_dataMap;
};
